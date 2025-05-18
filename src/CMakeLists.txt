find_package(Threads)
add_library(Shape_shape)
target_sources(Shape_shape PRIVATE
    shape.cpp
    element_intersections.cpp
    boolean_operations.cpp
    convex_hull.cpp
    extract_borders.cpp
    trapezoidation.cpp
    intersection_tree.cpp
    equalize.cpp
    offset.cpp
    simplification.cpp
    supports.cpp)
target_include_directories(Shape_shape PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_include_directories(Shape_shape PRIVATE
    ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(Shape_shape PUBLIC
    OptimizationTools::containers
    nlohmann_json::nlohmann_json)
add_library(Shape::shape ALIAS Shape_shape)
