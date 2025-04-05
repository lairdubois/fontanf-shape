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

private:

    using NodeId = int64_t;

    struct Node
    {
        char direction = 'x';

        LengthDbl position = 0.0;

        NodeId lesser_child_id = -1;

        NodeId greater_child_id = -1;

        std::vector<ShapePos> shape_ids;
    };

    const std::vector<Shape>& shapes_;

    std::vector<Node> tree_;

};

}
