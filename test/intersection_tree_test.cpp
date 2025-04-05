#include "shape/intersection_tree.hpp"

#include <gtest/gtest.h>

using namespace shape;

struct IntersectionTreeTestParams
{
    std::vector<Shape> shapes;
    bool strict = false;
    std::vector<std::pair<Shape, std::vector<ShapePos>>> test_shapes;
    std::vector<std::pair<ShapePos, ShapePos>> expected_intersecting_shapes;
};

class IntersectionTreeTest: public testing::TestWithParam<IntersectionTreeTestParams> { };

TEST_P(IntersectionTreeTest, IntersectionTree)
{
    IntersectionTreeTestParams test_params = GetParam();
    IntersectionTree intersection_tree(test_params.shapes);

    for (const auto& p: test_params.test_shapes) {
        std::vector<ShapePos> intersections = intersection_tree.intersect(p.first, test_params.strict);
        EXPECT_EQ(intersections.size(), p.second.size());
        for (ShapePos expected_shape_pos: p.second) {
            EXPECT_NE(std::find(
                        intersections.begin(),
                        intersections.end(),
                        expected_shape_pos),
                    intersections.end());
        }
    }

    std::vector<std::pair<ShapePos, ShapePos>> intersecting_shapes = intersection_tree.compute_intersecting_shapes(test_params.strict);
    EXPECT_EQ(intersecting_shapes.size(), test_params.expected_intersecting_shapes.size());
    for (auto expected_p: test_params.expected_intersecting_shapes) {
        EXPECT_NE(std::find(
                    intersecting_shapes.begin(),
                    intersecting_shapes.end(),
                    expected_p),
                intersecting_shapes.end());
    }
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        IntersectionTreeTest,
        testing::ValuesIn(std::vector<IntersectionTreeTestParams>{
            {
                {build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}})},
                true,
                {},
                {}
            }, {
                {build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}})},
                true,
                {
                    {build_shape({{3, 0}, {5, 0}, {5, 2}, {3, 2}}), {}},
                    {build_shape({{1, 0}, {3, 0}, {3, 2}, {1, 2}}), {0}}
                },
                {}
            }, {
                {
                    build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}}),
                    build_shape({{1, 0}, {3, 0}, {3, 2}, {1, 2}}),
                },
                true,
                {},
                {{0, 1}}
            },
            }));
