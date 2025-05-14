#pragma once

#include "shape/trapezoid.hpp"

namespace shape
{

std::vector<GeneralizedTrapezoid> trapezoidation(
        const ShapeWithHoles& shape);

}
