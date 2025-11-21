#pragma once

#include "shape/shape.hpp"

namespace shape
{

std::vector<Shape> compute_convex_partition(
        const ShapeWithHoles& shape);

}
