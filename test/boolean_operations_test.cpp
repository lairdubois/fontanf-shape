#include "shape/boolean_operations.hpp"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include <fstream>

using namespace shape;
namespace fs = boost::filesystem;


struct ComputeUnionTestParams
{
    std::vector<ShapeWithHoles> shapes;
    std::vector<ShapeWithHoles> expected_merged_shapes;
};

class ComputeUnionTest: public testing::TestWithParam<ComputeUnionTestParams> { };

TEST_P(ComputeUnionTest, ComputeUnion)
{
    ComputeUnionTestParams test_params = GetParam();
    std::cout << "shapes" << std::endl;
    for (const ShapeWithHoles& shape: test_params.shapes)
        std::cout << "- " << shape.to_string(2) << std::endl;
    std::cout << "expected_merged_shapes" << std::endl;
    for (const ShapeWithHoles& shape: test_params.expected_merged_shapes)
        std::cout << "- " << shape.to_string(2) << std::endl;

    std::vector<ShapeWithHoles> merged_shapes = compute_union(
            test_params.shapes);
    std::cout << "merged_shapes" << std::endl;
    for (const ShapeWithHoles& shape: merged_shapes)
        std::cout << "- " << shape.to_string(2) << std::endl;

    ASSERT_EQ(merged_shapes.size(), test_params.expected_merged_shapes.size());
    for (const ShapeWithHoles& expected_shape: test_params.expected_merged_shapes) {
        EXPECT_NE(std::find_if(
                    merged_shapes.begin(),
                    merged_shapes.end(),
                    [&expected_shape](const ShapeWithHoles& shape) { return equal(shape, expected_shape); }),
                merged_shapes.end());
    }
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        ComputeUnionTest,
        testing::ValuesIn(std::vector<ComputeUnionTestParams>{
            {
                {
                    {build_shape({{0, 0}, {2, 0}, {2, 1}, {0, 1}}), {}},
                    {build_shape({{1, 0}, {3, 0}, {3, 1}, {1, 1}}), {}},
                },
                {{build_shape({{0, 0}, {3, 0}, {3, 1}, {0, 1}}), {}}},
            }, {
                {
                    {build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}}), {}},
                    {build_shape({{1, 1}, {3, 1}, {3, 3}, {1, 3}}), {}},
                },
                {{build_shape({{0, 0}, {2, 0}, {2, 1}, {3, 1}, {3, 3}, {1, 3}, {1, 2}, {0, 2}}), {}}},
            }, {
                {
                    {build_shape({{0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 2}, {3, 2}, {3, 3}, {0, 3}}), {}},
                    {build_shape({{2, 0}, {5, 0}, {5, 3}, {2, 3}, {2, 2}, {4, 2}, {4, 1}, {2, 1}}), {}},
                },
                {{
                    build_shape({{0, 0}, {5, 0}, {5, 3}, {0, 3}}),
                    {build_shape({{1, 1}, {4, 1}, {4, 2}, {1, 2}})}
                }},
            }, {
                {
                    {build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}}), {}},
                    {build_shape({{1, 0}, {3, 0}, {3, 2}, {1, 2}}), {}},
                },
                {{build_shape({{0, 0}, {3, 0}, {3, 2}, {0, 2}}), {}}},
            }, {
                {
                    {build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}}), {}},
                    {build_shape({{1, 0}, {3, 0}, {3, 2}, {1, 2}}), {}},
                    {build_shape({{5, 0}, {7, 0}, {7, 2}, {5, 2}}), {}},
                },
                {
                    {build_shape({{0, 0}, {3, 0}, {3, 2}, {0, 2}}), {}},
                    {build_shape({{5, 0}, {7, 0}, {7, 2}, {5, 2}}), {}},
                },
            }}));
