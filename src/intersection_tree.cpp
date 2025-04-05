#include "shape/intersection_tree.hpp"

#include "shape/element_intersections.hpp"

#include "optimizationtools/containers/indexed_set.hpp"

using namespace shape;

IntersectionTree::IntersectionTree(
        const std::vector<Shape>& shapes):
    shapes_(shapes)
{

    // TODO

}

std::vector<ShapePos> IntersectionTree::intersect(
        const Shape& shape,
        bool strict) const
{
    if (shapes_.empty())
        return {};

    auto mm = shape.compute_min_max();

    optimizationtools::IndexedSet potentially_intersecting_shapes(shapes_.size());
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
        if (shape::intersect(shape, shapes_[shape_pos], strict))
            intersecting_shapes.push_back(shape_pos);
    return intersecting_shapes;
}
