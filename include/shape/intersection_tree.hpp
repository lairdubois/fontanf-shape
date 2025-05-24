#pragma once

#include "shape/shape.hpp"

namespace shape
{

class IntersectionTree
{

public:

    /** Constructor. */
    IntersectionTree(
            const std::vector<Shape>& shapes,
            const std::vector<ShapeElement>& elements,
            const std::vector<Point>& points);


    struct IntersectOutput
    {
        std::vector<ShapePos> shape_ids;

        std::vector<ShapePos> element_ids;

        std::vector<ShapePos> point_ids;
    };

    /** Check if a given shape intersects one of the tree shapes. */
    IntersectOutput intersect(
            const Shape& shape,
            bool strict) const;

    /** Check if a given element intersects one of the tree shapes. */
    IntersectOutput intersect(
            const ShapeElement& element,
            bool strict) const;

    /** Check if a given element intersects one of the tree shapes. */
    IntersectOutput intersect(
            const Point& point,
            bool strict) const;


    /** Get all the pairs of intersecting shapes in the tree. */
    std::vector<std::pair<ShapePos, ShapePos>> compute_intersecting_shapes(bool strict) const;

    struct ElementElementIntersection
    {
        ElementPos element_id_1 = -1;
        ElementPos element_id_2 = -1;
        std::vector<Point> intersections;
    };

    /** Get all the pairs of intersecting shape elements in the tree. */
    std::vector<ElementElementIntersection> compute_intersecting_elements(bool strict) const;

    /** Get all the pairs of equal points in the tree. */
    std::vector<std::pair<ElementPos, ElementPos>> compute_equal_points() const;

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

        std::vector<ElementPos> element_ids;

        std::vector<ElementPos> point_ids;
    };

    struct StackElement
    {
        NodeId node_id;
        std::vector<ShapePos> shape_ids;
        std::vector<ElementPos> element_ids;
        std::vector<ElementPos> point_ids;
    };

    void build_node_rec(
            NodeId node_id,
            const std::vector<ShapePos>& shape_ids);


    /** Get a shape of the intersection tree. */
    const Shape& shape(ShapePos shape_pos) const { return (*shapes_)[shape_pos]; }

    /** Get a shape element of the intersection tree. */
    const ShapeElement& element(ElementPos element_pos) const { return (*elements_)[element_pos]; }

    /** Get a point of the intersection tree. */
    const Point& point(ElementPos point_pos) const { return (*points_)[point_pos]; }


    /** Shapes. */
    const std::vector<Shape>* shapes_ = nullptr;

    /** Shape elements. */
    const std::vector<ShapeElement>* elements_ = nullptr;

    /** Points. */
    const std::vector<Point>* points_ = nullptr;

    std::vector<Node> tree_;

};

}
