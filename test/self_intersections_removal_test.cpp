#include "shape/self_intersections_removal.hpp"

#include <gtest/gtest.h>

using namespace shape;


struct SelfIntersectionsRemovalTestParams
{
    Shape shape;
    ShapeWithHoles expected_shape;
};

class SelfIntersectionsRemovalTest: public testing::TestWithParam<SelfIntersectionsRemovalTestParams> { };

TEST_P(SelfIntersectionsRemovalTest, SelfIntersectionsRemoval)
{
    SelfIntersectionsRemovalTestParams test_params = GetParam();
    std::cout << "shape " << test_params.shape.to_string(0) << std::endl;
    std::cout << "expected_shape " << test_params.expected_shape.to_string(0) << std::endl;

    auto shape = remove_self_intersections(test_params.shape);
    std::cout << "shape " << shape.to_string(0) << std::endl;

    EXPECT_TRUE(equal(shape, test_params.expected_shape));
}

INSTANTIATE_TEST_SUITE_P(
        ,
        SelfIntersectionsRemovalTest,
        testing::ValuesIn(std::vector<SelfIntersectionsRemovalTestParams>{
            {  // No self intersection
                build_shape({{0, 0}, {4, 0}, {4, 4}, {2.5, 4}, {2.5, 3}, {3, 3}, {3, 1}, {1, 1}, {1, 3}, {1.5, 3}, {1.5, 4}, {0, 4}}),
                build_shape({{0, 0}, {4, 0}, {4, 4}, {2.5, 4}, {2.5, 3}, {3, 3}, {3, 1}, {1, 1}, {1, 3}, {1.5, 3}, {1.5, 4}, {0, 4}}),
                {},
            }, {
                build_shape({{0, 0}, {4, 0}, {4, 3}, {3, 3}, {1, 4}, {3, 1}, {1, 1}, {3, 4}, {1, 3}, {0, 3}}),
                build_shape({{0, 0}, {4, 0}, {4, 3}, {3, 3}, {2.5, 3.25}, {3, 4}, {2, 3.5}, {1, 4}, {1.5, 3.25}, {1, 3}, {0, 3}}),
                {build_shape({{1, 1}, {3, 1}, {2, 2.5}})},
            }, {
                build_shape({{0, 0}, {4, 0}, {4, 4}, {1, 4}, {3, 3}, {3, 1}, {1, 1}, {1, 3}, {3, 4}, {0, 4}}),
                build_shape({{0, 0}, {4, 0}, {4, 4}, {0, 4}}),
                {build_shape({{1, 1}, {3, 1}, {3, 3}, {2, 3.5}, {1, 3}})},
            }, {
                build_shape({{0, 0}, {4, 0}, {4, 4}, {2, 4}, {3, 3}, {3, 1}, {1, 1}, {1, 3}, {2, 4}, {0, 4}}),
                build_shape({{0, 0}, {4, 0}, {4, 4}, {0, 4} }),
                {build_shape({ {1, 1}, {3, 1}, {3, 3}, {2, 4}, {1, 3}})},
            }, {
                build_shape({{0, 0}, {4, 0}, {4, 4}, {2, 4}, {3, 3}, {3, 1}, {2, 4}, {1, 1}, {1, 3}, {2, 4}, {0, 4}}),
                build_shape({{0, 0}, {4, 0}, {4, 4}, {0, 4}}),
                {
                    build_shape({{3, 1}, {3, 3}, {2, 4}}),
                    build_shape({{1, 1}, {2, 4}, {1, 3}})
                },
            }, {
                build_shape({{0, 0}, {4, 0}, {4, 4}, {2, 4}, {2, 3}, {3, 3}, {3, 1}, {1, 1}, {1, 3}, {2, 3}, {2, 4}, {0, 4}}),
                build_shape({{0, 0}, {4, 0}, {4, 4}, {0, 4}}),
                {build_shape({{1, 1}, {3, 1}, {3, 3}, {1, 3}})}
            }, {
                build_shape({{0, 0}, {4, 0}, {4, 4}, {1, 4}, {1, 1}, {3, 1}, {3, 4}, {0, 4}}),
                build_shape({{0, 0}, {4, 0}, {4, 4}, {0, 4}}),
                {}
            }}));


