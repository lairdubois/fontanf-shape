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

std::vector<ShapeWithHoles> simplify(
        const std::vector<SimplifyInputShape>& shapes,
        AreaDbl maximum_approximation_area);

void simplify_export_inputs(
        const std::vector<SimplifyInputShape>& shapes,
        AreaDbl maximum_approximation_area,
        const std::string& file_path);

}
