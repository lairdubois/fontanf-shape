#include "shape/intersection_tree.hpp"

#include "shape/element_intersections.hpp"

#include "optimizationtools/containers/indexed_set.hpp"

//#include <iostream>

using namespace shape;

IntersectionTree::IntersectionTree(
        const std::vector<Shape>& shapes):
    shapes_(&shapes)
{
    std::vector<std::pair<Point, Point>> shapes_min_max(shapes.size());
    for (ShapePos shape_id = 0;
            shape_id < (ShapePos)shapes.size();
            ++shape_id) {
        const Shape& shape = shapes[shape_id];
        shapes_min_max[shape_id] = shape.compute_min_max();
    }

    Node root;

    for (ShapePos shape_id = 0;
            shape_id < (ShapePos)shapes.size();
            ++shape_id) {
        const Shape& shape = shapes[shape_id];
        auto mm = shape.compute_min_max();
        root.l = (std::min)(root.l, mm.first.x);
        root.r = (std::max)(root.r, mm.first.x);
        root.b = (std::min)(root.b, mm.first.y);
        root.t = (std::max)(root.t, mm.first.y);
    }
    tree_.push_back(root);

    std::vector<StackElement> stack;
    StackElement stack_initial_element;
    stack_initial_element.node_id = 0;
    stack_initial_element.shape_ids = std::vector<ShapePos>(shapes.size());
    std::iota(stack_initial_element.shape_ids.begin(), stack_initial_element.shape_ids.end(), 0);
    stack.push_back(stack_initial_element);

    while (!stack.empty()) {
        StackElement stack_element = stack.back();
        stack.pop_back();

        NodeId node_id = stack_element.node_id;
        Node& node = tree_[node_id];
        //std::cout << "node_id " << node_id
        //    << " size " << stack_element.shape_ids.size()
        //    << " l " << node.l
        //    << " r " << node.r
        //    << " b " << node.b
        //    << " t " << node.t
        //    << std::endl;

        // Compute x_middle / y_middle
        std::vector<LengthDbl> xs =  {
            2 * node.l / 3 + node.r / 3,
            node.l / 2 + node.r / 2,
            node.l / 3 + 2 * node.r / 3};
        std::vector<LengthDbl> ys =  {
            2 * node.b / 3 + node.t / 3,
            node.b / 2 + node.t / 2,
            node.b / 3 + 2 * node.t / 3};

        // Compute left/right/bottom/top shapes.
        std::vector<std::vector<ShapePos>> left_shape_ids(xs.size());
        std::vector<std::vector<ShapePos>> right_shape_ids(xs.size());
        std::vector<std::vector<ShapePos>> bottom_shape_ids(ys.size());
        std::vector<std::vector<ShapePos>> top_shape_ids(ys.size());
        for (ShapePos shape_id: stack_element.shape_ids) {
            auto mm = shapes_min_max[shape_id];
            for (int i = 0; i < (int)xs.size(); ++i) {
                if (mm.first.x <= xs[i])
                    left_shape_ids[i].push_back(shape_id);
                if (mm.second.x >= xs[i])
                    right_shape_ids[i].push_back(shape_id);
            }
            for (int i = 0; i < (int)ys.size(); ++i) {
                if (mm.first.y <= ys[i])
                    bottom_shape_ids[i].push_back(shape_id);
                if (mm.second.y >= ys[i])
                    top_shape_ids[i].push_back(shape_id);
            }
        }

        // Select best cut.
        char best = 'x';
        ShapePos n = stack_element.shape_ids.size();
        ShapePos n_best = n * (n - 1) / 2;
        int i_best = -1;
        for (int i = 0; i < (int)xs.size(); ++i) {
            ShapePos nl = left_shape_ids[i].size();
            ShapePos nr = right_shape_ids[i].size();
            ShapePos nv = nl * (nl - 1) / 1 + nr * (nr - 1) / 2;
            if (n_best > nv) {
                n_best = nv;
                best = 'v';
                i_best = i;
            }
        }
        for (int i = 0; i < (int)ys.size(); ++i) {
            ShapePos nb = bottom_shape_ids[i].size();
            ShapePos nt = top_shape_ids[i].size();
            ShapePos nh = nb * (nb - 1) / 2 + nt * (nt - 1) / 2;
            if (n_best > nh) {
                n_best = nh;
                best = 'h';
                i_best = i;
            }
        }

        if (best == 'x') {
            node.direction = 'x';
            node.shape_ids = stack_element.shape_ids;
        } else if (best == 'v') {
            node.direction = 'v';
            node.position = xs[i_best];
            Node child_left;
            child_left.l = node.l;
            child_left.r = xs[i_best];
            child_left.b = node.b;
            child_left.t = node.t;
            StackElement stack_element_left;
            stack_element_left.node_id = tree_.size();
            node.lesser_child_id = tree_.size();
            stack_element_left.shape_ids = left_shape_ids[i_best];

            Node child_right;
            child_right.l = xs[i_best];
            child_right.r = node.r;
            child_right.b = node.b;
            child_right.t = node.t;
            StackElement stack_element_right;
            stack_element_right.node_id = tree_.size() + 1;
            node.greater_child_id = tree_.size() + 1;
            stack_element_right.shape_ids = right_shape_ids[i_best];

            tree_.push_back(child_left);
            tree_.push_back(child_right);
            stack.push_back(stack_element_right);
            stack.push_back(stack_element_left);
        } else {
            node.direction = 'h';
            node.position = ys[i_best];
            Node child_bottom;
            child_bottom.l = node.l;
            child_bottom.r = node.r;
            child_bottom.b = node.b;
            child_bottom.t = ys[i_best];
            StackElement stack_element_bottom;
            stack_element_bottom.node_id = tree_.size();
            node.lesser_child_id = tree_.size();
            stack_element_bottom.shape_ids = bottom_shape_ids[i_best];

            Node child_top;
            child_top.l = node.l;
            child_top.r = node.r;
            child_top.b = ys[i_best];
            child_top.t = node.t;
            StackElement stack_element_top;
            stack_element_top.node_id = tree_.size() + 1;
            node.greater_child_id = tree_.size() + 1;
            stack_element_top.shape_ids = top_shape_ids[i_best];

            tree_.push_back(child_bottom);
            tree_.push_back(child_top);
            stack.push_back(stack_element_top);
            stack.push_back(stack_element_bottom);
        }
    }
    //std::cout << "IntersectionTree::IntersectionTree end" << std::endl;
}

