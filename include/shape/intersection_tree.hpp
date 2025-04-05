#pragma once

#include "shape/shape.hpp"

namespace shape
{

class IntersectionTree
{

public:

    /** Constructor. */
    IntersectionTree(const std::vector<Shape>& shapes);

    /** Check if a given shape intersects one of the tree shapes. */
    std::vector<ShapePos> intersect(
            const Shape& shape,
            bool strict) const;

    /** Get all the pairs of intersecting shapes in the tree. */
    std::vector<std::pair<ShapePos, ShapePos>> compute_intersecting_shapes(bool strict) const;

private:

    using NodeId = int64_t;

    struct Node
    {
        LengthDbl l = std::numeric_limits<LengthDbl>::infinity();

        LengthDbl r = -std::numeric_limits<LengthDbl>::infinity();

        LengthDbl b = std::numeric_limits<LengthDbl>::infinity();

        LengthDbl t = -std::numeric_limits<LengthDbl>::infinity();

        char direction = 'x';

        LengthDbl position = 0.0;

        NodeId lesser_child_id = -1;

        NodeId greater_child_id = -1;

        std::vector<ShapePos> shape_ids;
    };

    struct StackElement
    {
        NodeId node_id;
        std::vector<ShapePos> shape_ids;
    };

    void build_node_rec(
            NodeId node_id,
            const std::vector<ShapePos>& shape_ids);

    const std::vector<Shape>& shapes_;

    std::vector<Node> tree_;

};

}
