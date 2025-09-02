#include "shape/clean.hpp"

#include "shape/equalize.hpp"
#include "shape/boolean_operations.hpp"
#include "shape/element_intersections.hpp"

using namespace shape;

std::pair<bool, Shape> shape::remove_redundant_vertices(
        const Shape& shape)
{
    //std::cout << "remove_redundant_vertices " << shape.to_string(2) << std::endl;
    if (!shape.check()) {
        throw std::invalid_argument(
                "shape::remove_redundant_vertices: invalid input shape.");
    }

    if (shape.elements.size() <= 3)
        return {false, shape};

    ElementPos number_of_elements_removed = 0;
    Shape shape_new;
    shape_new.is_path = shape.is_path;

    ElementPos element_prev_pos = shape.elements.size() - 1;
    for (ElementPos element_cur_pos = 0;
            element_cur_pos < (ElementPos)shape.elements.size();
            ++element_cur_pos) {

        //std::cout << "element_cur_pos " << element_cur_pos << std::endl;
        ElementPos element_next_pos = element_cur_pos + 1;
        const ShapeElement& element_prev = shape.elements[element_prev_pos];
        const ShapeElement& element = shape.elements[element_cur_pos];
        const ShapeElement& element_next = (element_next_pos < shape.elements.size())?
            shape.elements[element_next_pos]:
            shape_new.elements.front();
        bool useless = false;
        if (equal(element.start.x, element.end.x)
                && equal(element.start.y, element.end.y)) {
            useless = true;
        }
        if (!useless || shape.elements.size() - number_of_elements_removed <= 3) {
            if (!shape_new.elements.empty())
                shape_new.elements.back().end = element.start;
            shape_new.elements.push_back(element);
            element_prev_pos = element_cur_pos;
        } else {
            number_of_elements_removed++;
        }
    }
    shape_new.elements.back().end = shape_new.elements.front().start;

    if (!shape_new.check()) {
        throw std::invalid_argument(
                "shape::remove_redundant_vertices: invalid output shape.");
    }
    return {(number_of_elements_removed > 0), shape_new};
}

std::pair<bool, ShapeWithHoles> shape::remove_redundant_vertices(
        const ShapeWithHoles& shape)
{
    ShapeWithHoles res;
    bool b;

    auto p = remove_redundant_vertices(shape.shape);
    b |= p.first;
    res.shape = p.second;

    res.holes = std::vector<Shape>(shape.holes.size());
    for (ShapePos hole_pos = 0;
            hole_pos < (ShapePos)shape.holes.size();
            ++hole_pos) {
        const Shape& hole = shape.holes[hole_pos];
        auto p = remove_redundant_vertices(hole);
        b |= p.first;
        res.holes[hole_pos] = p.second;
    }

    return {b, res};
}

std::pair<bool, Shape> shape::remove_aligned_vertices(
        const Shape& shape)
{
    //std::cout << "remove_aligned_vertices " << shape.to_string(2) << std::endl;

    if (shape.elements.size() <= 3)
        return {false, shape};

    ElementPos number_of_elements_removed = 0;
    Shape shape_new;
    shape_new.is_path = shape.is_path;

    ElementPos element_prev_pos = shape.elements.size() - 1;
    for (ElementPos element_cur_pos = 0;
            element_cur_pos < (ElementPos)shape.elements.size();
            ++element_cur_pos) {

        //std::cout << "element_cur_pos " << element_cur_pos << std::endl;
        ElementPos element_next_pos = element_cur_pos + 1;
        const ShapeElement& element_prev = shape.elements[element_prev_pos];
        const ShapeElement& element = shape.elements[element_cur_pos];
        const ShapeElement& element_next = (element_next_pos < shape.elements.size())?
            shape.elements[element_next_pos]:
            shape_new.elements.front();
        bool useless = false;
        if (element.type == ShapeElementType::LineSegment
                && element_prev.type == ShapeElementType::LineSegment) {
            double v = compute_area(element_prev.start, element.start, element_next.start);
            //std::cout << "element_prev  " << element_prev_pos << " " << element_prev.to_string() << std::endl;
            //std::cout << "element       " << element_cur_pos << " " << element.to_string() << std::endl;
            //std::cout << "element_next  " << element_next_pos << " " << element_next.to_string() << std::endl;
            //std::cout << "v " << v << std::endl;
            if (element_prev.start.x == element.start.x
                    && element.start.x == element_next.start.x) {
                //std::cout << "useless " << element.to_string() << std::endl;
                useless = true;
            }
            if (element_prev.start.y == element.start.y
                    && element.start.y == element_next.start.y) {
                //std::cout << "useless " << element.to_string() << std::endl;
                useless = true;
            }
        }
        if (!useless || shape.elements.size() - number_of_elements_removed <= 3) {
            if (!shape_new.elements.empty())
                shape_new.elements.back().end = element.start;
            shape_new.elements.push_back(element);
            element_prev_pos = element_cur_pos;
        } else {
            number_of_elements_removed++;
        }
    }
    shape_new.elements.back().end = shape_new.elements.front().start;

    return {(number_of_elements_removed > 0), shape_new};
}

