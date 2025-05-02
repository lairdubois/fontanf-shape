#pragma once

#include "shape/shape.hpp"

namespace shape
{

struct SimplifyInputShape
{
    Shape shape;

    ShapePos copies;

    bool outer = true;
};

struct SimplifyOutputShape
{
    Shape shape;

    std::vector<Shape> holes;
};

std::vector<SimplifyOutputShape> simplify(
        const std::vector<SimplifyInputShape>& shapes,
        AreaDbl maximum_approximation_area);

}
