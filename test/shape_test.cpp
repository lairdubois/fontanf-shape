#include "shape/shape.hpp"

#include <gtest/gtest.h>

using namespace shape;

struct ShapeElementLengthTestParams
{
    ShapeElement element;
    LengthDbl expected_length;
};

class ShapeElementLengthTest: public testing::TestWithParam<ShapeElementLengthTestParams> { };

TEST_P(ShapeElementLengthTest, ShapeElementLength)
{
    ShapeElementLengthTestParams test_params = GetParam();
    EXPECT_TRUE(equal(test_params.element.length(), test_params.expected_length));
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        ShapeElementLengthTest,
        testing::ValuesIn(std::vector<ShapeElementLengthTestParams>{
            {build_shape({{0, 0}, {0, 1}}, true).elements.front(), 1 },
            {build_shape({{1, 0}, {0, 0, 1}, {0, 1}}, true).elements.front(), M_PI / 2 },
            {build_shape({{1, 0}, {0, 0, -1}, {0, -1}}, true).elements.front(), M_PI / 2 },
            }));


struct ShapeElementMiddleTestParams
{
    ShapeElement circular_arc;
    Point expected_middle;
};

class ShapeElementMiddleTest: public testing::TestWithParam<ShapeElementMiddleTestParams> { };

TEST_P(ShapeElementMiddleTest, ShapeElementMiddle)
{
    ShapeElementMiddleTestParams test_params = GetParam();
    std::cout << "circular_arc" << std::endl;
    std::cout << test_params.circular_arc.to_string() << std::endl;
    std::cout << "expected_middle" << std::endl;
    std::cout << test_params.expected_middle.to_string() << std::endl;

    Point middle = test_params.circular_arc.middle();
    std::cout << "computed_middle" << std::endl;
    std::cout << middle.to_string() << std::endl;

    EXPECT_TRUE(equal(middle, test_params.expected_middle));
}
 
INSTANTIATE_TEST_SUITE_P(
        Shape,
        ShapeElementMiddleTest,
        testing::ValuesIn(std::vector<ShapeElementMiddleTestParams>{
            {
                build_shape({{1, 0}, {0, 0, 1}, {0, 1}}, true).elements.front(),
                {sqrt(2) / 2, sqrt(2) / 2}
            }, {
                build_shape({{0, 1}, {0, 0, -1}, {1, 0}}, true).elements.front(),
                {sqrt(2) / 2, sqrt(2) / 2}
            }, {
                build_shape({{1, 0}, {0, 0, 1}, {-1, 0}}, true).elements.front(),
                {0, 1}
            }, {
                build_shape({{1, 0}, {0, 0, -1}, {-1, 0}}, true).elements.front(),
                {0, -1}
            }, {
                build_shape({{-1, 0}, {0, 0, 1}, {0, 1}}, true).elements.front(),
                {sqrt(2) / 2, - sqrt(2) / 2}
            }, {
                build_shape({{-1, 0}, {0, 0, -1}, {0, 1}}, true).elements.front(),
                {- sqrt(2) / 2, sqrt(2) / 2}
            }
        }));


struct ShapeElementMinMaxTestParams
{
    ShapeElement element;
    LengthDbl expected_x_min;
    LengthDbl expected_y_min;
    LengthDbl expected_x_max;
    LengthDbl expected_y_max;
};

class ShapeElementMinMaxTest: public testing::TestWithParam<ShapeElementMinMaxTestParams> { };

TEST_P(ShapeElementMinMaxTest, ShapeElementMinMax)
{
    ShapeElementMinMaxTestParams test_params = GetParam();
    std::cout << "element " << test_params.element.to_string() << std::endl;
    std::cout << "expected x_min " << test_params.expected_x_min
        << " y_min " << test_params.expected_y_min
        << " x_max " << test_params.expected_x_max
        << " y_max " << test_params.expected_y_max << std::endl;
    auto mm = test_params.element.min_max();
    std::cout << "x_min " << mm.first.x
        << " y_min " << mm.first.y
        << " x_max " << mm.second.x
        << " y_max " << mm.second.y << std::endl;
    EXPECT_TRUE(equal(mm.first.x, test_params.expected_x_min));
    EXPECT_TRUE(equal(mm.second.x, test_params.expected_x_max));
    EXPECT_TRUE(equal(mm.first.y, test_params.expected_y_min));
    EXPECT_TRUE(equal(mm.second.y, test_params.expected_y_max));
}

