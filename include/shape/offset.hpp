#pragma once

#include "shape/shape.hpp"

namespace shape
{

ShapeWithHoles inflate(
        const Shape& shape,
        LengthDbl offset,
        bool remove_self_intersections = true);

std::vector<Shape> deflate(
        const Shape& shape,
        LengthDbl offset,
        bool extract_all_holes = true);

}
