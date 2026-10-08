#include <simple_undirected_graph/graph.hpp>

#include <iostream>
#include <string>
#include <utility>

int main()
{
    using Graph = simple_undirected_graph::Graph<std::string>;
    std::cout << std::boolalpha;

    Graph original;
    Graph::VertexId originalId = original.emplaceVertex("original vertex");

    Graph copied(original);

    std::cout << "Original ID is valid in copy: " << copied.vertexExists(originalId) << '\n';

    Graph copySource;
    Graph::VertexId copySourceId = copySource.emplaceVertex("copy source");

    Graph copyTarget;
    copyTarget.emplaceVertex("old target vertex");

    copyTarget = copySource;

    // copied graphs get their own graph identity
    std::cout << "Source ID is valid after copy assignment: " << copyTarget.vertexExists(copySourceId) << '\n';

    Graph moveSource;
    Graph::VertexId movedId = moveSource.emplaceVertex("moved vertex");

    Graph moved(std::move(moveSource));

    // moving keeps existing IDs valid in the destination
    std::cout << "Moved ID is valid in destination: " << moved.vertexExists(movedId) << '\n';
    std::cout << "Moved-from graph is now empty: " << moveSource.empty() << '\n';

    Graph moveAssignmentSource;
    Graph::VertexId moveAssignmentId = moveAssignmentSource.emplaceVertex("move-assigned vertex");
    Graph moveAssignmentTarget;

    moveAssignmentTarget = std::move(moveAssignmentSource);

    std::cout << "Moved ID is valid after the move assignment: " << moveAssignmentTarget.vertexExists(moveAssignmentId) << '\n';
    std::cout << "Moved-from graph is empty: " << moveAssignmentSource.empty() << '\n';

    return 0;
}