INSTANTIATE_TEST_SUITE_P(
        ,
        ShapeElementMinMaxTest,
        testing::ValuesIn(std::vector<ShapeElementMinMaxTestParams>{
            {build_shape({{0, 1}, {2, 3}}, true).elements.front(), 0, 1, 2, 3 },
            {build_shape({{1, 0}, {0, 0, 1}, {-1, 0}}, true).elements.front(), -1, 0, 1, 1 },
            {build_shape({{1, 0}, {0, 0, -1}, {-1, 0}}, true).elements.front(), -1, -1, 1, 0 },

            {build_shape({{0, 1}, {0, 0, 1}, {1, 0}}, true).elements.front(), -1, -1, 1, 1 },
            {build_shape({{-1, 0}, {0, 0, 1}, {0, 1}}, true).elements.front(), -1, -1, 1, 1 },
            {build_shape({{0, -1}, {0, 0, 1}, {-1, 0}}, true).elements.front(), -1, -1, 1, 1 },
            {build_shape({{1, 0}, {0, 0, 1}, {0, -1}}, true).elements.front(), -1, -1, 1, 1 },
            }));


struct RemoveRedundantVerticesTestParams
{
    Shape shape;
    Shape expected_shape;
};

class RemoveRedundantVerticesTest: public testing::TestWithParam<RemoveRedundantVerticesTestParams> { };

TEST_P(RemoveRedundantVerticesTest, RemoveRedundantVertices)
{
    RemoveRedundantVerticesTestParams test_params = GetParam();
    Shape cleaned_shape = remove_redundant_vertices(test_params.shape).second;
    std::cout << cleaned_shape.to_string(0) << std::endl;
    EXPECT_EQ(test_params.expected_shape, cleaned_shape);
}

INSTANTIATE_TEST_SUITE_P(
        ,
        RemoveRedundantVerticesTest,
        testing::ValuesIn(std::vector<RemoveRedundantVerticesTestParams>{
            {
                build_shape({{0, 0}, {0, 0}, {100, 0}, {100, 100}}),
                build_shape({{0, 0}, {100, 0}, {100, 100}})
            }}));


struct ShapeContainsTestParams
{
    Shape shape;
    Point point;
    bool strict;
    bool expected_contained;
};

class ShapeContainsTest: public testing::TestWithParam<ShapeContainsTestParams> { };

TEST_P(ShapeContainsTest, ShapeContains)
{
    ShapeContainsTestParams test_params = GetParam();
    std::cout << "shape" << std::endl;
    std::cout << test_params.shape.to_string(0) << std::endl;
    std::cout << "point " << test_params.point.to_string() << std::endl;
    std::cout << "strict " << test_params.strict << std::endl;
    std::cout << "expected_contained " << test_params.expected_contained << std::endl;

    bool contained = test_params.shape.contains(
            test_params.point,
            test_params.strict);
    std::cout << "contained " << contained << std::endl;

    ASSERT_EQ(contained, test_params.expected_contained);
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        ShapeContainsTest,
        testing::ValuesIn(std::vector<ShapeContainsTestParams>{
            {  // Point oustide of polygon
                build_shape({{0, 0}, {1, 0}, {1, 1}, {0, 1}}),
                {2, 2},
                false,
                false,
            }, {  // Point inside polygon
                build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}}),
                {1, 1},
                false,
                true,
            }, {  // Point on polygon
                build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}}),
                {2, 2},
                false,
                true,
            }, {  // Point on polygon (strict)
                build_shape({{0, 0}, {2, 0}, {2, 2}, {0, 2}}),
                {2, 2},
                true,
                false,
            //}, {  // Point inside circle
            //    build_shape({{0, 2}, {0, 0, 1}, {0, 2}}),
            //    {1, 0},
            //    false,
            //    true,
            }, {  // Point outside of shape
                build_shape({{0, 2}, {0, 0, 1}, {0, -2}}),
                {0, 3},
                false,
                false,
            }, {  // Point outside of shape
                build_shape({{1, 0}, {0, 1}, {-1, 0}, {0, -1}}),
                {-1, -1},
                false,
                false,
            }, {  // Point outside of shape
                build_shape({{1, 0}, {1, 1, -1},
                             {0, 1}, {-1, 1, -1},
                             {-1, 0}, {-1, -1, -1},
                             {0, -1}, {1, -1, -1},
                             {1, 0}}),
                {-1, -1},
                false,
                false,
            }, {  // Point inside shape
                build_shape({{0, 2}, {0, 0, 1}, {0, -2}}),
                {0, 1},
                false,
                true,
            }, {  // Point inside shape
                build_shape({{1, 0}, {0, 1}, {-1, 0}, {0, -1}}),
                {0, 0},
                false,
                true,
            }, {  // Point inside shape
                build_shape({{1, 0}, {1, 1, -1},
                             {0, 1}, {-1, 1, -1},
                             {-1, 0}, {-1, -1, -1},
                             {0, -1}, {1, -1, -1},
                             {1, 0}}),
                {0, 0},
                false,
                true,
            }, {  // Point on shape
                build_shape({{0, 2}, {0, 0, 1}, {0, -2}}),
                {0, 2},
                false,
                true,
            }, {  // Point on shape (strict)
                build_shape({{0, 2}, {0, 0, 1}, {0, -2}}),
                {0, 2},
                true,
                false,
            }}));


