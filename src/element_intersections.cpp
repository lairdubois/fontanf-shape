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

    Point p;
    p.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denom;
    p.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denom;
    if (strict) {
        if (equal(p, line1.start)) {
            return {};
        } else if (equal(p, line1.end)) {
            return {};
        } else if (equal(p, line2.start)) {
            return {};
        } else if (equal(p, line2.end)) {
            return {};
        }
    } else {
        if (equal(p, line1.start)) {
            p = line1.start;
        } else if (equal(p, line1.end)) {
            p = line1.end;
        } else if (equal(p, line2.start)) {
            p = line2.start;
        } else if (equal(p, line2.end)) {
            p = line2.end;
        }
    }

    if (line1.contains(p) && line2.contains(p))
        return {p};
    return {};
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
    LengthDbl c = line.end.x * line.start.y - line.start.x * line.end.y;
    LengthDbl rsq = squared_distance(arc.center, arc.start);
    LengthDbl c_prime = c - a * xm - b * ym;

    // Single intersection point.
    if (equal(rsq * (a * a + b * b), c_prime * c_prime)) {
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
        if (line.contains(p) && arc.contains(p)) {
            return {p};
        } else {
            return {};
        }
    }

    // No intersection.
    if (strictly_lesser(rsq * (a * a + b * b), c_prime * c_prime))
        return {};

    std::vector<Point> intersections;
    LengthDbl discriminant = rsq * (a * a + b * b) - c_prime * c_prime;
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
    //std::cout << "p1 " << ps[0].to_string() << std::endl;
    //std::cout << "p2 " << ps[1].to_string() << std::endl;

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

    // Single intersection point.
    if (equal(rsq * (a * a + b * b), c_prime * c_prime)) {
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
        if (arc.contains(p) && arc_2.contains(p)) {
            return {p};
        } else {
            return {};
        }
    }

    // No intersection.
    if (strictly_lesser(rsq * (a * a + b * b), c_prime * c_prime))
        return {};

    std::vector<Point> intersections;
    LengthDbl discriminant = rsq * (a * a + b * b) - c_prime * c_prime;
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
    IntersectionTree intersection_tree(shapes, {}, {});
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

std::vector<Point> shape::equalize_points(
        const std::vector<Point>& points)
{
    IntersectionTree intersection_tree({}, {}, points);
    std::vector<std::pair<ElementPos, ElementPos>> equal_points
        = intersection_tree.compute_equal_points();

    // Build graph.
    std::vector<std::vector<ElementPos>> graph(points.size());
    for (auto p: equal_points) {
        graph[p.first].push_back(p.second);
        graph[p.second].push_back(p.first);
    }

    // For each connected component, build a point.
    ElementPos node_id = 0;
    std::vector<uint8_t> visited(points.size(), 0);
    std::vector<Point> new_points = points;
    for (;;) {
        while (node_id < points.size()
                && visited[node_id]) {
            node_id++;
        }
        if (node_id == points.size())
            break;

        const Point& point = points[node_id];
        visited[node_id] = 1;
        std::vector<ElementPos> stack = {node_id};
        while (!stack.empty()) {
            ElementPos node_id_cur = stack.back();
            stack.pop_back();
            for (ElementPos neighbor: graph[node_id_cur]) {
                if (visited[neighbor])
                    continue;
                stack.push_back(neighbor);
                visited[neighbor] = 1;
                new_points[neighbor] = point;
                //std::cout << "new_points[neighbor] " << new_points[neighbor].to_string()
                //    << " -> " << point.to_string() << std::endl;
            }
        }
    }

    return new_points;
}

std::vector<ShapeElement> shape::equalize_points(
        const std::vector<ShapeElement>& elements)
{
    std::vector<std::pair<ElementPos, uint8_t>> sorted_element_points;
    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)elements.size();
            ++element_pos) {
        const ShapeElement& element = elements[element_pos];
        sorted_element_points.push_back({element_pos, 0});
        sorted_element_points.push_back({element_pos, 1});
        if (element.type == ShapeElementType::CircularArc)
            sorted_element_points.push_back({element_pos, 2});
    }
    std::sort(
            sorted_element_points.begin(),
            sorted_element_points.end(),
            [&elements](
                const std::pair<ElementPos, bool>& p1,
                const std::pair<ElementPos, bool>& p2)
            {
                ElementPos element_pos_1 = p1.first;
                ElementPos element_pos_2 = p2.first;
                const ShapeElement& element_1 = elements[element_pos_1];
                const ShapeElement& element_2 = elements[element_pos_2];
                int x1 = p1.second;
                int x2 = p2.second;
                const Point& point_1 = (x1 == 0)? element_1.start: (x1 == 1)? element_1.end: element_1.center;
                const Point& point_2 = (x2 == 0)? element_2.start: (x2 == 2)? element_2.end: element_2.center;
                if (point_1.x != point_2.x)
                    return point_1.x < point_2.x;
                return point_1.y < point_2.y;
            });

    std::vector<ElementPos> new_elements_pos_2_equalize_points_pos(sorted_element_points.size());
    std::vector<Point> equalize_points_input;
    Point point_prev = {0, 0};
    for (ElementPos pos = 0; pos < (ElementPos)sorted_element_points.size(); ++pos) {
        ElementPos element_pos = sorted_element_points[pos].first;
        const ShapeElement& element = elements[element_pos];
        int x = sorted_element_points[pos].second;
        const Point& point = (x == 0)? element.start: (x == 1)? element.end: element.center;

        if (pos == 0 || !(point_prev == point))
            equalize_points_input.push_back(point);
        new_elements_pos_2_equalize_points_pos[pos] = equalize_points_input.size() - 1;

        point_prev = point;
    }

    std::vector<Point> equalize_points_output = equalize_points(equalize_points_input);

    std::vector<ShapeElement> elements_new = elements;
    for (ElementPos pos = 0;
            pos < (ElementPos)sorted_element_points.size();
            ++pos) {
        ElementPos element_pos = sorted_element_points[pos].first;
        ShapeElement& element = elements_new[element_pos];
        bool start = sorted_element_points[pos].second;
        int x = sorted_element_points[pos].second;
        Point& point = (x == 0)? element.start: (x == 1)? element.end: element.center;

        ElementPos p = new_elements_pos_2_equalize_points_pos[pos];
        if (!(point == equalize_points_output[p])) {
            //std::cout << "point " << point.to_string() << " -> " << equalize_points_output[p].to_string() << std::endl;
            point = equalize_points_output[p];
        }
    }
    return elements_new;
}
