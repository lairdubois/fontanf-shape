#include "shape/shape.hpp"

#include "shape/element_intersections.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace shape;

LengthDbl shape::largest_power_of_two_lesser_or_equal(LengthDbl value)
{
    double res = 1;
    if (res > value) {
        while (res > value)
            res /= 2;
    } else {
        while (res * 2 < value)
            res *= 2;
    }
    return res;
}

LengthDbl shape::smallest_power_of_two_greater_or_equal(LengthDbl value)
{
    double res = 1;
    if (res < value) {
        while (res < value)
            res *= 2;
    } else {
        while (res / 2 > value)
            res /= 2;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Point /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

std::string Point::to_string() const
{
    std::streamsize precision = std::cout.precision();
    std::stringstream ss;
    ss << std::setprecision(std::numeric_limits<LengthDbl>::digits10 + 1)
        << "(" << x << ", " << y << ")"
        << std::setprecision(precision);
    return ss.str();
    //return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

Point shape::operator+(
        const Point& point_1,
        const Point& point_2)
{
    return {point_1.x + point_2.x, point_1.y + point_2.y};
}

Point shape::operator*(
        LengthDbl scalar,
        const Point& point)
{
    return {scalar * point.x, scalar * point.y};
}

Point shape::operator-(
        const Point& point_1,
        const Point& point_2)
{
    return {point_1.x - point_2.x, point_1.y - point_2.y};
}

LengthDbl shape::norm(
        const Point& vector)
{
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

LengthDbl shape::squared_norm(
        const Point& vector)
{
    return vector.x * vector.x + vector.y * vector.y;
}

LengthDbl shape::distance(
        const Point& point_1,
        const Point& point_2)
{
    return norm(point_2 - point_1);
}

LengthDbl shape::squared_distance(
        const Point& point_1,
        const Point& point_2)
{
    return squared_norm(point_2 - point_1);
}

LengthDbl shape::dot_product(
        const Point& vector_1,
        const Point& vector_2)
{
    return vector_1.x * vector_2.x + vector_1.y * vector_2.y;
}

LengthDbl shape::cross_product(
        const Point& vector_1,
        const Point& vector_2)
{
    return vector_1.x * vector_2.y - vector_2.x * vector_1.y;
}

Point& Point::shift(
        LengthDbl x,
        LengthDbl y)
{
    this->x += x;
    this->y += y;
    return *this;
}

Point Point::rotate(
        Angle angle) const
{
    if (equal(angle, 0.0)) {
        return *this;
    } else if (equal(angle, 180)) {
        Point point_out;
        point_out.x = -x;
        point_out.y = -y;
        return point_out;
    } else if (equal(angle, 90)) {
        Point point_out;
        point_out.x = -y;
        point_out.y = x;
        return point_out;
    } else if (equal(angle, 270)) {
        Point point_out;
        point_out.x = y;
        point_out.y = -x;
        return point_out;
    } else {
        Point point_out;
        angle = M_PI * angle / 180;
        point_out.x = std::cos(angle) * x - std::sin(angle) * y;
        point_out.y = std::sin(angle) * x + std::cos(angle) * y;
        return point_out;
    }
}

Point Point::rotate_radians(
        Angle angle) const
{
    Point point_out;
    point_out.x = std::cos(angle) * x - std::sin(angle) * y;
    point_out.y = std::sin(angle) * x + std::cos(angle) * y;
    return point_out;
}

Point Point::rotate_radians(
        const Point& center,
        Angle angle) const
{
    Point point_out;
    point_out.x = center.x
        + std::cos(angle) * (this->x - center.x)
        - std::sin(angle) * (this->y - center.y);
    point_out.y = center.y
        + std::sin(angle) * (this->x - center.x)
        + std::cos(angle) * (this->y - center.y);
    return point_out;
}

Point Point::axial_symmetry_identity_line() const
{
    Point point_out;
    point_out.x = y;
    point_out.y = x;
    return point_out;
}

Point Point::axial_symmetry_y_axis() const
{
    Point point_out;
    point_out.x = -x;
    point_out.y = y;
    return point_out;
}

Point Point::axial_symmetry_x_axis() const
{
    Point point_out;
    point_out.x = x;
    point_out.y = -y;
    return point_out;
}

Angle shape::angle_radian(
        const Point& vector)
{
    Angle a = std::atan2(vector.y, vector.x);
    if (a < 0)
        a += 2 * M_PI;
    return a;
}

Angle shape::angle_radian(
        const Point& vector_1,
        const Point& vector_2)
{
    Angle a = atan2(vector_2.y, vector_2.x) - atan2(vector_1.y, vector_1.x);
    if (a < 0)
        a += 2 * M_PI;
    return a;
}

Point ShapeElement::middle() const
{
    switch (type) {
    case ShapeElementType::LineSegment: {
        Point point;
        point.x = (this->start.x + this->end.x) / 2;
        point.y = (this->start.y + this->end.y) / 2;
        return point;
    } case ShapeElementType::CircularArc: {
        if (this->anticlockwise) {
            Angle angle = angle_radian(
                    this->start - this->center,
                    this->end - this->center);
            return this->start.rotate_radians(this->center, angle / 2.);
        } else {
            Angle angle = angle_radian(
                this->end - this->center,
                this->start - this->center);
            return this->end.rotate_radians(this->center, angle / 2.);
        }
    }
    }
    return {0, 0};
}

std::pair<Point, Point> ShapeElement::min_max() const
{
    LengthDbl x_min = (std::min)(this->start.x, this->end.x);
    LengthDbl x_max = (std::max)(this->start.x, this->end.x);
    LengthDbl y_min = (std::min)(this->start.y, this->end.y);
    LengthDbl y_max = (std::max)(this->start.y, this->end.y);

    if (this->type == ShapeElementType::CircularArc) {
        LengthDbl radius = distance(this->center, this->start);
        Angle starting_angle = shape::angle_radian(this->start - this->center);
        Angle ending_angle = shape::angle_radian(this->end - this->center);
        if (!this->anticlockwise)
            std::swap(starting_angle, ending_angle);
        //std::cout << "starting_angle " << starting_angle << " ending_angle " << ending_angle << std::endl;
        if (starting_angle <= ending_angle) {
            if (starting_angle <= M_PI
                    && ending_angle >= M_PI) {
                x_min = std::min(x_min, this->center.x - radius);
            }
            if (starting_angle == 0)
                x_max = std::max(x_max, this->center.x + radius);
            if (starting_angle <= 3 * M_PI / 2
                    && ending_angle >= 3 * M_PI / 2 ) {
                y_min = std::min(y_min, this->center.y - radius);
            }
            if (starting_angle <= M_PI / 2
                    && ending_angle >= M_PI / 2) {
                y_max = std::max(y_max, this->center.y + radius);
            }
        } else {  // starting_angle > ending_angle
            if (starting_angle <= M_PI
                    || ending_angle >= M_PI) {
                x_min = std::min(x_min, this->center.x - radius);
            }
            x_max = std::max(x_max, this->center.x + radius);
            if (starting_angle <= 3 * M_PI / 2
                    || ending_angle >= 3 * M_PI / 2) {
                y_min = std::min(y_min, this->center.y - radius);
            }
            if (starting_angle <= M_PI / 2
                    || ending_angle >= M_PI / 2) {
                y_max = std::max(y_max, this->center.y + radius);
            }
        }
    }

    return {{x_min, y_min}, {x_max, y_max}};
}

int shape::counter_clockwise(
        const Point& point_1,
        const Point& point_2,
        const Point& point_3)
{
    AreaDbl area = (point_2.x - point_1.x) * (point_3.y - point_1.y)
        - (point_2.y - point_1.y) * (point_3.x - point_1.x);
    if (strictly_greater(area, 0)) {
        return -1;
    } else if (strictly_lesser(area, 0)) {
        return 1;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// ShapeElement /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

LengthDbl ShapeElement::length() const
{
    switch (type) {
    case ShapeElementType::LineSegment:
        return distance(this->start, this->end);
    case ShapeElementType::CircularArc:
        LengthDbl r = distance(center, start);
        if (anticlockwise) {
            return angle_radian(start - center, end - center) * r;
        } else {
            return angle_radian(end - center, start - center) * r;
        }
    }
    return -1;
}

bool ShapeElement::contains(const Point& point) const
{
    switch (type) {
    case ShapeElementType::LineSegment: {
        return equal(
                distance(this->start, point) + distance(point, this->end),
                distance(this->start, this->end));
    } case ShapeElementType::CircularArc: {
        // Check if point lies on circle
        if (!equal(
                    distance(point, this->center),
                    distance(this->start, this->center))) {
            return false;
        }

        // Calculate angles
        Angle point_angle = angle_radian(point - this->center);
        Angle start_angle = angle_radian(this->start - this->center);
        Angle end_angle = angle_radian(this->end - this->center);

        if (this->anticlockwise) {
            Angle a0 = angle_radian(
                    this->start - this->center,
                    this->end - this->center);
            Angle a = angle_radian(
                    this->start - this->center,
                    point - this->center);
            return !strictly_greater(a, a0);
        } else {
            Angle a0 = angle_radian(
                    this->end - this->center,
                    this->start - this->center);
            Angle a = angle_radian(
                    this->end - this->center,
                    point - this->center);
            return !strictly_greater(a, a0);
        }
    }
    }
    return -1;
}

std::string ShapeElement::to_string() const
{
    switch (type) {
    case ShapeElementType::LineSegment: {
        return "LineSegment start " + start.to_string()
            + " end " + end.to_string();
    } case ShapeElementType::CircularArc: {
        return "CircularArc start " + start.to_string()
            + " end " + end.to_string()
            + " center " + center.to_string()
            + ((anticlockwise)? " anticlockwise": " clockwise");
    }
    }
    return "";
}

nlohmann::json ShapeElement::to_json() const
{
    nlohmann::json json;
    json["type"] = element2str(type);
    json["start"]["x"] = start.x;
    json["start"]["y"] = start.y;
    json["end"]["x"] = end.x;
    json["end"]["y"] = end.y;
    if (type == ShapeElementType::CircularArc) {
        json["center"]["x"] = center.x;
        json["center"]["y"] = center.y;
        json["anticlockwise"] = anticlockwise;
    }
    return json;
}

ShapeElement& ShapeElement::shift(
        LengthDbl x,
        LengthDbl y)
{
    this->start.shift(x, y);
    this->end.shift(x, y);
    this->center.shift(x, y);
    return *this;
}

ShapeElement ShapeElement::rotate(
        Angle angle) const
{
    ShapeElement element_out = *this;
    element_out.start = start.rotate(angle);
    element_out.end = end.rotate(angle);
    element_out.center = center.rotate(angle);
    return element_out;
}

ShapeElement ShapeElement::axial_symmetry_identity_line() const
{
    ShapeElement element_out = *this;
    element_out.start = end.axial_symmetry_identity_line();
    element_out.end = start.axial_symmetry_identity_line();
    element_out.center = center.axial_symmetry_identity_line();
    element_out.anticlockwise = !anticlockwise;
    return element_out;
}

ShapeElement ShapeElement::axial_symmetry_x_axis() const
{
    ShapeElement element_out = *this;
    element_out.start = end.axial_symmetry_x_axis();
    element_out.end = start.axial_symmetry_x_axis();
    element_out.center = center.axial_symmetry_x_axis();
    element_out.anticlockwise = !anticlockwise;
    return element_out;
}

ShapeElement ShapeElement::axial_symmetry_y_axis() const
{
    ShapeElement element_out = *this;
    element_out.start = end.axial_symmetry_y_axis();
    element_out.end = start.axial_symmetry_y_axis();
    element_out.center = center.axial_symmetry_y_axis();
    element_out.anticlockwise = !anticlockwise;
    return element_out;
}

ShapeElement ShapeElement::reverse() const
{
    ShapeElement element_out;
    element_out.type = this->type;
    element_out.start.x = this->end.x;
    element_out.start.y = this->end.y;
    element_out.end.x = this->start.x;
    element_out.end.y = this->start.y;
    element_out.center.x = this->center.x;
    element_out.center.y = this->center.y;
    element_out.anticlockwise = !this->anticlockwise;
    return element_out;
}

ShapeElementType shape::str2element(const std::string& str)
{
    if (str == "LineSegment"
            || str == "line_segment"
            || str == "L"
            || str == "l") {
        return ShapeElementType::LineSegment;
    } else if (str == "CircularArc"
            || str == "circular_arc"
            || str == "C"
            || str == "c") {
        return ShapeElementType::CircularArc;
    } else {
        throw std::invalid_argument("");
        return ShapeElementType::LineSegment;
    }
}

std::string shape::element2str(ShapeElementType type)
{
    switch (type) {
    case ShapeElementType::LineSegment: {
        return "LineSegment";
    } case ShapeElementType::CircularArc: {
        return "CircularArc";
    }
    }
    return "";
}

char shape::element2char(ShapeElementType type)
{
    switch (type) {
    case ShapeElementType::LineSegment: {
        return 'L';
    } case ShapeElementType::CircularArc: {
        return 'C';
    }
    }
    return ' ';
}

ShapeElement shape::operator*(
        LengthDbl scalar,
        const ShapeElement& element)
{
    ShapeElement element_out;
    element_out.type = element.type;
    element_out.start = scalar * element.start;
    element_out.end = scalar * element.end;
    element_out.center = scalar * element.center;
    element_out.anticlockwise = element.anticlockwise;
    return element_out;
}

std::vector<ShapeElement> shape::approximate_circular_arc_by_line_segments(
        const ShapeElement& circular_arc,
        LengthDbl segment_length,
        bool outer)
{
    LengthDbl arc_length = circular_arc.length();
    ElementPos number_of_line_segments = std::ceil(arc_length / segment_length);

    if (circular_arc.type != ShapeElementType::CircularArc) {
        throw std::runtime_error(
                "shape::circular_arc_to_line_segments: "
                "input element must be of type CircularArc; "
                "circular_arc.type: " + element2str(circular_arc.type) + ".");
    }
    if (!outer && number_of_line_segments < 1) {
        number_of_line_segments = 1;
        //throw std::runtime_error(
        //        "shape::circular_arc_to_line_segments: "
        //        "at least 1 line segment is needed to inner approximate a circular arc; "
        //        "outer: " + std::to_string(outer) + "; "
        //        "number_of_line_segments: " + std::to_string(number_of_line_segments) + ".");
    }

    Angle angle = (circular_arc.anticlockwise)?
        angle_radian(
            circular_arc.start - circular_arc.center,
            circular_arc.end - circular_arc.center):
        angle_radian(
            circular_arc.end - circular_arc.center,
            circular_arc.start - circular_arc.center);
    if ((outer && circular_arc.anticlockwise)
            || (!outer && !circular_arc.anticlockwise)) {
        if (angle < M_PI && number_of_line_segments < 2) {
            number_of_line_segments = 2;
            //throw std::runtime_error(
            //        "shape::circular_arc_to_line_segments: "
            //        "at least 2 line segments are needed to approximate the circular arc; "
            //        "circular_arc: " + circular_arc.to_string() + "; "
            //        "outer: " + std::to_string(outer) + "; "
            //        "angle: " + std::to_string(angle) + "; "
            //        "number_of_line_segments: " + std::to_string(number_of_line_segments) + ".");
        } else if (angle >= M_PI && number_of_line_segments < 3) {
            number_of_line_segments = 3;
            //throw std::runtime_error(
            //        "shape::circular_arc_to_line_segments: "
            //        "at least 3 line segments are needed to approximate the circular arc; "
            //        "circular_arc: " + circular_arc.to_string() + "; "
            //        "outer: " + std::to_string(outer) + "; "
            //        "angle: " + std::to_string(angle) + "; "
            //        "number_of_line_segments: " + std::to_string(number_of_line_segments) + ".");
        }
    }

    std::vector<ShapeElement> line_segments;
    LengthDbl radius = distance(circular_arc.center, circular_arc.start);
    Point point_prev = circular_arc.start;
    Point point_circle_prev = circular_arc.start;
    for (ElementPos line_segment_id = 0;
            line_segment_id < number_of_line_segments - 1;
            ++line_segment_id) {
        Angle angle_cur = (angle * (line_segment_id + 1)) / (number_of_line_segments - 1);
        if (!circular_arc.anticlockwise)
            angle_cur *= -1;
        //std::cout << "angle_cur " << angle_cur << std::endl;
        Point point_circle = circular_arc.start.rotate_radians(
                circular_arc.center,
                angle_cur);
        //std::cout << "point_circle " << point_circle.to_string() << std::endl;
        Point point_cur;
        if ((outer && !circular_arc.anticlockwise) || (!outer && circular_arc.anticlockwise)) {
            point_cur = point_circle;
        } else {
            // https://en.wikipedia.org/wiki/Tangent_lines_to_circles#Cartesian_equation
            // https://en.wikipedia.org/wiki/Intersection_(geometry)#Two_lines
            // The tangent line of the circle at (x1, y1) has Cartesian equation
            // (x - x1)(x1 - xc) + (y - y1)(y1 - yc) = 0
            // (x1 - xc) * x + (y1 - yc) * y - x1 * (x1 - xc) - y1 * (y1 - yc) = 0
            // At (x2, y2)
            // (x2 - xc) * x + (y2 - yc) * y - x2 * (x2 - xc) - y2 * (y1 - yc) = 0
            LengthDbl a1 = (point_circle_prev.x - circular_arc.center.x);
            LengthDbl b1 = (point_circle_prev.y - circular_arc.center.y);
            LengthDbl c1 = point_circle_prev.x * (point_circle_prev.x - circular_arc.center.x)
                + point_circle_prev.y * (point_circle_prev.y - circular_arc.center.y);
            LengthDbl a2 = (point_circle.x - circular_arc.center.x);
            LengthDbl b2 = (point_circle.y - circular_arc.center.y);
            LengthDbl c2 = point_circle.x * (point_circle.x - circular_arc.center.x)
                + point_circle.y * (point_circle.y - circular_arc.center.y);
            point_cur.x = (c1 * b2 - c2 * b1) / (a1 * b2 - a2 * b1);
            point_cur.y = (a1 * c2 - a2 * c1) / (a1 * b2 - a2 * b1);
        }
        ShapeElement line_segment;
        line_segment.start = point_prev;
        line_segment.end = point_cur;
        line_segment.type = ShapeElementType::LineSegment;
        line_segments.push_back(line_segment);
        point_prev = point_cur;
        point_circle_prev = point_circle;
    }
    ShapeElement line_segment;
    line_segment.start = point_prev;
    line_segment.end = circular_arc.end;
    line_segment.type = ShapeElementType::LineSegment;
    line_segments.push_back(line_segment);
    return line_segments;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Shape /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

std::string shape::shape2str(ShapeType type)
{
    switch (type) {
    case ShapeType::Circle: {
        return "C";
    } case ShapeType::Square: {
        return "S";
    } case ShapeType::Rectangle: {
        return "R";
    } case ShapeType::Polygon: {
        return "P";
    } case ShapeType::PolygonWithHoles: {
        return "PH";
    } case ShapeType::MultiPolygon: {
        return "MP";
    } case ShapeType::MultiPolygonWithHoles: {
        return "MPH";
    } case ShapeType::GeneralShape: {
        return "G";
    }
    }
    return "";
}

bool Shape::is_circle() const
{
    return (elements.size() == 1
            && elements.front().type == ShapeElementType::CircularArc);
}

bool Shape::is_square() const
{
    if (elements.size() != 4)
        return false;
    auto it_prev = std::prev(elements.end());
    for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (it->type != ShapeElementType::LineSegment)
            return false;
        // Check angle.
        Angle theta = angle_radian(it_prev->start - it_prev->end, it->end - it->start);
        if (!equal(theta, M_PI / 2))
            return false;
        // Check length.
        if (!equal(it->length(), elements[0].length()))
            return false;
        it_prev = it;
    }
    return true;
}

bool Shape::is_rectangle() const
{
    if (elements.size() != 4)
        return false;
    auto it_prev = std::prev(elements.end());
    for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (it->type != ShapeElementType::LineSegment)
            return false;
        // Check angle.
        Angle theta = angle_radian(it_prev->start - it_prev->end, it->end - it->start);
        if (!equal(theta, M_PI / 2))
            return false;
        it_prev = it;
    }
    return true;
}

bool Shape::is_polygon() const
{
    for (auto it = elements.begin(); it != elements.end(); ++it)
        if (it->type != ShapeElementType::LineSegment)
            return false;
    return true;
}

AreaDbl Shape::compute_area() const
{
    AreaDbl area = 0.0;
    for (const ShapeElement& element: elements) {
        area += cross_product(element.start, element.end);
        // Handle circular arcs.
        if (element.type == ShapeElementType::CircularArc) {
            LengthDbl radius = distance(element.center, element.start);
            Angle theta = angle_radian(element.center - element.start, element.center - element.end);
            if (element.anticlockwise) {
                area += radius * radius * ((!(element.start == element.end))? theta: 2.0 * M_PI);
            } else {
                area -= radius * radius * ((!(element.start == element.end))? 2.0 * M_PI - theta: 2.0 * M_PI);
            }
        }
    }
    return area / 2;
}

std::pair<Point, Point> Shape::compute_min_max(
        Angle angle,
        bool mirror) const
{
    LengthDbl x_min = std::numeric_limits<LengthDbl>::infinity();
    LengthDbl x_max = -std::numeric_limits<LengthDbl>::infinity();
    LengthDbl y_min = std::numeric_limits<LengthDbl>::infinity();
    LengthDbl y_max = -std::numeric_limits<LengthDbl>::infinity();
    for (const ShapeElement& element_orig: elements) {
        ShapeElement element = (!mirror)?
            element_orig.rotate(angle):
            element_orig.axial_symmetry_y_axis().rotate(angle);
        auto mm = element.min_max();
        x_min = std::min(x_min, mm.first.x);
        x_max = std::max(x_max, mm.second.x);
        y_min = std::min(y_min, mm.first.y);
        y_max = std::max(y_max, mm.second.y);
    }
    return {{x_min, y_min}, {x_max, y_max}};
}

bool Shape::contains(
        const Point& point,
        bool strict) const
{
    if (this->elements.empty())
        return false;

    // First check if the point lies on any boundary.
    for (const ShapeElement& element : this->elements)
        if (element.contains(point))
            return (strict)? false: true;

    // Then use the ray-casting algorithm to check if the point is inside
    ElementPos intersection_count = 0;
    for (const ShapeElement& element: this->elements) {
        //std::cout << "element " << element.to_string() << std::endl;
        if (element.type == ShapeElementType::LineSegment) {
            // Horizontal edges are excluded.
            if (equal(element.start.y, element.end.y))
                continue;

            // Check y.
            if (strictly_greater(point.y, element.start.y)
                    && strictly_greater(point.y, element.end.y)) {
                continue;
            }
            if (strictly_lesser(point.y, element.start.y)
                    && strictly_lesser(point.y, element.end.y)) {
                continue;
            }

            bool upward = (element.end.y > element.start.y);

            if (upward) {
                // An upward edge includes its starting endpoint, and excludes its
                // final endpoint;
                if (equal(point.y, element.start.y)) {
                    if (element.start.x > point.x) {
                        intersection_count++;
                        continue;
                    }
                } else if (equal(point.y, element.end.y)) {
                    continue;
                }
            } else {
                // A downward edge excludes its starting endpoint, and includes its final endpoint;
                if (equal(point.y, element.start.y)) {
                    continue;
                } else if (equal(point.y, element.end.y)) {
                    if (element.end.x > point.x) {
                        intersection_count++;
                        continue;
                    }
                }
            }

            LengthDbl x_inters = element.start.x
                + (point.y - element.start.y)
                * (element.end.x - element.start.x)
                / (element.end.y - element.start.y);
            //std::cout << "x_inters " << x_inters << std::endl;
            if (x_inters > point.x) {
                intersection_count++;
            }
        } else if (element.type == ShapeElementType::CircularArc) {
            LengthDbl radius = distance(element.center, element.start);
            ShapeElement ray;
            ray.type = ShapeElementType::LineSegment;
            ray.start.x = point.x;
            ray.start.y = point.y;
            ray.end.x = (std::max)(point.x, element.center.x) + 2 * radius;
            ray.end.y = point.y;

            std::vector<Point> intersections = compute_intersections(ray, element);
            for (const Point& intersection: intersections) {
                if (intersection.x < point.x)
                    continue;
                //std::cout << "intersection " << intersection.to_string() << std::endl;
                if (intersection == element.start) {
                    Angle start_angle = angle_radian(element.start - element.center);
                    //std::cout << "start_angle " << start_angle
                    //    << " M_PI / 2 " << M_PI / 2
                    //    << " 3 * M_PI / 2 " << 3 * M_PI / 2
                    //    << std::endl;
                    bool start_upward = (element.anticlockwise)?
                        (start_angle < M_PI / 2 || start_angle >= 3 * M_PI / 2):
                        (start_angle > M_PI / 2 && start_angle <= 3 * M_PI / 2);
                    //std::cout << "start_upward " << start_upward << std::endl;
                    if (start_upward)
                        intersection_count++;
                } else if (intersection == element.end) {
                    Angle end_angle = angle_radian(element.end - element.center);
                    bool end_upward = (element.anticlockwise)?
                        (end_angle < M_PI / 2 || end_angle >= 3 * M_PI / 2):
                        (end_angle >= M_PI / 2 && end_angle < 3 * M_PI / 2);
                    //std::cout << "end_upward " << end_upward << std::endl;
                    if (!end_upward)
                        intersection_count++;
                } else {
                    intersection_count++;
                }
            }
        }
    }

    // If the number of intersections is odd, the point is inside the shape
    //std::cout << "intersection_count " << intersection_count << std::endl;
    return (intersection_count % 2 == 1);
}

Shape& Shape::shift(
        LengthDbl x,
        LengthDbl y)
{
    for (ShapeElement& element: elements)
        element.shift(x, y);
    return *this;
}

Shape Shape::rotate(Angle angle) const
{
    Shape shape;
    for (const ShapeElement& element: elements) {
        ShapeElement element_new = element.rotate(angle);
        shape.elements.push_back(element_new);
    }
    return shape;
}

Shape Shape::axial_symmetry_identity_line() const
{
    Shape shape;
    for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
        ShapeElement element_new = it->axial_symmetry_identity_line();
        shape.elements.push_back(element_new);
    }
    return shape;
}

Shape Shape::axial_symmetry_x_axis() const
{
    Shape shape;
    for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
        ShapeElement element_new = it->axial_symmetry_x_axis();
        shape.elements.push_back(element_new);
    }
    return shape;
}

Shape Shape::axial_symmetry_y_axis() const
{
    Shape shape;
    for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
        ShapeElement element_new = it->axial_symmetry_y_axis();
        shape.elements.push_back(element_new);
    }
    return shape;
}

Shape Shape::reverse() const
{
    Shape shape;
    for (auto it = elements.rbegin(); it != elements.rend(); ++it)
        shape.elements.push_back(it->reverse());
    return shape;
}

std::pair<LengthDbl, LengthDbl> Shape::compute_width_and_height(
        Angle angle,
        bool mirror) const
{
    auto points = compute_min_max(angle, mirror);
    LengthDbl width = points.second.x - points.first.x;
    LengthDbl height = points.second.y - points.first.y;
    return {width, height};
}

bool Shape::check() const
{
    // Check that the shape is not empty.
    if (elements.empty())
        return false;
    ElementPos element_prev_pos = this->elements.size() - 1;
    for (ElementPos element_cur_pos = 0;
            element_cur_pos < (ElementPos)this->elements.size();
            ++element_cur_pos) {
        const ShapeElement& element_prev = this->elements[element_prev_pos];
        const ShapeElement& element = this->elements[element_cur_pos];

        if (element_prev.type == ShapeElementType::LineSegment
                && element.type == ShapeElementType::LineSegment
                && !(element_prev.start == element_prev.end)
                && !(element.start == element.end)) {
            Angle angle = angle_radian(
                    element_prev.start - element_prev.end,
                    element.end - element.start);
            if (angle == 0) {
                std::cout << this->to_string(1) << std::endl;
                std::cout << "element_prev_pos  " << element_prev_pos << std::endl;
                std::cout << "element_pos       " << element_cur_pos << std::endl;
                std::cout << "element_prev  " << element_prev.to_string() << std::endl;
                std::cout << "element       " << element.to_string() << std::endl;
                std::cout << "angle  " << angle << std::endl;
                return false;
            }

            if (element_prev.start == element.end) {
                std::cout << this->to_string(1) << std::endl;
                std::cout << "element_pos       " << element_cur_pos << std::endl;
                std::cout << "element_prev_pos  " << element_prev_pos << std::endl;
                std::cout << "element       " << element.to_string() << std::endl;
                std::cout << "element_prev  " << element_prev.to_string() << std::endl;
                return false;
            }
        }

        element_prev_pos = element_cur_pos;
    }
    // TODO
    return true;
}

std::string Shape::to_string(
        Counter indentation) const
{
    std::string s = "";
    std::string indent = std::string(indentation, ' ');
    if (is_circle()) {
        LengthDbl radius = distance(elements.front().center, elements.front().start);
        s += "circle (radius: " + std::to_string(radius) + ")";
    } else if (is_square()) {
        s += "square (side: " + std::to_string(elements.front().length()) + ")";
    } else if (is_rectangle()) {
        s += "rectangle"
            " (side 1: " + std::to_string(elements[0].length())
            + "; side 2: " + std::to_string(elements[1].length()) + ")";
    } else if (is_polygon()) {
        s += "polygon (# edges " + std::to_string(elements.size()) + ")\n";
        for (Counter pos = 0; pos < (Counter)elements.size(); ++pos)
            s += indent + elements[pos].to_string() + ((pos < (Counter)elements.size() - 1)? "\n": "");
    } else  {
        s += "shape (# elements " + std::to_string(elements.size()) + ")\n";
        for (Counter pos = 0; pos < (Counter)elements.size(); ++pos)
            s += indent + elements[pos].to_string() + ((pos < (Counter)elements.size() - 1)? "\n": "");
    }
    return s;
}

nlohmann::json Shape::to_json() const
{
    nlohmann::json json;
    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)elements.size();
            ++element_pos) {
        json[element_pos] = elements[element_pos].to_json();
    }
    return json;
}

