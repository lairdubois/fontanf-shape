#include "shape/labeling.hpp"

#include <gtest/gtest.h>

using namespace shape;


struct LabelingTestParams
{
    Shape shape;
    std::vector<Shape> holes;
    Point expected_label_position;
};

class LabelingTest: public testing::TestWithParam<LabelingTestParams> { };

TEST_P(LabelingTest, Labeling)
{
    LabelingTestParams test_params = GetParam();
    std::cout << "shape " << test_params.shape.to_string(0) << std::endl;
    std::cout << "holes" << std::endl;
    for (const Shape& hole: test_params.holes)
        std::cout << "- " << hole.to_string(2) << std::endl;
    std::cout << "expected_label_position " << test_params.expected_label_position.to_string() << std::endl;

    auto label_position = find_label_position(test_params.shape, test_params.holes);

    EXPECT_TRUE(equal(label_position, test_params.expected_label_position));
}

INSTANTIATE_TEST_SUITE_P(
        ,
        LabelingTest,
        testing::ValuesIn(std::vector<LabelingTestParams>{
            {
                build_shape({{0, 0}, {1, 0}, {1, 1}, {0, 1}}),
                {},
                {0.5, 0.5}
            }}));
