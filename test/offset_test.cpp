#include "shape/offset.hpp"

#include <gtest/gtest.h>

using namespace shape;


struct InflateTestParams
{
    Shape shape;
    LengthDbl offset;
    ShapeWithHoles expected_shape;
};

class InflateTest: public testing::TestWithParam<InflateTestParams> { };

TEST_P(InflateTest, Inflate)
{
    InflateTestParams test_params = GetParam();
    std::cout << "shape " << test_params.shape.to_string(0) << std::endl;
    std::cout << "offset " << test_params.offset << std::endl;
    std::cout << "expected_shape " << test_params.expected_shape.to_string(0) << std::endl;

    ShapeWithHoles shape = inflate(test_params.shape, test_params.offset);
    std::cout << "shape " << shape.to_string(0) << std::endl;

    EXPECT_TRUE(equal(shape, test_params.expected_shape));
}

INSTANTIATE_TEST_SUITE_P(
        ,
        InflateTest,
        testing::ValuesIn(std::vector<InflateTestParams>{
            {  // Square
                build_shape({{0, 0}, {1, 0}, {1, 1}, {0, 1}}),
                1.0,
                build_shape({{0, -1}, {1, -1}, {1, 0, 1},
                             {2, 0}, {2, 1}, {1, 1, 1},
                             {1, 2}, {0, 2}, {0, 1, 1},
                             {-1, 1}, {-1, 0}, {0, 0, 1}}),
                {},
            //}, {  // Triangle
            //    build_shape({{1, 0}, {3, 1}, {0, 1}}),
            //    10,
            //    build_shape({{0, -1}, {1, -1}, {1, 0, 1},
            //                 {2, 0}, {2, 1}, {1, 1, 1},
            //                 {1, 2}, {0, 2}, {0, 1, 1},
            //                 {-1, 1}, {-1, 0}, {0, 0, 1}}),
            //    {},
            }}));


struct DeflateTestParams
{
    Shape hole;
    LengthDbl offset;
    std::vector<Shape> expected_holes;
};

class DeflateTest: public testing::TestWithParam<DeflateTestParams> { };

TEST_P(DeflateTest, Deflate)
{
    DeflateTestParams test_params = GetParam();
    std::cout << "hole " << test_params.hole.to_string(0) << std::endl;
    std::cout << "offset " << test_params.offset << std::endl;
    std::cout << "expected_holes" << std::endl;
    for (const Shape& hole: test_params.expected_holes)
        std::cout << "- " << hole.to_string(2) << std::endl;

    auto holes = deflate(test_params.hole, test_params.offset);
    std::cout << "holes" << std::endl;
    for (const Shape& hole: holes)
        std::cout << "- " << hole.to_string(2) << std::endl;

    ASSERT_EQ(holes.size(), test_params.expected_holes.size());
    for (const Shape& expected_hole: test_params.expected_holes) {
        EXPECT_NE(std::find(
                    holes.begin(),
                    holes.end(),
                    expected_hole),
                holes.end());
    }
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        DeflateTest,
        testing::ValuesIn(std::vector<DeflateTestParams>{
            {  // No self intersection
                build_shape({{0, 0}, {4, 0}, {4, 4}, {0, 4}}),
                1.0,
                {build_shape({{1, 1}, {3, 1}, {3, 3}, {1, 3}})},
            }}));