std::string Shape::to_svg(double factor) const
{
    std::string s = "M";
    for (const ShapeElement& element: elements) {
        Point center = {element.center.x * factor, -(element.center.y * factor)};
        Point start = {element.start.x * factor, -(element.start.y * factor)};
        Point end = {element.end.x * factor, -(element.end.y * factor)};
        s += std::to_string(start.x) + "," + std::to_string(start.y);
        if (element.type == ShapeElementType::LineSegment) {
            s += "L";
        } else {
            LengthDbl radius = distance(center, start);
            Angle theta = angle_radian(start - center, end - center);
            int large_arc_flag = (theta > M_PI)? 0: 1;
            int sweep_flag = (element.anticlockwise)? 0: 1;
            s += "A" + std::to_string(radius) + ","
                + std::to_string(radius) + ",0,"
                + std::to_string(large_arc_flag) + ","
                + std::to_string(sweep_flag) + ",";
        }
    }
    s += std::to_string(elements.front().start.x * factor)
        + "," + std::to_string(-(elements.front().start.y * factor))
        + "Z";
    return s;
}

void Shape::write_svg(
        const std::string& file_path) const
{
    if (file_path.empty())
        return;
    std::ofstream file{file_path};
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + file_path + "\".");
    }
    auto mm = compute_min_max(0.0);

    LengthDbl width = (mm.second.x - mm.first.x);
    LengthDbl height = (mm.second.y - mm.first.y);

    double factor = compute_svg_factor(width);

    std::string s = "<svg viewBox=\""
        + std::to_string(mm.first.x * factor)
        + " " + std::to_string(-mm.first.y * factor - height * factor)
        + " " + std::to_string(width * factor)
        + " " + std::to_string(height * factor)
        + "\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    file << s;

    file << "<path d=\"" << to_svg(factor) << "\""
        << " stroke=\"black\""
        << " stroke-width=\"1\""
        << " fill=\"blue\""
        << " fill-opacity=\"0.2\""
        << "/>" << std::endl;

    file << "</svg>" << std::endl;
}

