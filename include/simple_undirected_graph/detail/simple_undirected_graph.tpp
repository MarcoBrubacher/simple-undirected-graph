#include <utility>
#include <limits>
#include <stdexcept>

namespace simple_undirected_graph
{
    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::VertexId::VertexId(std::size_t graphId, std::size_t index, std::size_t generation) : graphId(graphId),
                                                                                                                                 index(index),
                                                                                                                                 generation(generation) {}

    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::EdgeId::EdgeId(std::size_t graphId, std::size_t index, std::size_t generation) : graphId(graphId),
                                                                                                                             index(index),
                                                                                                                             generation(generation) {}
    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::VertexIdHash::operator()(const VertexId &id) const noexcept
    {
        std::size_t seed = 0;

        seed = Graph::combineHash(seed, id.graphId);
        seed = Graph::combineHash(seed, id.index);
        seed = Graph::combineHash(seed, id.generation);

        return seed;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::EdgeIdHash::operator()(const EdgeId &id) const noexcept
    {
        std::size_t seed = 0;

        seed = Graph::combineHash(seed, id.graphId);
        seed = Graph::combineHash(seed, id.index);
        seed = Graph::combineHash(seed, id.generation);

        return seed;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::Graph() : graphId_(nextGraphId_++) {}

    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::Graph(GraphData graphData) : graphId_(nextGraphId_++), graphData_(std::move(graphData)) {}

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

        for (typename std::unordered_map<EndpointPair, EdgeId, EndpointPairHasher>::value_type &entry : edgeLookup_)
        {
            entry.second.graphId = graphId_;
        }
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData> &Graph<VertexData, EdgeData, GraphData>::operator=(const Graph &sourceGraph)
    {
        if (this == &sourceGraph)
        {
            return *this;
        }

        // use the copy constructor so the copied graph gets a new graph ID and all IDs inside the copied storage are updated correctly
        Graph copiedGraph(sourceGraph);

        *this = std::move(copiedGraph);
        return *this;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData>::Graph(Graph &&sourceGraph) noexcept(std::is_nothrow_move_constructible_v<GraphData>) : graphId_(sourceGraph.graphId_),
                                                                                                                                   graphData_(std::move(sourceGraph.graphData_)),
                                                                                                                                   vertexSlots_(std::move(sourceGraph.vertexSlots_)),
                                                                                                                                   edgeSlots_(std::move(sourceGraph.edgeSlots_)),
                                                                                                                                   freeVertexSlots_(std::move(sourceGraph.freeVertexSlots_)),
                                                                                                                                   freeEdgeSlots_(std::move(sourceGraph.freeEdgeSlots_)),
                                                                                                                                   liveVertices_(std::move(sourceGraph.liveVertices_)),
                                                                                                                                   liveEdges_(std::move(sourceGraph.liveEdges_)),
                                                                                                                                   edgeLookup_(std::move(sourceGraph.edgeLookup_))
    {
        // leave the moved-from graph empty and give it a fresh graph ID
        sourceGraph.clear();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData> &Graph<VertexData, EdgeData, GraphData>::operator=(Graph &&sourceGraph) noexcept(std::is_nothrow_move_assignable_v<GraphData>)
    {
        if (this == &sourceGraph)
        {
            return *this;
        }

        std::size_t sourceGraphId = sourceGraph.graphId_;

        // do potentially throwing work before changing graph identity
        graphData_ = std::move(sourceGraph.graphData_);

        vertexSlots_ = std::move(sourceGraph.vertexSlots_);
        edgeSlots_ = std::move(sourceGraph.edgeSlots_);

        freeVertexSlots_ = std::move(sourceGraph.freeVertexSlots_);
        freeEdgeSlots_ = std::move(sourceGraph.freeEdgeSlots_);

        liveVertices_ = std::move(sourceGraph.liveVertices_);
        liveEdges_ = std::move(sourceGraph.liveEdges_);

        edgeLookup_ = std::move(sourceGraph.edgeLookup_);

        graphId_ = sourceGraphId;

        // leave the moved-from graph empty and give it a fresh graph ID
        sourceGraph.clear();

        return *this;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    GraphData &Graph<VertexData, EdgeData, GraphData>::graphData() noexcept
    {
        return graphData_;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    const GraphData &Graph<VertexData, EdgeData, GraphData>::graphData() const noexcept
    {
        return graphData_;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    void Graph<VertexData, EdgeData, GraphData>::clear() noexcept
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
    bool Graph<VertexData, EdgeData, GraphData>::empty() const noexcept
    {
        return liveVertices_.empty();
    }

    // allows addVertex() for graphs whose vertex data type is NoProperties
    template <typename VertexData, typename EdgeData, typename GraphData>
    typename Graph<VertexData, EdgeData, GraphData>::VertexId Graph<VertexData, EdgeData, GraphData>::addVertex()
        requires std::same_as<VertexData, NoProperties>
    {
        return addVertex(NoProperties{});
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    typename Graph<VertexData, EdgeData, GraphData>::VertexId Graph<VertexData, EdgeData, GraphData>::addVertex(VertexData vertexData)
    {
        // determine which vertex slot will be used, but do not commit reuse yet
        const bool reusingSlot = !freeVertexSlots_.empty();

        std::size_t slotIndex;
        std::size_t generation;

        if (reusingSlot)
        {
            slotIndex = freeVertexSlots_.back();
            // a slot was reused, thus the generation of the VertexSlot must increase (but first check for overflow)
            if (vertexSlots_[slotIndex].generation == std::numeric_limits<std::size_t>::max())
            {
                throw std::overflow_error("vertex generation exhausted");
            }
            generation = vertexSlots_[slotIndex].generation + 1;
        }
        else
        {
            slotIndex = vertexSlots_.size();
            vertexSlots_.emplace_back();
            generation = vertexSlots_[slotIndex].generation;
        }

        VertexId id{graphId_, slotIndex, generation};

        // remember where the new VertexId will be appended so partial changes can be undone and liveIndex can later be set directly
        const std::size_t oldLiveVerticesSize = liveVertices_.size();

        try
        {
            vertexSlots_[slotIndex].vertex.emplace(std::move(vertexData));
            liveVertices_.push_back(id);
        }
        catch (...)
        {
            // undo all entries that were successfully completed to perserve the original graph state
            if (liveVertices_.size() > oldLiveVerticesSize)
            {
                liveVertices_.pop_back();
            }
            vertexSlots_[slotIndex].vertex.reset();
            if (!reusingSlot)
            {
                vertexSlots_.pop_back();
            }

            throw;
        }

        // everything succeeded, so store the final live position and commit the slot reuse
        vertexSlots_[slotIndex].liveIndex = oldLiveVerticesSize;
        if (reusingSlot)
        {
            vertexSlots_[slotIndex].generation = generation;
            freeVertexSlots_.pop_back();
        }

        return liveVertices_.back();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::removeVertex(VertexId id)
    {
        if (!vertexExists(id))
        {
            return false;
        }

        // removeEdge() removes the edge from both endpoints, so repeatedly removing adjacency.back() eventually isolates this vertex.
        while (!(*vertexSlots_[id.index].vertex).adjacency.empty())
        {
            removeEdge((*vertexSlots_[id.index].vertex).adjacency.back().edge);
        }

        std::size_t liveIndex = vertexSlots_[id.index].liveIndex;
        std::size_t lastLiveIndex = liveVertices_.size() - 1;

        // Erasing from the middle of liveVertices_ would shift later elements and make removal O(n).
        // Since iteration order is not significant, swap-and-pop is applied instead: (removes an element in O(1) by copying the last element over it and then removing the now-duplicate last element)
        if (liveIndex != lastLiveIndex)
        {
            VertexId movedId = liveVertices_.back();
            liveVertices_[liveIndex] = movedId;
            // If another ID is moved into this position, its stored liveIndex must be updated.
            vertexSlots_[movedId.index].liveIndex = liveIndex;
        }
        liveVertices_.pop_back();

        vertexSlots_[id.index].vertex.reset();
        freeVertexSlots_.push_back(id.index);

        return true;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::vertexCount() const noexcept
    {
        return liveVertices_.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::vertexExists(VertexId id) const noexcept
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
    std::span<const typename Graph<VertexData, EdgeData, GraphData>::Adjacency> Graph<VertexData, EdgeData, GraphData>::adjacency(VertexId id) const
    {
        if (!vertexExists(id))
        {
            throw std::invalid_argument("vertex does not exist");
        }

        return std::span<const Adjacency>{(*vertexSlots_[id.index].vertex).adjacency};
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

    // if edge data type is no properties, this allows edgeAdd(u,v)
    template <typename VertexData, typename EdgeData, typename GraphData>
    std::pair<typename Graph<VertexData, EdgeData, GraphData>::EdgeId, bool> Graph<VertexData, EdgeData, GraphData>::addEdge(VertexId u, VertexId v)
        requires std::same_as<EdgeData, NoProperties>
    {
        return addEdge(u, v, NoProperties{});
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::pair<typename Graph<VertexData, EdgeData, GraphData>::EdgeId, bool> Graph<VertexData, EdgeData, GraphData>::addEdge(VertexId u, VertexId v, const EdgeData &edgeData)
    {
        return insertEdge(u, v, edgeData);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::pair<typename Graph<VertexData, EdgeData, GraphData>::EdgeId, bool> Graph<VertexData, EdgeData, GraphData>::addEdge(VertexId u, VertexId v, EdgeData &&edgeData)
    {
        return insertEdge(u, v, std::move(edgeData));
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    template <typename T>
    std::pair<typename Graph<VertexData, EdgeData, GraphData>::EdgeId, bool> Graph<VertexData, EdgeData, GraphData>::insertEdge(VertexId u, VertexId v, T &&edgeData)
    {
        if (!vertexExists(u) || !vertexExists(v))
        {
            throw std::invalid_argument("vertex does not exist");
        }

        if (u == v)
        {
            throw std::invalid_argument("self-loops are not allowed");
        }

        EndpointPair endpoints = normalizeEndpoints(u, v);
        typename std::unordered_map<EndpointPair, EdgeId, EndpointPairHasher>::iterator existingEdge = edgeLookup_.find(endpoints);

        if (existingEdge != edgeLookup_.end())
        {
            return {existingEdge->second, false};
        }

        std::vector<Adjacency> &adjacencyU = (*vertexSlots_[u.index].vertex).adjacency;
        std::vector<Adjacency> &adjacencyV = (*vertexSlots_[v.index].vertex).adjacency;

        std::size_t adjacencyIndexU = adjacencyU.size();
        std::size_t adjacencyIndexV = adjacencyV.size();

        // construct the edge before changing the graph, if moving EdgeData throws here, no graph state has changed yet
        Edge edge{u, v, std::forward<T>(edgeData), adjacencyIndexU, adjacencyIndexV};

        // determine which edge slot will be used, but do not commit reuse yet
        bool reusingSlot = !freeEdgeSlots_.empty();

        std::size_t slotIndex;
        std::size_t generation;

        if (reusingSlot)
        {
            slotIndex = freeEdgeSlots_.back();

            // prevent the generation from wrapping around and making a very old stale ID valid again
            if (edgeSlots_[slotIndex].generation == std::numeric_limits<std::size_t>::max())
            {
                throw std::overflow_error("edge generation exhausted");
            }

            generation = edgeSlots_[slotIndex].generation + 1;
        }
        else
        {
            slotIndex = edgeSlots_.size();
            edgeSlots_.emplace_back();
            generation = edgeSlots_[slotIndex].generation;
        }

        EdgeId id{graphId_, slotIndex, generation};

        // remember where the new EdgeId will be appended so partial changes can be undone and liveIndex can later be set directly
        std::size_t oldLiveEdgesSize = liveEdges_.size();

        try
        {
            edgeSlots_[slotIndex].edge = std::move(edge);
            liveEdges_.push_back(id);

            adjacencyU.push_back(Adjacency{v, id});
            adjacencyV.push_back(Adjacency{u, id});

            std::pair<typename std::unordered_map<EndpointPair, EdgeId, EndpointPairHasher>::iterator, bool> insertionResult = edgeLookup_.emplace(endpoints, id);

            // the edge was checked before, so reaching this means the internal lookup state is inconsistent
            if (!insertionResult.second)
            {
                throw std::logic_error("edge lookup insertion failed");
            }
        }
        catch (...)
        {
            // undo all entries that were successfully completed
            if (adjacencyV.size() > adjacencyIndexV)
            {
                adjacencyV.pop_back();
            }
            if (adjacencyU.size() > adjacencyIndexU)
            {
                adjacencyU.pop_back();
            }
            if (liveEdges_.size() > oldLiveEdgesSize)
            {
                liveEdges_.pop_back();
            }

            edgeSlots_[slotIndex].edge.reset();

            if (!reusingSlot)
            {
                edgeSlots_.pop_back();
            }
            throw;
        }

        // everything succeeded, so store the final live position and commit the slot reuse
        edgeSlots_[slotIndex].liveIndex = oldLiveEdgesSize;

        if (reusingSlot)
        {
            edgeSlots_[slotIndex].generation = generation;
            freeEdgeSlots_.pop_back();
        }

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

        if (adjacencyIndexV != adjacencyV.size() - 1)
        {
            EdgeId movedId = adjacencyV.back().edge;
            adjacencyV[adjacencyIndexV] = std::move(adjacencyV.back());
            Edge &movedEdge = *edgeSlots_[movedId.index].edge;

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

        edgeLookup_.erase(normalizeEndpoints(edge.endpointU, edge.endpointV));

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

        edgeSlots_[id.index].edge.reset();
        freeEdgeSlots_.push_back(id.index);

        return true;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::edgeCount() const noexcept
    {
        return liveEdges_.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::edgeExists(EdgeId id) const noexcept
    {
        if (id.graphId != graphId_ || id.index >= edgeSlots_.size() || !edgeSlots_[id.index].edge.has_value() || id.generation != edgeSlots_[id.index].generation)
        {
            return false;
        }
        return true;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool Graph<VertexData, EdgeData, GraphData>::hasEdge(VertexId u, VertexId v) const
    {
        if (!vertexExists(u) || !vertexExists(v))
        {
            throw std::invalid_argument("vertex does not exist");
        }
        return edgeLookup_.find(normalizeEndpoints(u, v)) != edgeLookup_.end();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::optional<typename Graph<VertexData, EdgeData, GraphData>::EdgeId>
    Graph<VertexData, EdgeData, GraphData>::findEdgeId(VertexId u, VertexId v) const
    {
        if (!vertexExists(u) || !vertexExists(v))
        {
            throw std::invalid_argument("vertex does not exist");
        }

        typename std::unordered_map<EndpointPair, EdgeId, EndpointPairHasher>::const_iterator entry = edgeLookup_.find(normalizeEndpoints(u, v));
        if (entry == edgeLookup_.end())
        {
            return std::nullopt;
        }
        return (*entry).second;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    EdgeData &Graph<VertexData, EdgeData, GraphData>::edgeData(EdgeId id)
    {
        if (!edgeExists(id))
        {
            throw std::invalid_argument("edge does not exist");
        }

        return (*edgeSlots_[id.index].edge).data;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    const EdgeData &Graph<VertexData, EdgeData, GraphData>::edgeData(EdgeId id) const
    {
        if (!edgeExists(id))
        {
            throw std::invalid_argument("edge does not exist");
        }

        return (*edgeSlots_[id.index].edge).data;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::pair<typename Graph<VertexData, EdgeData, GraphData>::VertexId, typename Graph<VertexData, EdgeData, GraphData>::VertexId> Graph<VertexData, EdgeData, GraphData>::endpoints(EdgeId id) const
    {
        if (!edgeExists(id))
        {
            throw std::invalid_argument("edge does not exist");
        }
        const Edge &edge = *edgeSlots_[id.index].edge;

        return {edge.endpointU, edge.endpointV};
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const typename Graph<VertexData, EdgeData, GraphData>::VertexId> Graph<VertexData, EdgeData, GraphData>::vertices() const noexcept
    {
        return liveVertices_;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const typename Graph<VertexData, EdgeData, GraphData>::EdgeId> Graph<VertexData, EdgeData, GraphData>::edges() const noexcept
    {
        return liveEdges_;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    typename Graph<VertexData, EdgeData, GraphData>::EndpointPair Graph<VertexData, EdgeData, GraphData>::normalizeEndpoints(VertexId u, VertexId v) const noexcept
    {
        std::size_t endpointIndexA = u.index;
        std::size_t endpointIndexB = v.index;

        if (endpointIndexA > endpointIndexB)
        {
            std::swap(endpointIndexA, endpointIndexB);
        }

        return {endpointIndexA, endpointIndexB};
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::combineHash(std::size_t seed, std::size_t value) noexcept
    {
        // simple baseline mixer. TODO: replace after benchmarking hash strategies
        constexpr std::size_t bitCount = sizeof(std::size_t) * 8;

        value ^= value >> (bitCount / 2);

        seed ^= value;
        seed += (seed << 5) + (seed >> 3) + 1;

        return seed;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t Graph<VertexData, EdgeData, GraphData>::EndpointPairHasher::operator()(const EndpointPair &pair) const noexcept
    {
        std::size_t seed = 0;

        seed = Graph::combineHash(seed, pair.first);
        seed = Graph::combineHash(seed, pair.second);

        return seed;
    }
}