struct ApproximateCircularArcByLineSegmentsTestParams
{
    ShapeElement circular_arc;
    LengthDbl number_of_line_segments;
    bool outer;
    std::vector<ShapeElement> expected_line_segments;
};

class ApproximateCircularArcByLineSegmentsTest: public testing::TestWithParam<ApproximateCircularArcByLineSegmentsTestParams> { };

TEST_P(ApproximateCircularArcByLineSegmentsTest, ApproximateCircularArcByLineSegments)
{
    ApproximateCircularArcByLineSegmentsTestParams test_params = GetParam();
    std::cout << "circular_arc" << std::endl;
    std::cout << test_params.circular_arc.to_string() << std::endl;
    std::cout << "expected_line_segments" << std::endl;
    for (const ShapeElement& line_segment: test_params.expected_line_segments)
        std::cout << line_segment.to_string() << std::endl;
    LengthDbl segment_length = test_params.circular_arc.length() / test_params.number_of_line_segments;
    std::vector<ShapeElement> line_segments = approximate_circular_arc_by_line_segments(
            test_params.circular_arc,
            segment_length,
            test_params.outer);
    std::cout << "line_segments" << std::endl;
    for (const ShapeElement& line_segment: line_segments)
        std::cout << line_segment.to_string() << std::endl;

    ASSERT_EQ(line_segments.size(), test_params.number_of_line_segments);
    for (ElementPos pos = 0; pos < test_params.number_of_line_segments; ++pos) {
        //std::cout << std::setprecision (15) << line_segments[pos].start.x << std::endl;
        EXPECT_TRUE(equal(line_segments[pos], test_params.expected_line_segments[pos]));
    }
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        ApproximateCircularArcByLineSegmentsTest,
        testing::ValuesIn(std::vector<ApproximateCircularArcByLineSegmentsTestParams>{
            {
                build_shape({{1, 0}, {0, 0, 1}, {0, 1}}, true).elements.front(),
                1,
                false,
                build_shape({{1, 0}, {0, 1}}, true).elements
            }, {
                build_shape({{1, 0}, {1, 1, -1}, {0, 1}}, true).elements.front(),
                1,
                true,
                build_shape({{1, 0}, {0, 1}}, true).elements
            }, {
                build_shape({{1, 0}, {0, 0, 1}, {0, 1}}, true).elements.front(),
                2,
                true,
                build_shape({{1, 0}, {1, 1}, {0, 1}}, true).elements
            }, {
                build_shape({{1, 0}, {1, 1, -1}, {0, 1}}, true).elements.front(),
                2,
                false,
                build_shape({{1, 0}, {0, 0}, {0, 1}}, true).elements
            }, {
                build_shape({{1, 0}, {0, 0, 1}, {0, 1}}, true).elements.front(),
                3,
                true,
                build_shape({{1, 0}, {1, 0.414213562373095}, {0.414213562373095, 1}, {0, 1}}, true).elements
            }, {
                build_shape({{1, 0}, {1, 1, -1}, {0, 1}}, true).elements.front(),
                3,
                false,
                build_shape({{1, 0}, {0.585786437626905, 0}, {0, 0.585786437626905}, {0, 1}}, true).elements
            }}));