Shape shape::operator*(
        LengthDbl scalar,
        const Shape& shape)
{
    Shape shape_new;
    for (const ShapeElement& element: shape.elements)
        shape_new.elements.push_back(scalar * element);
    return shape_new;
}

Shape shape::approximate_by_line_segments(
        const Shape& shape_orig,
        LengthDbl segment_length,
        bool outer)
{
    Shape shape = remove_redundant_vertices(shape_orig).second;

    Shape shape_new;
    for (const ShapeElement& element: shape.elements) {
        switch (element.type) {
        case ShapeElementType::LineSegment: {
            shape_new.elements.push_back(element);
            break;
        } case ShapeElementType::CircularArc: {
            std::vector<ShapeElement> approximated_element
                = approximate_circular_arc_by_line_segments(
                        element,
                        segment_length,
                        outer);
            for (const ShapeElement& e: approximated_element)
                shape_new.elements.push_back(e);
            break;
        }
        }
    }

    shape_new = remove_redundant_vertices(shape_new).second;
    return shape_new;
}

Shape shape::build_shape(
        const std::vector<BuildShapeElement>& points,
        bool path)
{
    Shape shape;
    Point point_prev = {points.front().x, points.front().y};
    ShapeElementType type = ShapeElementType::LineSegment;
    bool anticlockwise = false;
    Point center = {0, 0};
    for (ElementPos pos = 1; pos <= (ElementPos)points.size(); ++pos) {
        const BuildShapeElement& point = points[(pos != points.size())? pos: 0];
        if (path && pos == points.size())
            break;
        if (point.type == 0) {
            ShapeElement element;
            element.type = type;
            element.start = point_prev;
            element.end = {point.x, point.y};
            element.center = center;
            element.anticlockwise = anticlockwise;
            shape.elements.push_back(element);

            point_prev = element.end;
            anticlockwise = true;
            center = {0, 0};
            type = ShapeElementType::LineSegment;
        } else {
            anticlockwise = (point.type == 1);
            center = {point.x, point.y};
            type = ShapeElementType::CircularArc;
        }
    }
    return shape;
}

