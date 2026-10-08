#include <simple_undirected_graph/graph.hpp>

#include <iostream>
#include <string>
#include <utility>

struct MoleculeInformation
{
    MoleculeInformation(std::string name, std::string formula, int charge) : name(std::move(name)), formula(std::move(formula)), charge(charge) {}
    std::string name;
    std::string formula;
    int charge;
};

int main()
{
    using Molecule = simple_undirected_graph::Graph<simple_undirected_graph::NoProperties, simple_undirected_graph::NoProperties, MoleculeInformation>;

    Molecule benzene(MoleculeInformation{"Benzene", "C6H6", 0});

    std::cout << "Molecule: " << benzene.graphData().name << '\n';
    std::cout << "Formula: " << benzene.graphData().formula << '\n';
    std::cout << "Charge: " << benzene.graphData().charge << '\n';

    std::cout << std::boolalpha;
    std::cout << "Graph empty: " << benzene.empty() << '\n';

    Molecule::VertexId carbon1 = benzene.addVertex();
    Molecule::VertexId carbon2 = benzene.addVertex();

    benzene.addEdge(carbon1, carbon2);

    std::cout << "Graph empty after adding vertices: " << benzene.empty() << '\n';
    std::cout << "Vertex count: " << benzene.vertexCount() << '\n';
    std::cout << "Edge count: " << benzene.edgeCount() << '\n';

    benzene.graphData().charge = 1;
    std::cout << "Modified charge: " << benzene.graphData().charge << '\n';

    benzene.clear(); // removes all vertices and edges but keeps the graph data object

    return 0;
}