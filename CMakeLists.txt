cmake_minimum_required(VERSION 3.21)

project(simple_undirected_graph VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(simple_undirected_graph INTERFACE)
add_library(
    simple_undirected_graph::simple_undirected_graph
    ALIAS simple_undirected_graph
)

target_compile_features(simple_undirected_graph INTERFACE cxx_std_20)
target_include_directories(
    simple_undirected_graph
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

option(
    SIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES
    "Build simple-undirected-graph examples"
    ${PROJECT_IS_TOP_LEVEL}
)
option(
    SIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS
    "Build simple-undirected-graph tests"
    ${PROJECT_IS_TOP_LEVEL}
)

if(SIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS simple_undirected_graph EXPORT simple_undirected_graphTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(
    EXPORT simple_undirected_graphTargets
    FILE simple_undirected_graphTargets.cmake
    NAMESPACE simple_undirected_graph::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simple_undirected_graph
)

configure_package_config_file(
    cmake/simple_undirected_graphConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/simple_undirected_graphConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simple_undirected_graph
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/simple_undirected_graphConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simple_undirected_graph
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/simple_undirected_graphConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/simple_undirected_graphConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simple_undirected_graph
)