double shape::compute_svg_factor(
        double width)
{
    double factor = 1;
    while (width * factor > 10000)
        factor /= 10;
    while (width * factor < 1000)
        factor *= 10;
    return factor;
}

std::string shape::to_svg(
        const Shape& shape,
        const std::vector<Shape>& holes,
        double factor,
        const std::string& fill_color)
{
    std::string s = "<path d=\"" + shape.to_svg(factor);
    for (const Shape& hole: holes)
        s += hole.reverse().to_svg(factor);
    s += "\""
        " stroke=\"black\""
        " stroke-width=\"1\"";
    if (!fill_color.empty()) {
        s += " fill=\"" + fill_color + "\""
            " fill-opacity=\"0.2\"";
    }
    s += "/>\n";
    return s;
}

void shape::write_svg(
        const Shape& shape,
        const std::vector<Shape>& holes,
        const std::string& file_path)
{
    if (file_path.empty())
        return;
    std::ofstream file{file_path};
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + file_path + "\".");
    }

    auto mm = shape.compute_min_max(0.0);
    LengthDbl width = (mm.second.x - mm.first.x);
    LengthDbl height = (mm.second.y - mm.first.y);

    double factor = compute_svg_factor(width);

    std::string s = "<svg viewBox=\""
        + std::to_string(mm.first.x * factor)
        + " " + std::to_string(-mm.first.y * factor - height * factor)
        + " " + std::to_string(width * factor)
        + " " + std::to_string(height * factor)
        + "\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    file << s;

    file << "<g>" << std::endl;
    file << to_svg(shape, holes, factor);
    //file << "<text x=\"" << std::to_string(x * factor)
    //    << "\" y=\"" << std::to_string(-y * factor)
    //    << "\" dominant-baseline=\"middle\" text-anchor=\"middle\">"
    //    << std::to_string(item_shape_pos)
    //    << "</text>" << std::endl;
    file << "</g>" << std::endl;

    file << "</svg>" << std::endl;
}

