#pragma once

#include "shape/trapezoid.hpp"

namespace shape
{

std::vector<GeneralizedTrapezoid> trapezoidation(
        const Shape& shape,
        const std::vector<Shape>& holes = {});

}
