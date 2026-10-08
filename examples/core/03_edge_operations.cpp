#include <simple_undirected_graph/graph.hpp>

#include <iostream>
#include <string>
#include <utility>

struct Atom
{
    Atom(std::string symbol, int atomicNumber) : symbol(std::move(symbol)), atomicNumber(atomicNumber) {}
    std::string symbol;
    int atomicNumber;
};

struct Bond
{
    explicit Bond(int order) : order(order) {}
    int order;
};

int main()
{
    using Molecule = simple_undirected_graph::Graph<Atom, Bond>;

    Molecule propene;

    Molecule::VertexId carbon1 = propene.emplaceVertex("C", 6);
    Molecule::VertexId carbon2 = propene.emplaceVertex("C", 6);
    Molecule::VertexId carbon3 = propene.emplaceVertex("C", 6);

    Molecule::VertexId hydrogen1 = propene.emplaceVertex("H", 1);
    Molecule::VertexId hydrogen2 = propene.emplaceVertex("H", 1);
    Molecule::VertexId hydrogen3 = propene.emplaceVertex("H", 1);
    Molecule::VertexId hydrogen4 = propene.emplaceVertex("H", 1);
    Molecule::VertexId hydrogen5 = propene.emplaceVertex("H", 1);
    Molecule::VertexId hydrogen6 = propene.emplaceVertex("H", 1);

    Bond singleBond{1};
    propene.addEdge(carbon2, carbon3, singleBond);

    propene.addEdge(carbon1, hydrogen1, Bond{1});

    propene.emplaceEdge(carbon1, hydrogen2, 1);
    propene.emplaceEdge(carbon2, hydrogen3, 1);
    propene.emplaceEdge(carbon3, hydrogen4, 1);
    propene.emplaceEdge(carbon3, hydrogen5, 1);
    propene.emplaceEdge(carbon3, hydrogen6, 1);

    std::pair<Molecule::EdgeId, bool> doubleBondInsertion = propene.emplaceEdge(carbon1, carbon2, 2);

    Molecule::EdgeId doubleBondId = doubleBondInsertion.first;
    bool doubleBondInserted = doubleBondInsertion.second;

    std::cout << std::boolalpha;
    std::cout << "Double bond inserted: " << doubleBondInserted << '\n';

    std::cout << "Bond count: " << propene.edgeCount() << '\n';

    std::cout << "Carbon 1 and carbon 2 connected: " << propene.hasEdge(carbon1, carbon2) << '\n';
    std::cout << "C1-C2 bond order: " << propene.edgeData(doubleBondId).order << '\n';

    propene.edgeData(doubleBondId).order = 1;
    std::cout << "Modified C1-C2 bond order: " << propene.edgeData(doubleBondId).order << '\n';
    propene.edgeData(doubleBondId).order = 2;

    std::optional<Molecule::EdgeId> foundBond = propene.findEdgeId(carbon2, carbon3);
    if (foundBond.has_value())
    {
        std::cout << "C2-C3 bond order: " << propene.edgeData(*foundBond).order << '\n';
    }

    std::pair<Molecule::VertexId, Molecule::VertexId> bondEndpoints = propene.endpoints(doubleBondId);
    std::cout << "First endpoint exists: " << propene.vertexExists(bondEndpoints.first) << '\n';
    std::cout << "Second endpoint exists: " << propene.vertexExists(bondEndpoints.second) << '\n';

    std::pair<Molecule::EdgeId, bool> duplicate = propene.emplaceEdge(carbon1, carbon2, 1);
    std::cout << "Duplicate edge inserted: " << duplicate.second << '\n';
    std::cout << "Existing bond order after duplicate insertion: " << propene.edgeData(duplicate.first).order << '\n';

    bool removed = propene.removeEdge(doubleBondId);
    std::cout << "Double bond removed: " << removed << '\n';

    simple_undirected_graph::Graph<> propertylessGraph;
    simple_undirected_graph::Graph<>::VertexId vertex1 = propertylessGraph.addVertex();
    simple_undirected_graph::Graph<>::VertexId vertex2 = propertylessGraph.addVertex();
    propertylessGraph.addEdge(vertex1, vertex2);

    return 0;
}