std::pair<bool, Shape> shape::remove_redundant_vertices(
        const Shape& shape)
{
    //std::cout << "remove_redundant_vertices " << shape.to_string(2) << std::endl;
    if (!shape.check()) {
        throw std::invalid_argument(
                "shape::remove_redundant_vertices: invalid input shape.");
    }

    if (shape.elements.size() <= 3)
        return {false, shape};

    ElementPos number_of_elements_removed = 0;
    Shape shape_new;

    ElementPos element_prev_pos = shape.elements.size() - 1;
    for (ElementPos element_cur_pos = 0;
            element_cur_pos < (ElementPos)shape.elements.size();
            ++element_cur_pos) {

        //std::cout << "element_cur_pos " << element_cur_pos << std::endl;
        ElementPos element_next_pos = element_cur_pos + 1;
        const ShapeElement& element_prev = shape.elements[element_prev_pos];
        const ShapeElement& element = shape.elements[element_cur_pos];
        const ShapeElement& element_next = (element_next_pos < shape.elements.size())?
            shape.elements[element_next_pos]:
            shape_new.elements.front();
        bool useless = false;
        if (equal(element.start.x, element.end.x)
                && equal(element.start.y, element.end.y)) {
            useless = true;
        }
        if (!useless || shape.elements.size() - number_of_elements_removed <= 3) {
            if (!shape_new.elements.empty())
                shape_new.elements.back().end = element.start;
            shape_new.elements.push_back(element);
            element_prev_pos = element_cur_pos;
        } else {
            number_of_elements_removed++;
        }
    }
    shape_new.elements.back().end = shape_new.elements.front().start;

    if (!shape_new.check()) {
        throw std::invalid_argument(
                "shape::remove_redundant_vertices: invalid output shape.");
    }
    return {(number_of_elements_removed > 0), shape_new};
}