struct ExtractHolesTestParams
{
    Shape hole;
    std::vector<Shape> expected_holes;
};

class ExtractHolesTest: public testing::TestWithParam<ExtractHolesTestParams> { };

TEST_P(ExtractHolesTest, ExtractHoles)
{
    ExtractHolesTestParams test_params = GetParam();
    std::cout << "hole " << test_params.hole.to_string(0) << std::endl;
    std::cout << "expected_holes" << std::endl;
    for (const Shape& hole: test_params.expected_holes)
        std::cout << "- " << hole.to_string(2) << std::endl;

    auto holes = extract_all_holes_from_self_intersecting_hole(test_params.hole);
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
        ,
        ExtractHolesTest,
        testing::ValuesIn(std::vector<ExtractHolesTestParams>{
            {  // No self intersection
                build_shape({{0, 0}, {4, 0}, {4, 4}, {2.5, 4}, {2.5, 3}, {3, 3}, {3, 1}, {1, 1}, {1, 3}, {1.5, 3}, {1.5, 4}, {0, 4}}),
                {build_shape({{0, 0}, {4, 0}, {4, 4}, {2.5, 4}, {2.5, 3}, {3, 3}, {3, 1}, {1, 1}, {1, 3}, {1.5, 3}, {1.5, 4}, {0, 4}})},
            }, {
                build_shape({{0, 0}, {4, 0}, {0, 2}, {4, 4}, {0, 4}, {4, 2}}),
                {
                    build_shape({{0, 0}, {4, 0}, {2, 1}}),
                    build_shape({{2, 3}, {4, 4}, {0, 4}}),
                },
            }}));


struct ComputeUnionTestParams
{
    Shape shape_1;
    Shape shape_2;
    ShapeWithHoles expected_shape;
};

class ComputeUnionTest: public testing::TestWithParam<ComputeUnionTestParams> { };

TEST_P(ComputeUnionTest, ComputeUnion)
{
    ComputeUnionTestParams test_params = GetParam();
    std::cout << "shape_1 " << test_params.shape_1.to_string(0) << std::endl;
    std::cout << "shape_2 " << test_params.shape_2.to_string(0) << std::endl;
    std::cout << "expected_shape " << test_params.expected_shape.to_string(0) << std::endl;

    ShapeWithHoles shape = compute_union(
            test_params.shape_1,
            test_params.shape_2);
    std::cout << "shape " << shape.to_string(0) << std::endl;

    EXPECT_TRUE(equal(shape, test_params.expected_shape));
}

INSTANTIATE_TEST_SUITE_P(
        ,
        ComputeUnionTest,
        testing::ValuesIn(std::vector<ComputeUnionTestParams>{
            {
                build_shape({{0, 0}, {2, 0}, {2, 1}, {0, 1}}),
                build_shape({{1, 0}, {3, 0}, {3, 1}, {1, 1}}),
                build_shape({{0, 0}, {3, 0}, {3, 1}, {0, 1}}),
                {},
            }, {
                build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}}),
                build_shape({{1, 1}, {3, 1}, {3, 3}, {1, 3}}),
                build_shape({{0, 0}, {2, 0}, {2, 1}, {3, 1}, {3, 3}, {1, 3}, {1, 2}, {0, 2}}),
                {},
            }, {
                build_shape({{0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 2}, {3, 2}, {3, 3}, {0, 3}}),
                build_shape({{2, 0}, {5, 0}, {5, 3}, {2, 3}, {2, 2}, {4, 2}, {4, 1}, {2, 1}}),
                build_shape({{0, 0}, {5, 0}, {5, 3}, {0, 3}}),
                {build_shape({{1, 1}, {4, 1}, {4, 2}, {1, 2}})},
            }}));
