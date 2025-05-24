#pragma once

#include "shape/shape.hpp"

namespace shape
{

struct ShapeSupports
{
    std::vector<Shape> supporting_parts;

    std::vector<Shape> supported_parts;
};

ShapeSupports compute_shape_supports(
        const Shape& shape,
        bool is_hole);

ShapeSupports compute_shape_supports(
        const ShapeWithHoles& shape);

}