std::pair<bool, Shape> shape::remove_aligned_vertices(
        const Shape& shape)
{
    //std::cout << "remove_aligned_vertices " << shape.to_string(2) << std::endl;

    if (shape.elements.size() <= 3)
        return {false, shape};

    ElementPos number_of_elements_removed = 0;
    Shape shape_new;

    ElementPos element_prev_pos = shape.elements.size() - 1;
    for (ElementPos element_cur_pos = 0;
            element_cur_pos < (ElementPos)shape.elements.size();
            ++element_cur_pos) {

        //std::cout << "element_cur_pos " << element_cur_pos << std::endl;
        ElementPos element_next_pos = element_cur_pos + 1;
        const ShapeElement& element_prev = shape.elements[element_prev_pos];
        const ShapeElement& element = shape.elements[element_cur_pos];
        const ShapeElement& element_next = (element_next_pos < shape.elements.size())?
            shape.elements[element_next_pos]:
            shape_new.elements.front();
        bool useless = false;
        if (element.type == ShapeElementType::LineSegment
                && element_prev.type == ShapeElementType::LineSegment) {
            double v = compute_area(element_prev.start, element.start, element_next.start);
            //std::cout << "element_prev  " << element_prev_pos << " " << element_prev.to_string() << std::endl;
            //std::cout << "element       " << element_cur_pos << " " << element.to_string() << std::endl;
            //std::cout << "element_next  " << element_next_pos << " " << element_next.to_string() << std::endl;
            //std::cout << "v " << v << std::endl;
            if (element_prev.start.x == element.start.x
                    && element.start.x == element_next.start.x) {
                //std::cout << "useless " << element.to_string() << std::endl;
                useless = true;
            }
            if (element_prev.start.y == element.start.y
                    && element.start.y == element_next.start.y) {
                //std::cout << "useless " << element.to_string() << std::endl;
                useless = true;
            }
        }
        if (!useless || shape.elements.size() - number_of_elements_removed <= 3) {
            if (!shape_new.elements.empty())
                shape_new.elements.back().end = element.start;
            shape_new.elements.push_back(element);
            element_prev_pos = element_cur_pos;
        } else {
            number_of_elements_removed++;
        }
    }
    shape_new.elements.back().end = shape_new.elements.front().start;

    return {(number_of_elements_removed > 0), shape_new};
}