std::vector<ShapePos> IntersectionTree::intersect(
        const Shape& shape,
        bool strict) const
{
    //std::cout << "intersect..." << std::endl;

    if (shapes_->empty())
        return {};

    auto mm = shape.compute_min_max();

    optimizationtools::IndexedSet potentially_intersecting_shapes(shapes_->size());
    std::vector<NodeId> stack = {0};

    while (!stack.empty()) {

        NodeId node_id = stack.back();
        stack.pop_back();
        const Node& node = tree_[node_id];

        if (node.direction == 'x') {
            for (ShapePos shape_id: node.shape_ids)
                potentially_intersecting_shapes.add(shape_id);
        } else if (node.direction == 'v') {
            if (mm.first.x <= node.position)
                stack.push_back(node.lesser_child_id);
            if (mm.second.x >= node.position)
                stack.push_back(node.greater_child_id);
        } else {  // node.direction == 'h'
            if (mm.first.y <= node.position)
                stack.push_back(node.lesser_child_id);
            if (mm.second.y >= node.position)
                stack.push_back(node.greater_child_id);
        }
    }

    std::vector<ShapePos> intersecting_shapes;
    for (ShapePos shape_pos: potentially_intersecting_shapes)
        if (shape::intersect(shape, this->shape(shape_pos), strict))
            intersecting_shapes.push_back(shape_pos);
    return intersecting_shapes;
}

std::vector<ShapePos> IntersectionTree::intersect(
        const ShapeElement& element,
        bool strict) const
{
    if (shapes_->empty())
        return {};

    auto mm = element.min_max();

    optimizationtools::IndexedSet potentially_intersecting_shapes(shapes_->size());
    std::vector<NodeId> stack = {0};

    while (!stack.empty()) {

        NodeId node_id = stack.back();
        stack.pop_back();
        const Node& node = tree_[node_id];

        if (node.direction == 'x') {
            for (ShapePos shape_id: node.shape_ids)
                potentially_intersecting_shapes.add(shape_id);
        } else if (node.direction == 'v') {
            if (mm.first.x <= node.position)
                stack.push_back(node.lesser_child_id);
            if (mm.second.x >= node.position)
                stack.push_back(node.greater_child_id);
        } else {  // node.direction == 'h'
            if (mm.first.y <= node.position)
                stack.push_back(node.lesser_child_id);
            if (mm.second.y >= node.position)
                stack.push_back(node.greater_child_id);
        }
    }

    std::vector<ShapePos> intersecting_shapes;
    for (ShapePos shape_pos: potentially_intersecting_shapes)
        if (shape::intersect(this->shape(shape_pos), element, strict))
            intersecting_shapes.push_back(shape_pos);
    return intersecting_shapes;
}

std::vector<std::pair<ShapePos, ShapePos>> IntersectionTree::compute_intersecting_shapes(bool strict) const
{
    //std::cout << "compute_intersecting_shapes..." << std::endl;

    std::vector<std::pair<ShapePos, ShapePos>> potentially_intersecting_shapes;
    for (const Node& node: tree_) {
        if (node.direction != 'x')
            continue;
        for (ShapePos pos_1 = 0;
                pos_1 < node.shape_ids.size();
                ++pos_1) {
            ShapePos shape_id_1 = node.shape_ids[pos_1];
            for (ShapePos pos_2 = pos_1 + 1;
                    pos_2 < node.shape_ids.size();
                    ++pos_2) {
                ShapePos shape_id_2 = node.shape_ids[pos_2];
                potentially_intersecting_shapes.push_back({shape_id_1, shape_id_2});
            }
        }
    }

    // Remove duplicates.
    std::sort(potentially_intersecting_shapes.begin(), potentially_intersecting_shapes.end());
    potentially_intersecting_shapes.erase(
            unique(potentially_intersecting_shapes.begin(), potentially_intersecting_shapes.end()),
            potentially_intersecting_shapes.end());

    // Compute intersections.
    std::vector<std::pair<ShapePos, ShapePos>> intersecting_shapes;
    for (auto p: potentially_intersecting_shapes)
        if (shape::intersect(this->shape(p.first), this->shape(p.second), strict))
            intersecting_shapes.push_back(p);
    return intersecting_shapes;
}
