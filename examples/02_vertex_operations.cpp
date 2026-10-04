#include <simple_undirected_graph.hpp>

#include <iostream>
#include <string>

int main()
{
    // defines the graph type and gives it a shorter name for convenience
    using Graph = simple_undirected_graph::Graph<std::string>;

    Graph graph;

    // adds a vertex without storing its ID
    graph.addVertex("first vertex");

    // adds a vertex and stores its ID for later operations
    Graph::VertexId secondVertexId = graph.addVertex("second vertex");

    // prints the number of vertices currently stored in the graph
    std::cout << "Vertex count: " << graph.vertexCount() << '\n';

    // reads the vertex data through its ID and stores a copy
    std::string vertexData = graph.vertexData(secondVertexId);
    std::cout << "Vertex 2 data: " << vertexData << '\n';

    // modifies the vertex data directly through its ID
    graph.vertexData(secondVertexId) = "modified second vertex";
    std::cout << "Modified vertex 2 data: " << graph.vertexData(secondVertexId) << '\n';

    // checks whether the stored ID currently refers to an existing vertex
    std::cout << std::boolalpha;
    std::cout << "Vertex 2 exists before removal: " << graph.vertexExists(secondVertexId) << '\n';

    // removes the vertex and reports whether the removal succeeded
    bool removed = graph.removeVertex(secondVertexId);
    std::cout << "Vertex 2 removed: " << removed << '\n';

    // checks that the removed vertex ID is no longer valid
    std::cout << "Vertex 2 exists after removal: " << graph.vertexExists(secondVertexId) << '\n';

    // prints the number of vertices remaining in the graph
    std::cout << "Vertex count after removal: " << graph.vertexCount() << '\n';

    return 0;
}