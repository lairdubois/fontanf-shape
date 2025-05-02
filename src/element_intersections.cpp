#include "shape/element_intersections.hpp"

#include "shape/intersection_tree.hpp"
#include "shape/self_intersections_removal.hpp"

//#include <iostream>

using namespace shape;

namespace
{

// Helper function to compute line-line intersections
std::vector<Point> compute_line_line_intersections(
        const ShapeElement& line1,
        const ShapeElement& line2,
        bool strict)
{
    LengthDbl x1 = line1.start.x;
    LengthDbl y1 = line1.start.y;
    LengthDbl x2 = line1.end.x;
    LengthDbl y2 = line1.end.y;
    LengthDbl x3 = line2.start.x;
    LengthDbl y3 = line2.start.y;
    LengthDbl x4 = line2.end.x;
    LengthDbl y4 = line2.end.y;

    // Check if both line segments are colinear.
    LengthDbl denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (denom == 0.0) {
        if (strict)
            return {};

        // If they are colinear, check if they are aligned.
        LengthDbl denom_2 = (x1 - x2) * (y3 - y1) - (y1 - y2) * (x3 - x1);
        if (denom_2 != 0.0)
            return {};

        // If they are aligned, check if they overlap.
        std::array<ElementPos, 4> sorted_points = {0, 1, 2, 3};
        std::sort(
                sorted_points.begin(),
                sorted_points.end(),
                [&line1, &line2](
                    ElementPos point_pos_1,
                    ElementPos point_pos_2)
                {
                    const Point& point_1 =
                        (point_pos_1 == 0)? line1.start:
                        (point_pos_1 == 1)? line1.end:
                        (point_pos_1 == 2)? line2.start:
                        line2.end;
                    const Point& point_2 =
                        (point_pos_2 == 0)? line1.start:
                        (point_pos_2 == 1)? line1.end:
                        (point_pos_2 == 2)? line2.start:
                        line2.end;
                    if (point_1.x != point_2.x)
                        return point_1.x < point_2.x;
                    return point_1.y < point_2.y;
                });
        if (sorted_points[0] + sorted_points[1] == 1
                || sorted_points[0] + sorted_points[1] == 5) {
            return {};
        }

        // Return the two interior points.
        const Point& point_1 =
            (sorted_points[1] == 0)? line1.start:
            (sorted_points[1] == 1)? line1.end:
            (sorted_points[1] == 2)? line2.start:
            line2.end;
        const Point& point_2 =
            (sorted_points[2] == 0)? line1.start:
            (sorted_points[2] == 1)? line1.end:
            (sorted_points[2] == 2)? line2.start:
            line2.end;
        return {point_1, point_2};
    }

    // Otherwise, compute intersection.
    LengthDbl t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
    LengthDbl u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;

    // Edge cases.
    if (strict) {
        if (!strictly_greater(t, 0)
                || !strictly_lesser(t, 1)
                || !strictly_greater(u, 0)
                || !strictly_lesser(u, 1)) {
            // No intersection.
            return {};
        }
    } else {
        if (strictly_lesser(t, 0)
                || strictly_greater(t, 1)
                || strictly_lesser(u, 0)
                || strictly_greater(u, 1)) {
            // No intersection.
            return {};
        } else if (equal(t, 0.0)) {
            return {line1.start};
        } else if (equal(t, 1.0)) {
            return {line1.end};
        } else if (equal(u, 0.0)) {
            return {line2.start};
        } else if (equal(u, 1.0)) {
            return {line2.end};
        }
    }

    // Standard intersection.
    LengthDbl xp = x1 + t * (x2 - x1);
    LengthDbl yp = y1 + t * (y2 - y1);
    return {{xp, yp}};
}

// Helper function to compute line-arc intersections
std::vector<Point> compute_line_arc_intersections(
        const ShapeElement& line,
        const ShapeElement& arc,
        bool strict)
{
    // x (y1 - y2) + y (x2 - x1) + (x1 y2 - x2 y1) = 0
    LengthDbl xm = arc.center.x;
    LengthDbl ym = arc.center.y;
    LengthDbl a = line.start.y - line.end.y;
    LengthDbl b = line.end.x - line.start.x;
    LengthDbl c = line.end.x * line.start.y
        - line.start.x * line.end.y;
    LengthDbl rsq = squared_distance(arc.center, arc.start);
    LengthDbl c_prime = c - a * xm - b * ym;
    LengthDbl discriminant = rsq * (a * a + b * b) - c_prime * c_prime;
    //std::cout << "discriminant " << discriminant << std::endl;

    // No intersection.
    if (strictly_lesser(discriminant, 0))
        return {};

    // Single intersection point.
    if (equal(discriminant, 0.0)) {
        if (strict)
            return {};

        LengthDbl denom = a * a + b * b;
        LengthDbl eta = (a * c_prime) / denom;
        LengthDbl teta = (b * c_prime) / denom;
        Point p;
        p.x = xm + eta;
        p.y = ym + teta;
        //std::cout << "p " << p.to_string() << std::endl;

        if (equal(p, line.start)) {
            p = line.start;
        } else if (equal(p, line.end)) {
            p = line.end;
        } else if (equal(p, arc.start)) {
            p = arc.start;
        } else if (equal(p, arc.end)) {
            p = arc.end;
        }
        if (line.contains(p) && arc.contains(p))
            return {p};
    }

    std::vector<Point> intersections;
    LengthDbl denom = a * a + b * b;
    LengthDbl eta_1 = (a * c_prime + b * std::sqrt(discriminant)) / denom;
    LengthDbl eta_2 = (a * c_prime - b * std::sqrt(discriminant)) / denom;
    LengthDbl teta_1 = (b * c_prime - a * std::sqrt(discriminant)) / denom;
    LengthDbl teta_2 = (b * c_prime + a * std::sqrt(discriminant)) / denom;
    Point ps[2];
    ps[0].x = xm + eta_1;
    ps[0].y = ym + teta_1;
    ps[1].x = xm + eta_2;
    ps[1].y = ym + teta_2;

    for (Point& p: ps) {
        // Check if any intersection coincides with an arc endpoint
        if (strict) {
            if (equal(p, line.start)) {
                continue;
            } else if (equal(p, line.end)) {
                continue;
            } else if (equal(p, arc.start)) {
                continue;
            } else if (equal(p, arc.end)) {
                continue;
            }
        } else {
            if (equal(p, line.start)) {
                p = line.start;
            } else if (equal(p, line.end)) {
                p = line.end;
            } else if (equal(p, arc.start)) {
                p = arc.start;
            } else if (equal(p, arc.end)) {
                p = arc.end;
            }
        }
        if (line.contains(p) && arc.contains(p))
            intersections.push_back(p);
    }

    return intersections;
}

// Helper function to compute arc-arc intersections
std::vector<Point> compute_arc_arc_intersections(
        const ShapeElement& arc,
        const ShapeElement& arc_2,
        bool strict)
{
    LengthDbl rsq = squared_distance(arc.center, arc.start);
    LengthDbl r2sq = squared_distance(arc_2.center, arc_2.start);
    if (equal(arc.center, arc_2.center)) {
        if (strict)
            return {};
        if (equal(rsq, r2sq)) {
            std::vector<Point> intersections;
            if (arc.contains(arc_2.start))
                intersections.push_back(arc_2.start);
            if (!equal(arc_2.end, arc_2.start)
                    && arc.contains(arc_2.end))
                intersections.push_back(arc_2.end);
            if (!equal(arc.start, arc_2.start)
                    && !equal(arc.start, arc_2.end)
                    && arc_2.contains(arc.start))
                intersections.push_back(arc.start);
            if (!equal(arc.end, arc_2.start)
                    && !equal(arc.end, arc_2.end)
                    && !equal(arc.end, arc.start)
                    && arc_2.contains(arc.end))
                intersections.push_back(arc.end);
            return intersections;
        } else {
            return {};
        }
    }

    LengthDbl xm = arc.center.x;
    LengthDbl ym = arc.center.y;
    LengthDbl xm2 = arc_2.center.x;
    LengthDbl ym2 = arc_2.center.y;
    LengthDbl a = 2 * (xm2 - xm);
    LengthDbl b = 2 * (ym2 - ym);
    LengthDbl c = rsq - (xm * xm) - (ym * ym)
        - r2sq + (xm2 * xm2) + (ym2 * ym2);
    //std::cout << "a " << a << " b " << b << " c " << c << std::endl;

    LengthDbl c_prime = c - a * xm - b * ym;
    LengthDbl discriminant = rsq * (a * a + b * b) - c_prime * c_prime;

    // No intersection.
    if (strictly_lesser(discriminant, 0))
        return {};

    // Single intersection point.
    if (equal(discriminant, 0.0)) {
        if (strict)
            return {};

        LengthDbl denom = a * a + b * b;
        LengthDbl eta = (a * c_prime) / denom;
        LengthDbl teta = (b * c_prime) / denom;
        Point p;
        p.x = xm + eta;
        p.y = ym + teta;
        //std::cout << "p " << p.to_string() << std::endl;

        if (equal(p, arc.start)) {
            p = arc.start;
        } else if (equal(p, arc.end)) {
            p = arc.end;
        } else if (equal(p, arc_2.start)) {
            p = arc_2.start;
        } else if (equal(p, arc_2.end)) {
            p = arc_2.end;
        }
        if (arc.contains(p) && arc_2.contains(p))
            return {p};
    }

    std::vector<Point> intersections;
    LengthDbl denom = a * a + b * b;
    LengthDbl eta_1 = (a * c_prime + b * std::sqrt(discriminant)) / denom;
    LengthDbl eta_2 = (a * c_prime - b * std::sqrt(discriminant)) / denom;
    LengthDbl teta_1 = (b * c_prime - a * std::sqrt(discriminant)) / denom;
    LengthDbl teta_2 = (b * c_prime + a * std::sqrt(discriminant)) / denom;
    Point ps[2];
    ps[0].x = xm + eta_1;
    ps[0].y = ym + teta_1;
    ps[1].x = xm + eta_2;
    ps[1].y = ym + teta_2;

    for (Point& p: ps) {
        // Check if any intersection coincides with an arc endpoint
        if (strict) {
            if (equal(p, arc.start)) {
                continue;
            } else if (equal(p, arc.end)) {
                continue;
            } else if (equal(p, arc_2.start)) {
                continue;
            } else if (equal(p, arc_2.end)) {
                continue;
            }
        } else {
            if (equal(p, arc.start)) {
                p = arc.start;
            } else if (equal(p, arc.end)) {
                p = arc.end;
            } else if (equal(p, arc_2.start)) {
                p = arc_2.start;
            } else if (equal(p, arc_2.end)) {
                p = arc_2.end;
            }
        }
        if (arc.contains(p) && arc_2.contains(p))
            intersections.push_back(p);
    }

    return intersections;
}

}

