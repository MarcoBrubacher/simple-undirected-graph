#include <utility>
#include <iostream>

namespace simple_undirected_graph
{
    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::Graph() : graphId_(nextGraphId_++)
    {
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::Graph(GraphData graphData) : graphId_(nextGraphId_++), graphData_(std::move(graphData))
    {
    }

    // initialize all graph members by copying the source graph, while giving the copy a new graph ID
    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::Graph(const Graph &sourceGraph) : graphId_(nextGraphId_++),
                                                                              graphData_(sourceGraph.graphData_),
                                                                              vertexSlots_(sourceGraph.vertexSlots_),
                                                                              edgeSlots_(sourceGraph.edgeSlots_),
                                                                              freeVertexSlots_(sourceGraph.freeVertexSlots_),
                                                                              freeEdgeSlots_(sourceGraph.freeEdgeSlots_),
                                                                              liveVertices_(sourceGraph.liveVertices_),
                                                                              liveEdges_(sourceGraph.liveEdges_),
                                                                              edgeLookup_(sourceGraph.edgeLookup_)
    {
        // the graph was copied with a new graph ID, but all IDs inside the copied storage still contain the source graph ID
        // therefore every stored VertexId and EdgeId must be updated to belong to this graph

        for (VertexId &id : liveVertices_)
        {
            id.graphId = graphId_;
        }

        for (EdgeId &id : liveEdges_)
        {
            id.graphId = graphId_;
        }

        for (VertexSlot &slot : vertexSlots_)
        {
            if (!slot.vertex.has_value())
            {
                continue;
            }

            for (Adjacency &entry : slot.vertex->adjacency)
            {
                entry.neighbor.graphId = graphId_;
                entry.edge.graphId = graphId_;
            }
        }

        for (EdgeSlot &slot : edgeSlots_)
        {
            if (!slot.edge.has_value())
            {
                continue;
            }
            slot.edge->endpointU.graphId = graphId_;
            slot.edge->endpointV.graphId = graphId_;
        }

        // go through every stored endpoint-pair -> EdgeId mapping, where entry.first is the EndpointPair and entry.second is the EdgeId
        // the copied EdgeIds still contain the old graphId, so update each one to this graph's new graphId_
        for (typename std::unordered_map<EndpointPair, EdgeId, EndpointPairHasher>::value_type &entry : edgeLookup_)
        {
            entry.second.graphId = graphId_;
        }
    }

    // replace this graph with an independent copy of the source graph
    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData> &Graph<VertexData, EdgeData, GraphData>::operator=(const Graph &sourceGraph)
    {
        // if both names refer to the same graph there is nothing to copy
        if (this == &sourceGraph)
        {
            return *this;
        }

        // use the copy constructor so the copied graph gets a new graph ID and all IDs inside the copied storage are updated correctly
        Graph copiedGraph(sourceGraph);

        // move the finished copy into the already existing graph
        *this = std::move(copiedGraph);

        return *this;
    }

    // create a new graph by taking over the source graph's contents and existing graph ID
    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::Graph(Graph &&sourceGraph) : graphId_(sourceGraph.graphId_),
                                                                         graphData_(std::move(sourceGraph.graphData_)),
                                                                         vertexSlots_(std::move(sourceGraph.vertexSlots_)),
                                                                         edgeSlots_(std::move(sourceGraph.edgeSlots_)),
                                                                         freeVertexSlots_(std::move(sourceGraph.freeVertexSlots_)),
                                                                         freeEdgeSlots_(std::move(sourceGraph.freeEdgeSlots_)),
                                                                         liveVertices_(std::move(sourceGraph.liveVertices_)),
                                                                         liveEdges_(std::move(sourceGraph.liveEdges_)),
                                                                         edgeLookup_(std::move(sourceGraph.edgeLookup_))
    {
        // give the moved-from graph a fresh ID because its old ID was transferred to the new graph
        sourceGraph.graphId_ = nextGraphId_++;
    }

    // replace this graph by taking over the source graph's contents and graph ID
    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData> &Graph<VertexData, EdgeData, GraphData>::operator=(Graph &&sourceGraph)
    {
        // if both names refer to the same graph there is nothing to move
        if (this == &sourceGraph)
        {
            return *this;
        }

        graphId_ = sourceGraph.graphId_;
        graphData_ = std::move(sourceGraph.graphData_);

        vertexSlots_ = std::move(sourceGraph.vertexSlots_);
        edgeSlots_ = std::move(sourceGraph.edgeSlots_);

        freeVertexSlots_ = std::move(sourceGraph.freeVertexSlots_);
        freeEdgeSlots_ = std::move(sourceGraph.freeEdgeSlots_);

        liveVertices_ = std::move(sourceGraph.liveVertices_);
        liveEdges_ = std::move(sourceGraph.liveEdges_);

        edgeLookup_ = std::move(sourceGraph.edgeLookup_);

        // this graph now uses the source graph's old ID, thus give the source graph a new ID so both graph objects do not have the same one
        sourceGraph.graphId_ = nextGraphId_++;

        return *this;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    GraphData &Graph<VertexData, EdgeData, GraphData>::graphData()
    {
        return graphData_;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    const GraphData &Graph<VertexData, EdgeData, GraphData>::graphData() const
    {
        return graphData_;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    void Graph<VertexData, EdgeData, GraphData>::clear()
    {
        vertexSlots_.clear();
        edgeSlots_.clear();

        freeVertexSlots_.clear();
        freeEdgeSlots_.clear();

        liveVertices_.clear();
        liveEdges_.clear();

        edgeLookup_.clear();

        // give the cleared graph a new ID so all IDs are created before clear() become invalid
        graphId_ = nextGraphId_++;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::empty() const
    {
        return liveVertices_.empty();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    typename Graph<VertexData, EdgeData, GraphData>::VertexId Graph<VertexData, EdgeData, GraphData>::addVertex(VertexData vertexData)
    {
        // create a vertex with the supplied data and an empty adjacency list; then move the supplied data into the stored vertex to avoid an unnecessary copy
        Vertex v{std::move(vertexData), {}};

        std::size_t slotIndex;
        // choose a slot index, if a free slot exists, reuse it otherwise append a new VertexSlot
        if (!freeVertexSlots_.empty())
        {
            slotIndex = freeVertexSlots_.back();
            freeVertexSlots_.pop_back();
            vertexSlots_[slotIndex].vertex = std::move(v);
            // a slot was reused, thus the generation of the VertexSlot must increase
            vertexSlots_[slotIndex].generation++;
        }
        else
        {
            slotIndex = vertexSlots_.size();
            vertexSlots_.emplace_back();
            vertexSlots_[slotIndex].vertex = std::move(v);
            // generation stays the same so nothing to do
        }

        VertexId id{graphId_, slotIndex, vertexSlots_[slotIndex].generation};
        // store the ID's position in the slot's liveIndex
        vertexSlots_[slotIndex].liveIndex = liveVertices_.size();
        liveVertices_.push_back(std::move(id));

        return liveVertices_.back();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::removeVertex(VertexId id)
    {
        // check if it even exists
        if (!vertexExists(id))
        {
            return false;
        }

        // since std::optional<Vertex> is a container that may hold a Vertex or may be empty, vertexSlots_[id.index].vertex returns std::optional<Vertex>
        // by dereferencing this, it uses std::optional's overloaded operator*() to access the Vertex and through the Vertex one can access the adjacency list, where it can be checked if empty
        // this allows iterating over it and remove all elements (edges) until it is empty to make sure the vertex is isolated and holds no more edges, and then it can be safely removed
        while (!(*vertexSlots_[id.index].vertex).adjacency.empty())
        {
            // take the last adjacency entry of this vertex and get the EdgeId stored inside it
            // removeEdge() will delete that edge from both endpoint adjacency lists, so this vertex's adjacency list becomes one element smaller each loop
            removeEdge((*vertexSlots_[id.index].vertex).adjacency.back().edge);
        }

        // get the position of this VertexId inside liveVertices_ and the position of the last live VertexId
        std::size_t liveIndex = vertexSlots_[id.index].liveIndex;
        std::size_t lastLiveIndex = liveVertices_.size() - 1;

        // Erasing from the middle of liveVertices_ would shift later elements and make removal O(n) (because of the left shift of all other elements to the right).
        // Since iteration order is not significant, swap-and-pop is applied instead:
        // Swap-and-pop removes an element in O(1) by copying the last element over it and then removing the now-duplicate last element.
        if (liveIndex != lastLiveIndex)
        {
            VertexId movedId = liveVertices_.back();
            liveVertices_[liveIndex] = movedId;
            vertexSlots_[movedId.index].liveIndex = liveIndex;
        }
        liveVertices_.pop_back();

        vertexSlots_[id.index].vertex.reset(); // destroy the Vertex stored in its slot
        freeVertexSlots_.push_back(id.index);  // store the now empty slot so addVertex() can reuse it later

        return true;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::vertexCount() const
    {
        return liveVertices_.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::vertexExists(VertexId id) const
    {
        // validate the ID using cheap checks first and only access the slot once the index is known to be valid
        // check graphId before touching vertexSlots_, then check the index bounds, and only then inspect the optional stored in that slot, since inspecting a slot is the most expensive
        // and conclude by comparing the generation to reject stale IDs from an earlier use of the same slot
        if (id.graphId != graphId_ || id.index >= vertexSlots_.size() || !vertexSlots_[id.index].vertex.has_value() || id.generation != vertexSlots_[id.index].generation)
        {
            return false;
        }
        return true;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::degree(VertexId id) const
    {
        if (!vertexExists(id))
        {
            throw std::invalid_argument("vertex does not exist");
        }
        return (*vertexSlots_[id.index].vertex).adjacency.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    const std::vector<typename Graph<VertexData, EdgeData, GraphData>::Adjacency> &
    Graph<VertexData, EdgeData, GraphData>::adjacency(VertexId id) const
    {
        if (!vertexExists(id))
        {
            throw std::invalid_argument("vertex does not exist");
        }
        return (*vertexSlots_[id.index].vertex).adjacency;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    VertexData &Graph<VertexData, EdgeData, GraphData>::vertexData(VertexId id)
    {
        if (!vertexExists(id))
        {
            throw std::invalid_argument("vertex does not exist");
        }
        return (*vertexSlots_[id.index].vertex).data;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    const VertexData &Graph<VertexData, EdgeData, GraphData>::vertexData(VertexId id) const
    {
        if (!vertexExists(id))
        {
            throw std::invalid_argument("vertex does not exist");
        }
        return (*vertexSlots_[id.index].vertex).data;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::pair<typename Graph<VertexData, EdgeData, GraphData>::EdgeId, bool> Graph<VertexData, EdgeData, GraphData>::addEdge(VertexId u, VertexId v, EdgeData edgeData)
    {
        if (!vertexExists(u) || !vertexExists(v))
        {
            throw std::invalid_argument("vertex does not exist");
        }

        if (u == v)
        {
            throw std::invalid_argument("self-loops are not allowed");
        }

        // normalize the two endpoint slot indices so (U,V) and (V,U) result in the same EndpointPair
        std::size_t endpointIndexA = u.index;
        std::size_t endpointIndexB = v.index;

        if (endpointIndexA > endpointIndexB)
        {
            std::swap(endpointIndexA, endpointIndexB);
        }
        EndpointPair endpoints{endpointIndexA, endpointIndexB};

        // check the lookup map whether this undirected edge already exists. If it exists leave its data unchanged and return its existing ID
        typename std::unordered_map<EndpointPair, EdgeId, EndpointPairHasher>::iterator existingEdge = edgeLookup_.find(endpoints);

        if (existingEdge != edgeLookup_.end())
        {
            return {existingEdge->second, false};
        }

        // access the adjacency lists of both endpoint vertices
        std::vector<Adjacency> &adjacencyU = (*vertexSlots_[u.index].vertex).adjacency;
        std::vector<Adjacency> &adjacencyV = (*vertexSlots_[v.index].vertex).adjacency;

        std::size_t slotIndex;
        // choose a slot index, if a free slot exists reuse it otherwise append a new EdgeSlot
        if (!freeEdgeSlots_.empty())
        {
            slotIndex = freeEdgeSlots_.back();
            freeEdgeSlots_.pop_back();

            // a slot was reused, thus the generation of the EdgeSlot must increase
            edgeSlots_[slotIndex].generation++;
        }
        else
        {
            slotIndex = edgeSlots_.size();
            edgeSlots_.emplace_back();
            // generation stays the same so nothing to do
        }

        EdgeId id{graphId_, slotIndex, edgeSlots_[slotIndex].generation};

        // since the new adjacency entries will be appended at the end, the current sizes are exactly their future indices
        std::size_t adjacencyIndexU = adjacencyU.size();
        std::size_t adjacencyIndexV = adjacencyV.size();

        // create the Edge and store where its two adjacency entries are located so they can later be removed directly in O(1)
        Edge edge{u, v, std::move(edgeData), adjacencyIndexU, adjacencyIndexV};
        edgeSlots_[slotIndex].edge = std::move(edge);
        edgeSlots_[slotIndex].liveIndex = liveEdges_.size();

        liveEdges_.push_back(id);

        // add one adjacency entry to each endpoint, each storing the neighbour and the connecting EdgeId
        adjacencyU.push_back(Adjacency{v, id});
        adjacencyV.push_back(Adjacency{u, id});

        // store the normalized endpoint pair in the lookup map so the edge can later be found in average O(1)
        edgeLookup_.emplace(endpoints, id);

        return {id, true};
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::removeEdge(EdgeId id)
    {
        if (!edgeExists(id))
        {
            return false;
        }

        Edge &edge = *edgeSlots_[id.index].edge;
        // endpointU and endpointV are VertexIds, so their index members are the slot positions of the two connected vertices.
        // use these indices to access the adjacency lists of both endpoint vertices
        std::vector<Adjacency> &adjacencyU = (*vertexSlots_[edge.endpointU.index].vertex).adjacency;
        std::vector<Adjacency> &adjacencyV = (*vertexSlots_[edge.endpointV.index].vertex).adjacency;

        // the Edge directly stores where its entries are located in both adjacency lists, thus no search is required
        std::size_t adjacencyIndexU = edge.adjacencyIndexU;
        std::size_t adjacencyIndexV = edge.adjacencyIndexV;

        // remove this edge from endpointU's adjacency list without shifting all later entries, if the removed entry is not already last, move the last entry into its place
        // then update the moved edge so it knows its new adjacency position
        if (adjacencyIndexU != adjacencyU.size() - 1)
        {
            EdgeId movedId = adjacencyU.back().edge;
            adjacencyU[adjacencyIndexU] = std::move(adjacencyU.back());
            Edge &movedEdge = *edgeSlots_[movedId.index].edge;

            // determine whether endpointU of the removed edge is endpointU or endpointV of the moved edge, then update the corresponding stored adjacency index
            if (movedEdge.endpointU == edge.endpointU)
            {
                movedEdge.adjacencyIndexU = adjacencyIndexU;
            }
            else
            {
                movedEdge.adjacencyIndexV = adjacencyIndexU;
            }
        }
        adjacencyU.pop_back();

        // do the same for endpointV
        if (adjacencyIndexV != adjacencyV.size() - 1)
        {
            EdgeId movedId = adjacencyV.back().edge;
            adjacencyV[adjacencyIndexV] = std::move(adjacencyV.back());
            Edge &movedEdge = *edgeSlots_[movedId.index].edge;

            // same as above
            if (movedEdge.endpointU == edge.endpointV)
            {
                movedEdge.adjacencyIndexU = adjacencyIndexV;
            }
            else
            {
                movedEdge.adjacencyIndexV = adjacencyIndexV;
            }
        }
        adjacencyV.pop_back();

        // normalize the endpoint slot indices so the same EndpointPair used during addEdge can be removed from edgeLookup_
        std::size_t endpointIndexA = edge.endpointU.index;
        std::size_t endpointIndexB = edge.endpointV.index;

        if (endpointIndexA > endpointIndexB)
        {
            std::swap(endpointIndexA, endpointIndexB);
        }
        EndpointPair endpoints{endpointIndexA, endpointIndexB};

        edgeLookup_.erase(endpoints); // remove the edge from the average O(1) lookup map

        // get the position of this EdgeId inside liveEdges_
        std::size_t liveIndex = edgeSlots_[id.index].liveIndex;
        std::size_t lastLiveIndex = liveEdges_.size() - 1;

        // remove this EdgeId from liveEdges_ without shifting all later ID, if it is not already last, move the last EdgeId into its place
        // then update that moved edge's liveIndex so it still points to the correct position
        if (liveIndex != lastLiveIndex)
        {
            EdgeId movedId = liveEdges_.back();
            liveEdges_[liveIndex] = movedId;
            edgeSlots_[movedId.index].liveIndex = liveIndex;
        }
        liveEdges_.pop_back();

        edgeSlots_[id.index].edge.reset();  // destroy the Edge stored in its slot
        freeEdgeSlots_.push_back(id.index); // store the now empty slot so addEdge can reuse it later in the freeEdgeSlots

        return true;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::edgeCount() const
    {
        return liveEdges_.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::edgeExists(EdgeId id) const
    {
        // same idea as in vertexExists
        if (id.graphId != graphId_ || id.index >= edgeSlots_.size() || !edgeSlots_[id.index].edge.has_value() || id.generation != edgeSlots_[id.index].generation)
        {
            return false;
        }
        return true;
    }

    // hasEdge TODO!!!!!
    // findEdgeId TODO!!!!!
    // &edgeData TODO!!!!!
    // &edgeData TODO!!!!!
    // endpointU TODO!!!!!
    // endpointV TODO!!!!!
    // existingVertices TODO!!!!!
    // existingEdges TODO!!!!!
}