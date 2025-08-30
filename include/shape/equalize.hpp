#pragma once

#include "shape/shape.hpp"

namespace shape
{

std::vector<LengthDbl> equalize_values(
        const std::vector<LengthDbl>& values);

std::vector<Point> equalize_points(
        const std::vector<Point>& points);

Shape equalize_shape(
        const Shape& shape);

ShapeWithHoles equalize_shape(
        const ShapeWithHoles& shape);

}
