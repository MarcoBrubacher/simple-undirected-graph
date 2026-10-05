#include <simple_undirected_graph/graph.hpp>

#include <iostream>
#include <string>
#include <utility>

struct City
{
    explicit City(std::string name) : name(std::move(name)) {}
    std::string name;
};

struct Road
{
    explicit Road(double distanceKm) : distanceKm(distanceKm) {}
    double distanceKm;
};

int main()
{
    using Network = simple_undirected_graph::Graph<City, Road>;

    Network network;

    Network::VertexId paris = network.emplaceVertex("Paris");
    Network::VertexId brussels = network.emplaceVertex("Brussels");
    Network::VertexId amsterdam = network.emplaceVertex("Amsterdam");
    Network::VertexId cologne = network.emplaceVertex("Cologne");

    network.emplaceEdge(paris, brussels, 320.0);
    network.emplaceEdge(paris, amsterdam, 510.0);
    network.emplaceEdge(brussels, cologne, 220.0);
    network.emplaceEdge(amsterdam, cologne, 265.0);

    std::cout << "Cities:\n";
    for (Network::VertexId cityId : network.vertices())
    {
        std::cout << network.vertexData(cityId).name << " (" << network.degree(cityId) << " connections)\n";
    }

    std::cout << "\nRoads\n";
    for (Network::EdgeId roadId : network.edges())
    {
        std::pair<Network::VertexId, Network::VertexId> endpoints = network.endpoints(roadId);

        const City &firstCity = network.vertexData(endpoints.first);
        const City &secondCity = network.vertexData(endpoints.second);
        const Road &road = network.edgeData(roadId);

        std::cout << firstCity.name << " to " << secondCity.name << ": " << road.distanceKm << " km\n";
    }

    std::cout << "\nFrom Paris\n";
    for (const Network::Adjacency &connection : network.adjacency(paris))
    {
        const City &city = network.vertexData(connection.neighbor);
        const Road &road = network.edgeData(connection.edge);

        std::cout << city.name << ": " << road.distanceKm << " km\n";
    }

    return 0;
}