std::vector<Point> shape::compute_intersections(
        const ShapeElement& element_1,
        const ShapeElement& element_2,
        bool strict)
{
    if (element_1.type == ShapeElementType::LineSegment
            && element_2.type == ShapeElementType::LineSegment) {
        // Line segment - Line segment intersection
        return compute_line_line_intersections(element_1, element_2, strict);
    } else if (element_1.type == ShapeElementType::LineSegment
            && element_2.type == ShapeElementType::CircularArc) {
        // Line segment - Circular arc intersection
        return compute_line_arc_intersections(element_1, element_2, strict);
    } else if (element_1.type == ShapeElementType::CircularArc
            && element_2.type == ShapeElementType::LineSegment) {
        return compute_line_arc_intersections(element_2, element_1, strict);
    } else if (element_1.type == ShapeElementType::CircularArc
            && element_2.type == ShapeElementType::CircularArc) {
        // Circular arc - Circular arc intersection
        return compute_arc_arc_intersections(element_1, element_2, strict);
    }

    throw std::invalid_argument("shape::compute_intersections: Invalid element types");
    return {};
}

bool shape::intersect(
        const Shape& shape_1,
        const Shape& shape_2,
        bool strict)
{
    for (const ShapeElement& element_1: shape_1.elements) {
        for (const ShapeElement& element_2: shape_2.elements) {
            auto intersections = compute_intersections(
                    element_1,
                    element_2,
                    strict);
            if (!intersections.empty())
                return true;
        }
    }
    for (const ShapeElement& element_1: shape_1.elements) {
        Point middle = element_1.middle();
        if (shape_2.contains(middle, strict))
            return true;
    }
    for (const ShapeElement& element_2: shape_2.elements) {
        Point middle = element_2.middle();
        if (shape_1.contains(middle, strict))
            return true;
    }

    return false;
}

