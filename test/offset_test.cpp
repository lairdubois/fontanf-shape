#include "shape/offset.hpp"

#include "shape/writer.hpp"

#include <gtest/gtest.h>

#include "test_params.hpp"

using namespace shape;


struct InflateShapeTestParams
{
    Shape shape;
    LengthDbl offset;
    ShapeWithHoles expected_output;


    static InflateShapeTestParams from_json(
            nlohmann::basic_json<>& json_item)
    {
        InflateShapeTestParams test_params;
        test_params.shape = Shape::from_json(json_item["shape"]);
        test_params.offset = json_item["offset"];
        test_params.expected_output = ShapeWithHoles::from_json(json_item["expected_output"]);
        return test_params;
    }
};

class InflateShapeTest: public testing::TestWithParam<InflateShapeTestParams> { };

TEST_P(InflateShapeTest, InflateShape)
{
    InflateShapeTestParams test_params = GetParam();
    std::cout << "shape " << test_params.shape.to_string(0) << std::endl;
    std::cout << "offset " << test_params.offset << std::endl;
    std::cout << "expected_output " << test_params.expected_output.to_string(0) << std::endl;

    auto output = inflate(
        test_params.shape,
        test_params.offset);
    std::cout << "output " << output.to_string(0) << std::endl;

    Writer writer;
    writer.add_shape(test_params.shape);
    if (!test_params.expected_output.shape.elements.empty())
        writer.add_shape_with_holes(test_params.expected_output);
    writer.add_shape_with_holes(output);
    writer.write_json("inflate_shape_input.json");;

    EXPECT_TRUE(equal(output, test_params.expected_output));
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        InflateShapeTest,
        testing::ValuesIn(std::vector<InflateShapeTestParams>{
            {
                build_path({{0, 0}, {0, 10}}),
                1.0,
                {
                    build_shape({{-1, 0}, {0, 0, 1}, {1, 0}, {1, 10}, {0, 10, 1}, {-1, 10}}),
                },
            },
            //SimplificationTestParams::read_json(
            //        (fs::path("data") / "tests" / "simplification" / "2.json").string()),
            }));


struct InflateShapeWithHolesTestParams: TestParams<InflateShapeWithHolesTestParams>
{
    ShapeWithHoles shape;
    LengthDbl offset = 0;
    ShapeWithHoles expected_output;


    static InflateShapeWithHolesTestParams from_json(
            nlohmann::basic_json<>& json_item)
    {
        InflateShapeWithHolesTestParams test_params = TestParams::from_json(json_item);
        test_params.shape = ShapeWithHoles::from_json(json_item["shape"]);
        test_params.offset = json_item["offset"];
        test_params.expected_output = ShapeWithHoles::from_json(json_item["expected_output"]);
        return test_params;
    }
};

class InflateShapeWithHolesTest: public testing::TestWithParam<InflateShapeWithHolesTestParams> { };

TEST_P(InflateShapeWithHolesTest, InflateShapeWithHoles)
{
    InflateShapeWithHolesTestParams test_params = GetParam();
    std::cout << "Testing " << test_params.name << " (" << test_params.description << ")" << "..." << std::endl;
    std::cout << "shape " << test_params.shape.to_string(0) << std::endl;
    std::cout << "offset " << test_params.offset << std::endl;
    std::cout << "expected_output " << test_params.expected_output.to_string(0) << std::endl;
    //Writer().add_shape_with_holes(test_params.shape).add_shape_with_holes(test_params.expected_output).write_json("inflate_input.json");

    auto output = inflate(
        test_params.shape,
        test_params.offset);
    std::cout << "output " << output.to_string(0) << std::endl;

    if (test_params.write_json || test_params.write_svg) {
        std::string base_filename = "inflate_" + fs::path(test_params.name).filename().replace_extension("").string();

        if (test_params.write_json) {
            Writer().add_shape_with_holes(test_params.shape).write_json(base_filename + "_shapes.json");
            Writer().add_shape_with_holes(test_params.expected_output).write_json(base_filename + "_expected_output.json");
            Writer().add_shape_with_holes(output).write_json(base_filename + "_output.json");
        }
        if (test_params.write_svg) {
            Writer().add_shape_with_holes(test_params.shape).write_svg(base_filename + "_shapes.svg");
            Writer().add_shape_with_holes(test_params.expected_output).write_svg(base_filename + "_expected_output.svg");
            Writer().add_shape_with_holes(output).write_svg(base_filename + "_output.svg");
        }
    }

    EXPECT_TRUE(equal(output, test_params.expected_output));
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        InflateShapeWithHolesTest,
        testing::ValuesIn(InflateShapeWithHolesTestParams::read_dir((fs::path("data") / "tests" / "offset" / "inflate").string())));


