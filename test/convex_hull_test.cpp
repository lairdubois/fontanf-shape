#include "shape/convex_hull.hpp"

#include <gtest/gtest.h>

using namespace shape;

TEST(PolygonConvexHull, Triangle)
{
    Shape shape = build_shape({{0, 0}, {3, 0}, {1, 3}});

    Shape convex_hull = shape::convex_hull(shape);

    Shape expected_shape = build_shape({{0, 0}, {3, 0}, {1, 3}});
    EXPECT_EQ(expected_shape, convex_hull);
}