namespace
{

std::pair<bool, Shape> clean_extreme_slopes_rec(
        const Shape& shape,
        bool outer)
{
    //std::cout << "clean_extreme_slopes " << shape.to_string(2) << std::endl;
    if (!shape.check()) {
        throw std::invalid_argument(
                "shape::clean_extreme_slopes: invalid input shape.");
    }

    bool found = false;
    Shape shape_new;

    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)shape.elements.size();
            ++element_pos) {
        ShapeElement element = shape.elements[element_pos];
        const ShapeElement& element_prev = (!shape_new.elements.empty())?
            shape_new.elements.back():
            shape.elements.back();
        const ShapeElement& element_next = (element_pos < shape.elements.size() - 1)?
            shape.elements[element_pos + 1]:
            shape_new.elements.front();
        if (element_pos > 0)
            element.start = shape_new.elements.back().end;
        if (element_pos == shape.elements.size() - 1)
            element.end = shape_new.elements.front().start;

        double slope
            = (element.end.y - element.start.y)
            / (element.end.x - element.start.x);
        //std::cout << "element " << element.to_string() << " slope " << slope << std::endl;
        //std::cout << "element_prev " << element_prev.to_string() << std::endl;
        if (element.start.x != element.end.x && std::abs(slope) > 1e2) {
            if (equal(element.start.x, element.end.x)) {
                found = true;
                //std::cout << "found x" << std::endl;
                element.end.x = element.start.x;
            } else if (slope > 1e2) {
                found = true;
                //std::cout << "found a" << std::endl;
                //std::cout << "   " << element.to_string() << std::endl;
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                if (outer) {
                    element_new.end.x = element.end.x;
                    element_new.end.y = element.start.y;
                } else {
                    element_new.end.x = element.start.x;
                    element_new.end.y = element.end.y;
                }
                Angle angle_1 = angle_radian(
                            element_prev.start - element_prev.end,
                            element_new.end - element_new.start);
                Angle angle_2 = angle_radian(
                            element_new.start - element_new.end,
                            element.end - element_new.end);
                Angle angle_3 = angle_radian(
                            element_new.end - element.end,
                            element_next.end - element_next.start);
                if (angle_1 != 0 && angle_2 != 0 && angle_3 != 0) {
                    element.start = element_new.end;
                    shape_new.elements.push_back(element_new);
                    //std::cout << "angle_1 " << angle_1 << " angle_2 " << angle_2 << " angle_3 " << angle_3 << std::endl;
                    //std::cout << "-> " << element_new.to_string() << std::endl;
                    //std::cout << "-> " << element.to_string() << std::endl;
                } else {
                    element.end.x = element.start.x;
                }
            } else if (slope < -1e2) {
                found = true;
                //std::cout << "found b" << std::endl;
                //std::cout << "   " << element.to_string() << std::endl;
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                if (outer) {
                    element_new.end.x = element.start.x;
                    element_new.end.y = element.end.y;
                } else {
                    element_new.end.x = element.end.x;
                    element_new.end.y = element.start.y;
                }
                Angle angle_1 = angle_radian(
                            element_prev.start - element_prev.end,
                            element_new.end - element_new.start);
                Angle angle_2 = angle_radian(
                            element_new.start - element_new.end,
                            element.end - element_new.end);
                Angle angle_3 = angle_radian(
                            element_new.end - element.end,
                            element_next.end - element_next.start);
                if (angle_1 != 0 && angle_2 != 0 && angle_3 != 0) {
                    element.start = element_new.end;
                    shape_new.elements.push_back(element_new);
                    //std::cout << "angle_1 " << angle_1 << " angle_2 " << angle_2 << " angle_3 " << angle_3 << std::endl;
                    //std::cout << "-> " << element_new.to_string() << std::endl;
                    //std::cout << "-> " << element.to_string() << std::endl;
                } else {
                    element.end.x = element.start.x;
                }
            }
        } else if (element.start.y != element.end.y && std::abs(slope) < 1e-2) {
            if (equal(element.start.y, element.end.y)) {
                found = true;
                //std::cout << "found y" << std::endl;
                element.end.y = element.start.y;
            } else if (slope > 0 && slope < 1e-2) {
                found = true;
                //std::cout << "found c" << std::endl;
                //std::cout << "   " << element.to_string() << std::endl;
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                if (outer) {
                    element_new.end.x = element.end.x;
                    element_new.end.y = element.start.y;
                } else {
                    element_new.end.x = element.start.x;
                    element_new.end.y = element.end.y;
                }
                Angle angle_1 = angle_radian(
                            element_prev.start - element_prev.end,
                            element_new.end - element_new.start);
                Angle angle_2 = angle_radian(
                            element_new.start - element_new.end,
                            element.end - element_new.end);
                Angle angle_3 = angle_radian(
                            element_new.end - element.end,
                            element_next.end - element_next.start);
                if (angle_1 != 0 && angle_2 != 0 && angle_3 != 0) {
                    element.start = element_new.end;
                    shape_new.elements.push_back(element_new);
                    //std::cout << "angle_1 " << angle_1 << " angle_2 " << angle_2 << " angle_3 " << angle_3 << std::endl;
                    //std::cout << "-> " << element_new.to_string() << std::endl;
                    //std::cout << "-> " << element.to_string() << std::endl;
                } else {
                    element.end.y = element.start.y;
                }
            } else if (slope < 0 && slope > -1e-2) {
                found = true;
                //std::cout << "found d" << std::endl;
                //std::cout << "   " << element.to_string() << std::endl;
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                if (outer) {
                    element_new.end.x = element.start.x;
                    element_new.end.y = element.end.y;
                } else {
                    element_new.end.x = element.end.x;
                    element_new.end.y = element.start.y;
                }
                Angle angle_1 = angle_radian(
                            element_prev.start - element_prev.end,
                            element_new.end - element_new.start);
                Angle angle_2 = angle_radian(
                            element_new.start - element_new.end,
                            element.end - element_new.end);
                Angle angle_3 = angle_radian(
                            element_new.end - element.end,
                            element_next.end - element_next.start);
                if (angle_1 != 0 && angle_2 != 0 && angle_3 != 0) {
                    element.start = element_new.end;
                    shape_new.elements.push_back(element_new);
                    //std::cout << "angle_1 " << angle_1 << " angle_2 " << angle_2 << " angle_3 " << angle_3 << std::endl;
                    //std::cout << "-> " << element_new.to_string() << std::endl;
                    //std::cout << "-> " << element.to_string() << std::endl;
                } else {
                    element.end.y = element.start.y;
                }
            }
        }
        if (!shape_new.elements.empty())
            shape_new.elements.back().end = element.start;
        shape_new.elements.push_back(element);
    }
    shape_new.elements.front().start = shape_new.elements.back().end;

    shape_new = remove_aligned_vertices(shape_new).second;

    if (!shape_new.check()) {
        throw std::invalid_argument(
                "shape::clean_extreme_slopes: invalid output shape.");
    }
    return {found, shape_new};
}

}