struct DeflateTestParams: TestParams<DeflateTestParams>
{
    Shape shape;
    LengthDbl offset = 0;
    std::vector<Shape> expected_output;


    static DeflateTestParams from_json(
            nlohmann::basic_json<>& json_item)
    {
        DeflateTestParams test_params = TestParams::from_json(json_item);
        test_params.shape = Shape::from_json(json_item["shape"]);
        test_params.offset = json_item["offset"];
        for (auto& json_shape: json_item["expected_output"].items())
            test_params.expected_output.emplace_back(Shape::from_json(json_shape.value()));
        return test_params;
    }
};

class DeflateTest: public testing::TestWithParam<DeflateTestParams> { };

TEST_P(DeflateTest, Deflate)
{
    DeflateTestParams test_params = GetParam();
    std::cout << "Testing " << test_params.name << " (" << test_params.description << ")" << "..." << std::endl;
    std::cout << "hole " << test_params.shape.to_string(0) << std::endl;
    std::cout << "offset " << test_params.offset << std::endl;
    std::cout << "expected_output" << std::endl;
    for (const Shape& hole: test_params.expected_output)
        std::cout << "- " << hole.to_string(2) << std::endl;

    auto output = deflate(
        test_params.shape,
        test_params.offset);
    std::cout << "output" << std::endl;
    for (const Shape& hole: output)
        std::cout << "- " << hole.to_string(2) << std::endl;

    if (test_params.write_json || test_params.write_svg) {
        std::string base_filename = "deflate_" + fs::path(test_params.name).filename().replace_extension("").string();

        if (test_params.write_json) {
            test_params.shape.write_json(base_filename + "_shape.json");
            for (const auto& shape: test_params.expected_output)
                shape.write_json(base_filename + "_expected_output.json");
            for (const auto& shape: output)
                shape.write_json(base_filename + "_output.json");
        }
        if (test_params.write_svg) {
            test_params.shape.write_svg(base_filename + "_shape.svg");
            for (const auto& shape: test_params.expected_output)
                shape.write_svg(base_filename + "_expected_output.svg");
            for (const auto& shape: output)
                shape.write_svg(base_filename + "_output.svg");
        }

        if (test_params.write_json) {
            Writer().add_shape(test_params.shape).write_json(base_filename + "_shapes.json");
            Writer().add_shapes(test_params.expected_output).write_json(base_filename + "_expected_output.json");
            Writer().add_shapes(output).write_json(base_filename + "_output.json");
        }
        if (test_params.write_svg) {
            Writer().add_shape(test_params.shape).write_svg(base_filename + "_shapes.svg");
            Writer().add_shapes(test_params.expected_output).write_svg(base_filename + "_expected_output.svg");
            Writer().add_shapes(output).write_svg(base_filename + "_output.svg");
        }
    }

    ASSERT_EQ(output.size(), test_params.expected_output.size());
    for (const Shape& expected_hole: test_params.expected_output) {
        EXPECT_NE(std::find(
                      output.begin(),
                      output.end(),
                      expected_hole),
                  output.end());
    }
}

INSTANTIATE_TEST_SUITE_P(
        Shape,
        DeflateTest,
        testing::ValuesIn(DeflateTestParams::read_dir((fs::path("data") / "tests" / "offset" / "deflate").string())));
