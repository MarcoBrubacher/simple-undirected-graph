#include <simple_undirected_graph/graph.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

int main()
{
    using Graph = simple_undirected_graph::Graph<>;

    Graph network;

    Graph::VertexId london = network.addVertex();
    Graph::VertexId paris = network.addVertex();
    Graph::VertexId berlin = network.addVertex();

    std::pair<Graph::EdgeId, bool> londonParis = network.addEdge(london, paris);

    std::pair<Graph::EdgeId, bool> parisBerlin = network.addEdge(paris, berlin);

    // VertexIdHash allows vertex IDs to be used as keys in unordered containers
    std::unordered_map<Graph::VertexId, std::string, Graph::VertexIdHash> cityNames;

    cityNames.emplace(london, "London");
    cityNames.emplace(paris, "Paris");
    cityNames.emplace(berlin, "Berlin");

    std::cout << cityNames.at(london) << '\n';
    std::cout << cityNames.at(paris) << '\n';
    std::cout << cityNames.at(berlin) << '\n';

    // useful for algorithms that need to remember visited vertices
    std::unordered_set<Graph::VertexId, Graph::VertexIdHash> visited;

    visited.insert(london);
    visited.insert(paris);

    std::cout << std::boolalpha;
    std::cout << "Paris visited: " << visited.contains(paris) << '\n';
    std::cout << "Berlin visited: " << visited.contains(berlin) << '\n';

    // EdgeIdHash provides the same for edge IDs
    std::unordered_map<Graph::EdgeId, std::string, Graph::EdgeIdHash> roadNames;

    roadNames.emplace(londonParis.first, "London-Paris");
    roadNames.emplace(parisBerlin.first, "Paris-Berlin");

    std::cout << "Road: " << roadNames.at(londonParis.first) << '\n';

    return 0;
}