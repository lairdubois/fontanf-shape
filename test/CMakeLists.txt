enable_testing()

include(GoogleTest)

add_executable(Shape_shape_test)
target_sources(Shape_shape_test PRIVATE
    trapezoid_test.cpp
    shape_test.cpp
    element_intersections_test.cpp
    convex_hull_test.cpp
    extract_borders_test.cpp
    self_intersections_removal_test.cpp
    trapezoidation_test.cpp
    intersection_tree_test.cpp
    offset_test.cpp
    supports_test.cpp)
target_include_directories(Shape_shape_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(Shape_shape_test
    Shape_shape
    Boost::filesystem
    GTest::gtest_main)
gtest_discover_tests(Shape_shape_test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
