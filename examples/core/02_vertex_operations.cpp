#include <simple_undirected_graph/graph.hpp>

#include <iostream>
#include <string>
#include <utility>

struct Protein
{
    Protein(std::string name, int aminoAcidCount) : name(std::move(name)), aminoAcidCount(aminoAcidCount) {}

    std::string name;
    int aminoAcidCount;
};

int main()
{
    // represents proteins as vertices of a protein-protein interaction network
    using Graph = simple_undirected_graph::Graph<Protein>;

    Graph graph;

    // vertex data can be created outside the graph and then copied into it
    Protein tp53{"TP53", 393};
    graph.addVertex(tp53);

    // stores the ID when the vertex needs to be accessed later
    Protein mdm2{"MDM2", 491};
    Graph::VertexId mdm2Id = graph.addVertex(mdm2);

    // constructs the Protein directly inside the graph from its constructor arguments
    Graph::VertexId egfrId = graph.emplaceVertex("EGFR", 1210);

    // prints the number of proteins currently represented by the graph
    std::cout << "Protein count: " << graph.vertexCount() << '\n';

    // reads the protein data through its vertex ID
    const Protein &egfr = graph.vertexData(egfrId);
    std::cout << "Protein: " << egfr.name << ", amino acids: " << egfr.aminoAcidCount << '\n';

    // modifies stored vertex data directly through its ID
    graph.vertexData(mdm2Id).name = "MDM2 protein";
    std::cout << "Modified protein: " << graph.vertexData(mdm2Id).name << '\n';

    // checks whether the ID currently refers to an existing vertex
    std::cout << std::boolalpha;
    std::cout << "MDM2 exists before removal: " << graph.vertexExists(mdm2Id) << '\n';

    // removes the protein vertex and reports whether the removal succeeded
    bool removed = graph.removeVertex(mdm2Id);
    std::cout << "MDM2 removed: " << removed << '\n';

    // the old ID no longer refers to an existing vertex
    std::cout << "MDM2 exists after removal: " << graph.vertexExists(mdm2Id) << '\n';

    std::cout << "Protein count after removal: " << graph.vertexCount() << '\n';

    // propertyless graphs can still add vertices without supplying data
    simple_undirected_graph::Graph<> propertylessGraph;
    propertylessGraph.addVertex();

    return 0;
}