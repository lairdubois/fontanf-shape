#include "shape/trapezoidation.hpp"

#include <gtest/gtest.h>

using namespace shape;

TEST(PolygonTrapezoidation, Triangle1)
{
    ShapeWithHoles shape = {build_shape({{0, 0}, {3, 0}, {1, 3}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    //for (const GeneralizedTrapezoid& trapezoid: trapezoids)
    //    std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 1);
    GeneralizedTrapezoid trapezoid_1(0, 3, 0, 3, 1, 1);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
}

TEST(PolygonTrapezoidation, Triangle2)
{
    ShapeWithHoles shape = {build_shape({{2, 0}, {3, 3}, {0, 3}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    //for (const GeneralizedTrapezoid& trapezoid: trapezoids)
    //    std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 1);
    GeneralizedTrapezoid trapezoid_1(0, 3, 2, 2, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
}

TEST(PolygonTrapezoidation, Square)
{
    ShapeWithHoles shape = {build_shape({{0, 0}, {1, 0}, {1, 1}, {0, 1}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);

    EXPECT_EQ(trapezoids.size(), 1);
    GeneralizedTrapezoid trapezoid_1(0, 1, 0, 1, 0, 1);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
}

TEST(PolygonTrapezoidation, Trapezoid1)
{
    ShapeWithHoles shape = {build_shape({{0, 0}, {3, 0}, {2, 3}, {1, 3}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);

    EXPECT_EQ(trapezoids.size(), 1);
    GeneralizedTrapezoid trapezoid_1(0, 3, 0, 3, 1, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
}

TEST(PolygonTrapezoidation, Trapezoid2)
{
    ShapeWithHoles shape = {build_shape({{1, 0}, {2, 0}, {3, 3}, {0, 3}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);

    EXPECT_EQ(trapezoids.size(), 1);
    GeneralizedTrapezoid trapezoid_1(0, 3, 1, 2, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
}

TEST(PolygonTrapezoidation, Triangle3)
{
    ShapeWithHoles shape = {build_shape({{4, 0}, {1, 3}, {0, 1}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    //for (const GeneralizedTrapezoid& trapezoid: trapezoids)
    //    std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 2);
    GeneralizedTrapezoid trapezoid_1(1, 3, 0, 3, 1, 1);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(0, 1, 4, 4, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
}

TEST(PolygonTrapezoidation, Trapezoid3)
{
    ShapeWithHoles shape = {build_shape({{5, 0}, {2, 3}, {1, 3}, {0, 1}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    for (const GeneralizedTrapezoid& trapezoid: trapezoids)
        std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 2);
    GeneralizedTrapezoid trapezoid_1(1, 3, 0, 4, 1, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(0, 1, 5, 5, 0, 4);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
}

TEST(PolygonTrapezoidation, DoubleTrapezoid1)
{
    ShapeWithHoles shape = {build_shape({{0, 0}, {4, 0}, {2, 2}, {4, 4}, {0, 4}, {1, 2}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    //for (const GeneralizedTrapezoid& trapezoid: trapezoids)
    //    std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 2);
    GeneralizedTrapezoid trapezoid_1(2, 4, 1, 2, 0, 4);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(0, 2, 0, 4, 1, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
}

TEST(PolygonTrapezoidation, DoubleTrapezoid2)
{
    ShapeWithHoles shape = {build_shape({{1, 0}, {2, 0}, {4, 2}, {2, 4}, {1, 4}, {0, 2}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    //for (const GeneralizedTrapezoid& trapezoid: trapezoids)
    //    std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 2);
    GeneralizedTrapezoid trapezoid_1(2, 4, 0, 4, 1, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(0, 2, 1, 2, 0, 4);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
}

TEST(PolygonTrapezoidation, ReversedH)
{
    ShapeWithHoles shape = {build_shape({
            {0, 0}, {3, 0}, {3, 1}, {2, 1}, {2, 2}, {3, 2},
            {3, 3}, {0, 3}, {0, 2}, {1, 2}, {1, 1}, {0, 1}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    //for (const GeneralizedTrapezoid& trapezoid: trapezoids)
    //    std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 3);
    GeneralizedTrapezoid trapezoid_1(2, 3, 0, 3, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(1, 2, 1, 2, 1, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
    GeneralizedTrapezoid trapezoid_3(0, 1, 0, 3, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_3), trapezoids.end());
}

TEST(PolygonTrapezoidation, Cross)
{
    ShapeWithHoles shape = {build_shape({
            {1, 0}, {2, 0}, {2, 1}, {3, 1}, {3, 2}, {2, 2},
            {2, 3}, {1, 3}, {1, 2}, {0, 2}, {0, 1}, {1, 1}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    //for (const GeneralizedTrapezoid& trapezoid: trapezoids)
    //    std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 3);
    GeneralizedTrapezoid trapezoid_1(2, 3, 1, 2, 1, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(1, 2, 0, 3, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
    GeneralizedTrapezoid trapezoid_3(0, 1, 1, 2, 1, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_3), trapezoids.end());
}

TEST(PolygonTrapezoidation, U)
{
    ShapeWithHoles shape = {build_shape({
            {0, 0}, {3, 0}, {3, 3}, {2, 3},
            {2, 1}, {1, 1}, {1, 3}, {0, 3}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    //for (const GeneralizedTrapezoid& trapezoid: trapezoids)
    //    std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 3);
    GeneralizedTrapezoid trapezoid_1(1, 3, 0, 1, 0, 1);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(1, 3, 2, 3, 2, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
    GeneralizedTrapezoid trapezoid_3(0, 1, 0, 3, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_3), trapezoids.end());
}

TEST(PolygonTrapezoidation, W)
{
    ShapeWithHoles shape = {build_shape({
            {0, 0}, {5, 0}, {5, 3}, {4, 3},
            {4, 1}, {3, 1}, {3, 2}, {2, 2},
            {2, 1}, {1, 1}, {1, 3}, {0, 3}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    for (const GeneralizedTrapezoid& trapezoid: trapezoids)
        std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 4);
    GeneralizedTrapezoid trapezoid_1(1, 3, 0, 1, 0, 1);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(1, 3, 4, 5, 4, 5);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
    GeneralizedTrapezoid trapezoid_3(1, 2, 2, 3, 2, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_3), trapezoids.end());
    GeneralizedTrapezoid trapezoid_4(0, 1, 0, 5, 0, 5);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_4), trapezoids.end());
}

TEST(PolygonTrapezoidation, Shape1)
{
    ShapeWithHoles shape = {build_shape({
            {185.355, 114.645},
            {150.0, 79.289},
            {79.289, 150.0},
            {114.645, 185.355},
            {0.0, 300.0},
            {0.0, 0.0},
            {300.0, 0.0}})};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    for (const GeneralizedTrapezoid& trapezoid: trapezoids)
        std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 5);
    GeneralizedTrapezoid trapezoid_1(185.355, 300, 0, 114.645, 0, 0);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(150, 185.355, 0, 79.289, 0, 114.645);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
    GeneralizedTrapezoid trapezoid_3(79.289, 150, 0, 150, 0, 79.289);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_3), trapezoids.end());
    GeneralizedTrapezoid trapezoid_4(79.289, 114.645, 150, 220.711, 185.355, 185.355);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_4), trapezoids.end());
    GeneralizedTrapezoid trapezoid_5(0, 79.289, 0, 300, 0, 220.711);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_5), trapezoids.end());
}

TEST(PolygonTrapezoidation, SquareRing)
{
    ShapeWithHoles shape = {build_shape({
            {0, 0},
            {3, 0},
            {3, 3},
            {0, 3}}),
                   {build_shape({
            {1, 1},
            {2, 1},
            {2, 2},
            {1, 2}})}};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    for (const GeneralizedTrapezoid& trapezoid: trapezoids)
        std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 4);
    GeneralizedTrapezoid trapezoid_1(2, 3, 0, 3, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(1, 2, 0, 1, 0, 1);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
    GeneralizedTrapezoid trapezoid_3(1, 2, 2, 3, 2, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_3), trapezoids.end());
    GeneralizedTrapezoid trapezoid_4(0, 1, 0, 3, 0, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_4), trapezoids.end());
}

TEST(PolygonTrapezoidation, DiamondHole)
{
    ShapeWithHoles shape = {build_shape({
            {1, 0},
            {3, 0},
            {4, 1},
            {4, 3},
            {3, 4},
            {1, 4},
            {0, 3},
            {0, 1}}),
          {build_shape({
            {2, 1},
            {3, 2},
            {2, 3},
            {1, 2}})}};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    for (const GeneralizedTrapezoid& trapezoid: trapezoids)
        std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 6);
    GeneralizedTrapezoid trapezoid_1(3, 4, 0, 4, 1, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(2, 3, 0, 1, 0, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
    GeneralizedTrapezoid trapezoid_3(2, 3, 3, 4, 2, 4);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_3), trapezoids.end());
    GeneralizedTrapezoid trapezoid_4(1, 2, 0, 2, 0, 1);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_4), trapezoids.end());
    GeneralizedTrapezoid trapezoid_5(1, 2, 2, 4, 3, 4);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_5), trapezoids.end());
    GeneralizedTrapezoid trapezoid_6(0, 1, 1, 3, 0, 4);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_6), trapezoids.end());
}

TEST(PolygonTrapezoidation, ButterflyHole)
{
    ShapeWithHoles shape = {build_shape({
            {1, 0},
            {3, 0},
            {4, 1},
            {4, 3},
            {3, 4},
            {1, 4},
            {0, 3},
            {0, 1}}),
                   {build_shape({
            {1, 1},
            {2, 1.5},
            {3, 1},
            {3, 3},
            {2, 2.5},
            {1, 3}})}};
    std::vector<GeneralizedTrapezoid> trapezoids = trapezoidation(shape);
    for (const GeneralizedTrapezoid& trapezoid: trapezoids)
        std::cout << trapezoid << std::endl;

    EXPECT_EQ(trapezoids.size(), 6);
    GeneralizedTrapezoid trapezoid_1(3, 4, 0, 4, 1, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_1), trapezoids.end());
    GeneralizedTrapezoid trapezoid_2(2.5, 3, 2, 2, 1, 3);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_2), trapezoids.end());
    GeneralizedTrapezoid trapezoid_3(1, 3, 0, 1, 0, 1);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_3), trapezoids.end());
    GeneralizedTrapezoid trapezoid_4(1, 1.5, 1, 3, 2, 2);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_4), trapezoids.end());
    GeneralizedTrapezoid trapezoid_5(1, 3, 3, 4, 3, 4);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_5), trapezoids.end());
    GeneralizedTrapezoid trapezoid_6(0, 1, 1, 3, 0, 4);
    EXPECT_NE(std::find(trapezoids.begin(), trapezoids.end(), trapezoid_6), trapezoids.end());
}
