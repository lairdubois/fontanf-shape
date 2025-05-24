#pragma once

#include "shape/shape.hpp"

namespace shape
{

ShapeWithHoles remove_self_intersections(
        const Shape& shape);

ShapeWithHoles compute_union(
        const Shape& shape_1,
        const Shape& shape_2);

std::vector<Shape> extract_all_holes_from_self_intersecting_hole(
        const Shape& shape);

}
