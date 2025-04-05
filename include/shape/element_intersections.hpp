#pragma once

#include "shape/shape.hpp"

namespace shape
{

std::vector<Point> compute_intersections(
        const ShapeElement& element_1,
        const ShapeElement& element_2,
        bool strict = false);

bool intersect(
        const Shape& shape_1,
        const Shape& shape_2,
        bool strict = false);

bool intersect(
        const Shape& shape,
        const ShapeElement& element,
        bool strict = false);

}