std::pair<bool, ShapeWithHoles> shape::remove_aligned_vertices(
        const ShapeWithHoles& shape)
{
    ShapeWithHoles res;
    bool b;

    auto p = remove_aligned_vertices(shape.shape);
    b |= p.first;
    res.shape = p.second;

    res.holes = std::vector<Shape>(shape.holes.size());
    for (ShapePos hole_pos = 0;
            hole_pos < (ShapePos)shape.holes.size();
            ++hole_pos) {
        const Shape& hole = shape.holes[hole_pos];
        auto p = remove_aligned_vertices(hole);
        b |= p.first;
        res.holes[hole_pos] = p.second;
    }

    return {b, res};
}

namespace
{

struct CleanExtremeSlopesOutput
{
    Shape shape;

    std::vector<ShapeWithHoles> union_input;
};

CleanExtremeSlopesOutput clean_extreme_slopes_outer_rec(
        const Shape& shape)
{
    CleanExtremeSlopesOutput output;

    if (shape.elements.size() == 1) {
        output.shape = shape;
        return output;
    }

    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)shape.elements.size();
            ++element_pos) {
        ShapeElement element = shape.elements[element_pos];
        const ShapeElement& element_prev = (!output.shape.elements.empty())?
            output.shape.elements.back():
            shape.elements.back();
        const ShapeElement& element_next = (element_pos < shape.elements.size() - 1)?
            shape.elements[element_pos + 1]:
            output.shape.elements.front();
        if (element_pos > 0)
            element.start = output.shape.elements.back().end;
        if (element_pos == shape.elements.size() - 1)
            element.end = output.shape.elements.front().start;

        double slope
            = (element.end.y - element.start.y)
            / (element.end.x - element.start.x);
        //std::cout << "element " << element.to_string() << " slope " << slope << std::endl;
        //std::cout << "element_prev " << element_prev.to_string() << std::endl;
        if (element.type != ShapeElementType::LineSegment) {
        } else if (element.start.x != element.end.x && std::abs(slope) > 1e2) {
            if (equal(element.start.x, element.end.x)) {
                throw std::logic_error(
                        "shape::clean_extreme_slopes_rec: x");
            } else if (slope > 1e2) {
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                element_new.end.x = element.end.x;
                element_new.end.y = element.start.y;
                element.start = element_new.end;
                output.shape.elements.push_back(element_new);
                output.union_input.push_back({build_triangle(
                            element_new.start,
                            element.start,
                            element.end)});
            } else if (slope < -1e2) {
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                element_new.end.x = element.start.x;
                element_new.end.y = element.end.y;
                element.start = element_new.end;
                output.shape.elements.push_back(element_new);
                output.union_input.push_back({build_triangle(
                            element_new.start,
                            element.start,
                            element.end)});
            }
        } else if (element.start.y != element.end.y && std::abs(slope) < 1e-2) {
            if (equal(element.start.y, element.end.y)) {
                throw std::logic_error(
                        "shape::clean_extreme_slopes_rec: y");
            } else if (slope > 0 && slope < 1e-2) {
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                element_new.end.x = element.end.x;
                element_new.end.y = element.start.y;
                element.start = element_new.end;
                output.shape.elements.push_back(element_new);
                output.union_input.push_back({build_triangle(
                            element_new.start,
                            element.start,
                            element.end)});
            } else if (slope < 0 && slope > -1e-2) {
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                element_new.end.x = element.start.x;
                element_new.end.y = element.end.y;
                element.start = element_new.end;
                output.shape.elements.push_back(element_new);
                output.union_input.push_back({build_triangle(
                            element_new.start,
                            element.start,
                            element.end)});
            }
        }
        if (!output.shape.elements.empty())
            output.shape.elements.back().end = element.start;
        output.shape.elements.push_back(element);
    }
    output.shape.elements.front().start = output.shape.elements.back().end;
    return output;
}

