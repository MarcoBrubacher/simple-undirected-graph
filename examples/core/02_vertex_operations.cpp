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
    using Graph = simple_undirected_graph::Graph<Protein>;

    Graph graph;

    // adding vertices //
    Protein tp53{"TP53", 393};
    graph.addVertex(tp53);

    Protein mdm2{"MDM2", 491};
    Graph::VertexId mdm2Id = graph.addVertex(mdm2);

    Graph::VertexId egfrId = graph.emplaceVertex("EGFR", 1210);
    std::cout << "Protein count: " << graph.vertexCount() << '\n';

    // accessing vertex data //
    const Protein &egfr = graph.vertexData(egfrId);
    std::cout << "Protein: " << egfr.name << ", amino acids: " << egfr.aminoAcidCount << '\n';

    graph.vertexData(mdm2Id).name = "MDM2 protein";
    std::cout << "Modified protein: " << graph.vertexData(mdm2Id).name << '\n';

    // checking and removing vertices //
    std::cout << std::boolalpha;
    std::cout << "MDM2 exists before removal: " << graph.vertexExists(mdm2Id) << '\n';

    bool removed = graph.removeVertex(mdm2Id);
    std::cout << "MDM2 removed: " << removed << '\n';

    // propertyless vertices //
    simple_undirected_graph::Graph<> propertylessGraph;
    propertylessGraph.addVertex();

    return 0;
}