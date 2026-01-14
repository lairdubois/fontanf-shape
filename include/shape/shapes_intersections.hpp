#pragma once

#include "shape/shape.hpp"

namespace shape
{

bool intersect(
        const ShapeElement& element_1,
        const ShapeElement& element_2);

bool intersect(
        const Shape& shape);

bool intersect(
        const Shape& shape,
        const ShapeElement& element,
        bool strict = false);

inline bool intersect(
        const ShapeElement& element,
        const Shape& shape,
        bool strict = false)
{
    return intersect(shape, element, strict);
}

std::vector<ShapePoint> compute_intersections(
        const ShapeElement& element,
        const Shape& shape,
        bool strict = false,
        bool only_first = false);

struct PathShapeIntersectionPoint
{
    ElementPos path_element_pos;
    ElementPos shape_element_pos;
    Point point;
};

std::vector<PathShapeIntersectionPoint> compute_intersections(
        const Shape& path,
        const Shape& shape,
        bool strict = false,
        bool only_first = false);

void compute_intersections_export_inputs(
        const std::string& file_path,
        const Shape& path,
        const Shape& shape,
        bool strict = false,
        bool only_first = false);

bool intersect(
        const Shape& shape_1,
        const Shape& shape_2,
        bool strict = false);

bool intersect(
        const ShapeWithHoles& shape,
        const ShapeElement& element,
        bool strict = false);

inline bool intersect(
        const ShapeElement& element,
        const ShapeWithHoles& shape_with_holes,
        bool strict = false)
{
    return intersect(shape_with_holes, element, strict);
}

bool intersect(
        const ShapeWithHoles& shape_with_holes_1,
        const Shape& shape_2,
        bool strict = false);

inline bool intersect(
        const Shape& shape_1,
        const ShapeWithHoles& shape_with_holes_2,
        bool strict = false)
{
    return intersect(shape_with_holes_2, shape_1, strict);
}

bool intersect(
        const ShapeWithHoles& shape_with_holes_1,
        const ShapeWithHoles& shape_with_holes_2,
        bool strict = false);

}