CleanExtremeSlopesOutput clean_extreme_slopes_inner_rec(
        const Shape& shape)
{
    CleanExtremeSlopesOutput output;

    if (shape.elements.size() == 1) {
        output.shape = shape;
        return output;
    }

    for (ElementPos element_pos = 0;
            element_pos < (ElementPos)shape.elements.size();
            ++element_pos) {
        ShapeElement element = shape.elements[element_pos];
        const ShapeElement& element_prev = (!output.shape.elements.empty())?
            output.shape.elements.back():
            shape.elements.back();
        const ShapeElement& element_next = (element_pos < shape.elements.size() - 1)?
            shape.elements[element_pos + 1]:
            output.shape.elements.front();
        if (element_pos > 0)
            element.start = output.shape.elements.back().end;
        if (element_pos == shape.elements.size() - 1)
            element.end = output.shape.elements.front().start;

        double slope
            = (element.end.y - element.start.y)
            / (element.end.x - element.start.x);
        //std::cout << "element " << element.to_string() << " slope " << slope << std::endl;
        //std::cout << "element_prev " << element_prev.to_string() << std::endl;
        if (element.type != ShapeElementType::LineSegment) {
        } else if (element.start.x != element.end.x && std::abs(slope) > 1e2) {
            if (equal(element.start.x, element.end.x)) {
                throw std::logic_error(
                        "shape::clean_extreme_slopes_rec: x");
            } else if (slope > 1e2) {
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                element_new.end.x = element.start.x;
                element_new.end.y = element.end.y;
                element.start = element_new.end;
                output.shape.elements.push_back(element_new);
                output.union_input.push_back({build_triangle(
                            element_new.start,
                            element.end,
                            element.start)});
            } else if (slope < -1e2) {
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                element_new.end.x = element.end.x;
                element_new.end.y = element.start.y;
                element.start = element_new.end;
                output.shape.elements.push_back(element_new);
                output.union_input.push_back({build_triangle(
                            element_new.start,
                            element.end,
                            element.start)});
            }
        } else if (element.start.y != element.end.y && std::abs(slope) < 1e-2) {
            if (equal(element.start.y, element.end.y)) {
                throw std::logic_error(
                        "shape::clean_extreme_slopes_rec: y");
            } else if (slope > 0 && slope < 1e-2) {
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                element_new.end.x = element.start.x;
                element_new.end.y = element.end.y;
                element.start = element_new.end;
                output.shape.elements.push_back(element_new);
                output.union_input.push_back({build_triangle(
                            element_new.start,
                            element.end,
                            element.start)});
            } else if (slope < 0 && slope > -1e-2) {
                ShapeElement element_new;
                element_new.type = ShapeElementType::LineSegment;
                element_new.start = element.start;
                element_new.end.x = element.end.x;
                element_new.end.y = element.start.y;
                element.start = element_new.end;
                output.shape.elements.push_back(element_new);
                output.union_input.push_back({build_triangle(
                            element_new.start,
                            element.end,
                            element.start)});
            }
        }
        if (!output.shape.elements.empty())
            output.shape.elements.back().end = element.start;
        output.shape.elements.push_back(element);
    }
    output.shape.elements.front().start = output.shape.elements.back().end;
    return output;
}

}

std::vector<Shape> shape::clean_extreme_slopes_inner(
        const Shape& shape)
{
    //std::cout << "clean_extreme_slopes_inner" << std::endl;
    //std::cout << "shape " << shape.to_string(0) << std::endl;
    Shape equalized_shape = equalize_shape(shape);
    std::vector<ShapeWithHoles> union_input;
    Shape shape_new = equalized_shape;
    for (int i = 0;; ++i) {
        if (i == 100) {
            throw std::runtime_error(
                    "packingsolver::irregular::process_shape_outer: "
                    "too many iterations.");
        }
        auto output = clean_extreme_slopes_inner_rec(shape_new);
        if (output.union_input.empty())
            break;
        shape_new = output.shape;
        for (const ShapeWithHoles& s: output.union_input)
            union_input.push_back(s);
    }
    std::vector<Shape> output;
    if (!intersect(shape_new)) {
        shape_new = remove_redundant_vertices(shape_new).second;
        shape_new = remove_aligned_vertices(shape_new).second;
        output.push_back(shape_new);
    } else {
        std::vector<ShapeWithHoles> difference_output
            = compute_difference({equalized_shape}, union_input);
        for (const ShapeWithHoles& difference_output_shape: difference_output)
            output.push_back(difference_output_shape.shape);
    }

    //std::cout << "output" << std::endl;
    //for (const Shape& shape: output)
    //    std::cout << shape.to_string(0) << std::endl;
    //std::cout << "clean_extreme_slopes_inner end" << std::endl;

    return output;
}

ShapeWithHoles shape::clean_extreme_slopes_outer(
        const Shape& shape)
{
    //std::cout << "clean_extreme_slopes_outer" << std::endl;
    Shape equalized_shape = equalize_shape(shape);
    std::vector<ShapeWithHoles> union_input = {{equalized_shape}};
    Shape shape_new = equalized_shape;
    for (int i = 0;; ++i) {
        if (i == 100) {
            throw std::runtime_error(
                    "packingsolver::irregular::process_shape_outer: "
                    "too many iterations.");
        }
        auto output = clean_extreme_slopes_outer_rec(shape_new);
        if (output.union_input.empty())
            break;
        shape_new = output.shape;
        for (const ShapeWithHoles& s: output.union_input)
            union_input.push_back(s);
    }
    if (!intersect(shape_new)) {
        //std::cout << "clean_extreme_slopes_outer end" << std::endl;
        shape_new = remove_redundant_vertices(shape_new).second;
        shape_new = remove_aligned_vertices(shape_new).second;
        return {shape_new};
    } else {
        std::vector<ShapeWithHoles> union_output = compute_union(union_input);
        //std::cout << "clean_extreme_slopes_outer end" << std::endl;
        return union_output.front();
    }
}