Shape shape::clean_extreme_slopes(
        const Shape& shape,
        bool outer)
{
    Shape shape_new = shape;
    for (int i = 0;; ++i) {
        if (i == 100) {
            throw std::runtime_error(
                    "packingsolver::irregular::process_shape_outer: "
                    "too many iterations.");
        }
        auto res = clean_extreme_slopes_rec(shape_new, outer);
        if (!res.first)
            break;
        shape_new = res.second;
    }
    return shape_new;
}

bool shape::operator==(
        const ShapeElement& element_1,
        const ShapeElement& element_2)
{
    if (element_1.type != element_2.type)
        return false;
    if (!(element_1.start == element_2.start))
        return false;
    if (!(element_1.end == element_2.end))
        return false;
    if (element_1.type == ShapeElementType::CircularArc) {
        if (!(element_1.center == element_2.center))
            return false;
        if (element_1.anticlockwise != element_2.anticlockwise)
            return false;
    }
    return true;
}

bool shape::equal(
        const ShapeElement& element_1,
        const ShapeElement& element_2)
{
    if (element_1.type != element_2.type)
        return false;
    if (!equal(element_1.start, element_2.start))
        return false;
    if (!equal(element_1.end, element_2.end))
        return false;
    if (element_1.type == ShapeElementType::CircularArc) {
        if (!equal(element_1.center, element_2.center))
            return false;
        if (element_1.anticlockwise != element_2.anticlockwise)
            return false;
    }
    return true;
}

bool shape::operator==(
        const Shape& shape_1,
        const Shape& shape_2)
{
    // First, check if both shapes have the same number of elements.
    if (shape_1.elements.size() != shape_2.elements.size())
        return false;

    ElementPos offset = -1;
    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)shape_2.elements.size();
            ++element_pos) {
        if (shape_2.elements[element_pos] == shape_1.elements[0]) {
            offset = element_pos;
            break;
        }
    }
    if (offset == -1)
        return false;

    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)shape_2.elements.size();
            ++element_pos) {
        ElementPos element_pos_2 = (element_pos + offset) % shape_2.elements.size();
        if (!(shape_1.elements[element_pos] == shape_2.elements[element_pos_2])) {
            return false;
        }
    }

    return true;
}

bool shape::equal(
        const Shape& shape_1,
        const Shape& shape_2)
{
    // First, check if both shapes have the same number of elements.
    if (shape_1.elements.size() != shape_2.elements.size())
        return false;

    ElementPos offset = -1;
    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)shape_2.elements.size();
            ++element_pos) {
        if (equal(shape_2.elements[element_pos], shape_1.elements[0])) {
            offset = element_pos;
            break;
        }
    }
    if (offset == -1)
        return false;

    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)shape_2.elements.size();
            ++element_pos) {
        ElementPos element_pos_2 = (element_pos + offset) % shape_2.elements.size();
        if (!equal(shape_1.elements[element_pos], shape_2.elements[element_pos_2])) {
            return false;
        }
    }

    return true;
}
