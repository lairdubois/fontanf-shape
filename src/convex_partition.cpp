#include "shape/convex_partition.hpp"

#include "shape/trapezoidation.hpp"

using namespace shape;

std::vector<Shape> shape::compute_convex_partition(
        const ShapeWithHoles& shape)
{
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);

    //std::vector<TrapezoidPos> trapezoids_matching_top(trapezoids.size(), -1);
    //std::vector<TrapezoidPos> trapezoids_matching_bottom(trapezoids.size(), -1);

    std::vector<Shape> res;
    for (const GeneralizedTrapezoid& trapezoid: trapezoids)
        res.push_back(trapezoid.to_shape());

    return res;
}
