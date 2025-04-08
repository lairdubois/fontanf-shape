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
    LengthDbl r = distance(arc.center, arc.start);
    LengthDbl c_prime = c - a * xm - b * ym;
    LengthDbl discriminant = r * r * (a * a + b * b) - c_prime * c_prime;
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
    Point p1;
    p1.x = xm + eta_1;
    p1.y = ym + teta_1;
    Point p2;
    p2.x = xm + eta_2;
    p2.y = ym + teta_2;

    // Check if any intersection coincides with an arc endpoint
    if (equal(p1, line.start)) {
        p1 = line.start;
    } else if (equal(p1, line.end)) {
        p1 = line.end;
    } else if (equal(p1, arc.start)) {
        p1 = arc.start;
    } else if (equal(p1, arc.end)) {
        p1 = arc.end;
    }
    if (line.contains(p1)) {
        if (p1 == arc.start || p1 == arc.end) {
            if (!strict)
                intersections.push_back(p1);
        } else if (arc.contains(p1)) {
            intersections.push_back(p1);
        }
    } else if (arc.contains(p1)) {
        if (p1 == line.start || p1 == line.end) {
            if (!strict)
                intersections.push_back(p1);
        } else if (line.contains(p1)) {
            intersections.push_back(p1);
        }
    }

    if (equal(p2, line.start)) {
        p2 = line.start;
    } else if (equal(p2, line.end)) {
        p2 = line.end;
    } else if (equal(p2, arc.start)) {
        p2 = arc.start;
    } else if (equal(p2, arc.end)) {
        p2 = arc.end;
    }
    if (line.contains(p2)) {
        if (p2 == arc.start || p2 == arc.end) {
            if (!strict)
                intersections.push_back(p2);
        } else if (arc.contains(p2)) {
            intersections.push_back(p2);
        }
    } else if (arc.contains(p2)) {
        if (p2 == line.start || p2 == line.end) {
            if (!strict)
                intersections.push_back(p2);
        } else if (line.contains(p2)) {
            intersections.push_back(p2);
        }
    }

    return intersections;
}

// Helper function to check if two arcs overlap
bool arcs_overlap(const ShapeElement& arc1, const ShapeElement& arc2) {
    // If centers or radii are different, they can only overlap at intersection points
    if (!equal(distance(arc1.center, arc2.center), 0.0) ||
        !equal(distance(arc1.center, arc1.start), distance(arc2.center, arc2.start))) {
        return false;
    }

    // Calculate angles for all endpoints
    auto calculate_angle = [](const Point& center, const Point& point) {
        return std::atan2(point.y - center.y, point.x - center.x);
    };

    LengthDbl start_angle_1 = calculate_angle(arc1.center, arc1.start);
    LengthDbl end_angle_1 = calculate_angle(arc1.center, arc1.end);
    LengthDbl start_angle_2 = calculate_angle(arc2.center, arc2.start);
    LengthDbl end_angle_2 = calculate_angle(arc2.center, arc2.end);

    // Normalize angles based on anticlockwise/clockwise
    if (!arc1.anticlockwise && end_angle_1 > start_angle_1) {
        end_angle_1 -= 2 * M_PI;
    } else if (arc1.anticlockwise && end_angle_1 < start_angle_1) {
        end_angle_1 += 2 * M_PI;
    }

    if (!arc2.anticlockwise && end_angle_2 > start_angle_2) {
        end_angle_2 -= 2 * M_PI;
    } else if (arc2.anticlockwise && end_angle_2 < start_angle_2) {
        end_angle_2 += 2 * M_PI;
    }

    // Check for overlap by seeing if any endpoint of one arc lies on the other arc
    if (!arc1.anticlockwise) {
        if ((start_angle_1 >= start_angle_2 && start_angle_1 <= end_angle_2) ||
            (end_angle_1 >= start_angle_2 && end_angle_1 <= end_angle_2) ||
            (start_angle_2 >= end_angle_1 && start_angle_2 <= start_angle_1) ||
            (end_angle_2 >= end_angle_1 && end_angle_2 <= start_angle_1)) {
            return true;
        }
    } else {
        if ((start_angle_1 <= start_angle_2 && start_angle_2 <= end_angle_1) ||
            (start_angle_1 <= end_angle_2 && end_angle_2 <= end_angle_1) ||
            (start_angle_2 <= start_angle_1 && start_angle_1 <= end_angle_2) ||
            (start_angle_2 <= end_angle_1 && end_angle_1 <= end_angle_2)) {
            return true;
        }
    }

    return false;
}

