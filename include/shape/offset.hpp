#pragma once

#include "shape/shape.hpp"

namespace shape
{

std::pair<Shape, std::vector<Shape>> inflate(
        const Shape& shape,
        LengthDbl offset);

std::vector<Shape> deflate(
        const Shape& shape,
        LengthDbl offset);

}