bool shape::intersect(
        const Shape& shape,
        const ShapeElement& element,
        bool strict)
{
    for (const ShapeElement& shape_element: shape.elements) {
        auto intersections = compute_intersections(
                element,
                shape_element,
                strict);
        if (!intersections.empty())
            return true;
    }
    Point middle = element.middle();
    if (shape.contains(middle, strict))
        return true;
    return false;
}

std::vector<Shape> shape::merge_intersecting_shapes(
        const std::vector<Shape>& shapes)
{
    IntersectionTree intersection_tree(shapes);
    std::vector<std::pair<ShapePos, ShapePos>> intersecting_shapes
        = intersection_tree.compute_intersecting_shapes(false);

    // Build graph.
    std::vector<std::vector<ShapePos>> graph(shapes.size());
    for (auto p: intersecting_shapes) {
        graph[p.first].push_back(p.second);
        graph[p.second].push_back(p.first);
    }

    // For each connected component, build a shape.
    ShapePos node_id = 0;
    std::vector<uint8_t> visited(shapes.size(), 0);
    std::vector<Shape> new_shapes;
    for (;;) {
        while (node_id < shapes.size()
                && visited[node_id]) {
            node_id++;
        }
        if (node_id == shapes.size())
            break;

        Shape shape = shapes[node_id];
        visited[node_id] = 1;
        std::vector<ShapePos> stack = {node_id};
        while (!stack.empty()) {
            ShapePos node_id_cur = stack.back();
            stack.pop_back();
            for (ShapePos neighbor: graph[node_id_cur]) {
                if (visited[neighbor])
                    continue;
                stack.push_back(neighbor);
                visited[neighbor] = 1;
                shape = compute_union(shape, shapes[neighbor]).first;
            }
        }

        new_shapes.push_back(shape);
    }

    return new_shapes;
}