// Helper function to compute arc-arc intersections
std::vector<Point> compute_arc_arc_intersections(
        const ShapeElement& arc1,
        const ShapeElement& arc2,
        bool strict)
{
    // Get circle centers and radii
    LengthDbl x1 = arc1.center.x;
    LengthDbl y1 = arc1.center.y;
    LengthDbl r1 = distance(arc1.center, arc1.start);

    LengthDbl x2 = arc2.center.x;
    LengthDbl y2 = arc2.center.y;
    LengthDbl r2 = distance(arc2.center, arc2.start);

    // Calculate distance between centers
    LengthDbl d = distance(arc1.center, arc2.center);

    // Check for arc overlap case (only if not strict)
    if (!strict && arcs_overlap(arc1, arc2)) {
        std::vector<Point> intersections;

        // For overlapping arcs, we return all endpoints that are contained in the other arc
        if (arc2.contains(arc1.start)) {
            intersections.push_back(arc1.start);
        }

        if (arc2.contains(arc1.end)) {
            // Check if the point is already in the intersections
            bool is_duplicate = false;
            for (const auto& p : intersections) {
                if (equal(distance(p, arc1.end), 0.0)) {
                    is_duplicate = true;
                    break;
                }
            }
            if (!is_duplicate) {
                intersections.push_back(arc1.end);
            }
        }

        if (arc1.contains(arc2.start)) {
            // Check if the point is already in the intersections
            bool is_duplicate = false;
            for (const auto& p : intersections) {
                if (equal(distance(p, arc2.start), 0.0)) {
                    is_duplicate = true;
                    break;
                }
            }
            if (!is_duplicate) {
                intersections.push_back(arc2.start);
            }
        }

        if (arc1.contains(arc2.end)) {
            // Check if the point is already in the intersections
            bool is_duplicate = false;
            for (const auto& p : intersections) {
                if (equal(distance(p, arc2.end), 0.0)) {
                    is_duplicate = true;
                    break;
                }
            }
            if (!is_duplicate) {
                intersections.push_back(arc2.end);
            }
        }

        return intersections;
    }

    // Check for concentric arcs (same center)
    if (equal(d, 0.0)) {
        // If centers are the same and radii are the same, arcs may overlap
        if (equal(r1, r2) && !strict) {
            std::vector<Point> intersections;

            // For overlapping arcs, we need to return all endpoints that lie on both arcs
            if (arc2.contains(arc1.start)) {
                intersections.push_back(arc1.start);
            }

            if (arc2.contains(arc1.end)) {
                // Check if the point is already in the intersections
                bool is_duplicate = false;
                for (const auto& p : intersections) {
                    if (equal(distance(p, arc1.end), 0.0)) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    intersections.push_back(arc1.end);
                }
            }

            if (arc1.contains(arc2.start)) {
                // Check if the point is already in the intersections
                bool is_duplicate = false;
                for (const auto& p : intersections) {
                    if (equal(distance(p, arc2.start), 0.0)) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    intersections.push_back(arc2.start);
                }
            }

            if (arc1.contains(arc2.end)) {
                // Check if the point is already in the intersections
                bool is_duplicate = false;
                for (const auto& p : intersections) {
                    if (equal(distance(p, arc2.end), 0.0)) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    intersections.push_back(arc2.end);
                }
            }

            return intersections;
        }
        return {};
    }

    // Check if circles are too far apart or one inside another with no intersection
    if (d > r1 + r2 || d < std::abs(r1 - r2)) {
        return {};
    }

    // Handle overlapping arcs with same radius when centers are different but arc parts overlap
    if (equal(r1, r2) && !strict) {
        // Check if the arcs might overlap along their path (not just at intersection points)
        // For this to happen, the distance between centers must be less than 2*r
        if (d < 2 * r1) {
            std::vector<Point> intersections;

            // Calculate standard intersection points
            LengthDbl a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
            LengthDbl h = std::sqrt(r1 * r1 - a * a);

            // Calculate the point P2 that lies on the line between the centers
            LengthDbl x3 = x1 + a * (x2 - x1) / d;
            LengthDbl y3 = y1 + a * (y2 - y1) / d;

            // First intersection point
            Point p1 = {
                x3 + h * (y2 - y1) / d,
                y3 - h * (x2 - x1) / d
            };

            // Second intersection point
            Point p2 = {
                x3 - h * (y2 - y1) / d,
                y3 + h * (x2 - x1) / d
            };

            // Add intersection points if they lie on both arcs
            if (arc1.contains(p1) && arc2.contains(p1)) {
                intersections.push_back(p1);
            }

            if (arc1.contains(p2) && arc2.contains(p2)) {
                if (intersections.empty() || !equal(distance(p2, intersections[0]), 0.0)) {
                    intersections.push_back(p2);
                }
            }

            // Also check if any endpoints of one arc lie on the other arc
            if (arc2.contains(arc1.start)) {
                // Check if the point is already in the intersections
                bool is_duplicate = false;
                for (const auto& p : intersections) {
                    if (equal(distance(p, arc1.start), 0.0)) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    intersections.push_back(arc1.start);
                }
            }

            if (arc2.contains(arc1.end)) {
                // Check if the point is already in the intersections
                bool is_duplicate = false;
                for (const auto& p : intersections) {
                    if (equal(distance(p, arc1.end), 0.0)) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    intersections.push_back(arc1.end);
                }
            }

            if (arc1.contains(arc2.start)) {
                // Check if the point is already in the intersections
                bool is_duplicate = false;
                for (const auto& p : intersections) {
                    if (equal(distance(p, arc2.start), 0.0)) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    intersections.push_back(arc2.start);
                }
            }

            if (arc1.contains(arc2.end)) {
                // Check if the point is already in the intersections
                bool is_duplicate = false;
                for (const auto& p : intersections) {
                    if (equal(distance(p, arc2.end), 0.0)) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    intersections.push_back(arc2.end);
                }
            }

            return intersections;
        }
    }

    // Special case: circles touch externally or internally
    if (equal(d, r1 + r2) || equal(d, std::abs(r1 - r2))) {
        // Circles touch at exactly one point
        Point touching_point;
        if (equal(d, r1 + r2)) {
            // External touching
            touching_point = {
                x1 + r1 * (x2 - x1) / d,
                y1 + r1 * (y2 - y1) / d
            };
        } else {
            // Internal touching
            touching_point = {
                x1 + r1 * (x2 - x1) / d,
                y1 + r1 * (y2 - y1) / d
            };
        }

        // Check if the touching point lies on both arcs
        if (arc1.contains(touching_point) && arc2.contains(touching_point)) {
            return {touching_point};
        }
        return {};
    }

    // Calculate intersection points of circles
    LengthDbl a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
    LengthDbl h = std::sqrt(r1 * r1 - a * a);

    // Calculate the point P2 that lies on the line between the centers
    LengthDbl x3 = x1 + a * (x2 - x1) / d;
    LengthDbl y3 = y1 + a * (y2 - y1) / d;

    // Calculate the intersection points
    std::vector<Point> intersections;

    // First intersection point
    Point p1 = {
        x3 + h * (y2 - y1) / d,
        y3 - h * (x2 - x1) / d
    };

    // Second intersection point
    Point p2 = {
        x3 - h * (y2 - y1) / d,
        y3 + h * (x2 - x1) / d
    };

    // Check if points lie on both arcs
    if (arc1.contains(p1) && arc2.contains(p1)) {
        // Check if p1 coincides with arc endpoints
        if (equal(distance(p1, arc1.start), 0.0)) {
            if (!strict) {
                intersections.push_back(arc1.start);
            }
        } else if (equal(distance(p1, arc1.end), 0.0)) {
            if (!strict) {
                intersections.push_back(arc1.end);
            }
        } else if (equal(distance(p1, arc2.start), 0.0)) {
            if (!strict) {
                intersections.push_back(arc2.start);
            }
        } else if (equal(distance(p1, arc2.end), 0.0)) {
            if (!strict) {
                intersections.push_back(arc2.end);
            }
        } else {
            intersections.push_back(p1);
        }
    }

    if (arc1.contains(p2) && arc2.contains(p2)) {
        // Check if p2 coincides with arc endpoints
        if (equal(distance(p2, arc1.start), 0.0)) {
            if (!strict && (intersections.empty() || !equal(distance(intersections[0], arc1.start), 0.0))) {
                intersections.push_back(arc1.start);
            }
        } else if (equal(distance(p2, arc1.end), 0.0)) {
            if (!strict && (intersections.empty() || !equal(distance(intersections[0], arc1.end), 0.0))) {
                intersections.push_back(arc1.end);
            }
        } else if (equal(distance(p2, arc2.start), 0.0)) {
            if (!strict && (intersections.empty() || !equal(distance(intersections[0], arc2.start), 0.0))) {
                intersections.push_back(arc2.start);
            }
        } else if (equal(distance(p2, arc2.end), 0.0)) {
            if (!strict && (intersections.empty() || !equal(distance(intersections[0], arc2.end), 0.0))) {
                intersections.push_back(arc2.end);
            }
        } else {
            // Check if p2 is duplicate of p1 (can happen due to floating point errors)
            if (intersections.empty() || !equal(distance(p2, intersections[0]), 0.0)) {
                intersections.push_back(p2);
            }
        }
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
