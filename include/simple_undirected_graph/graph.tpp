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
                                                                              liveEdges_(sourceGraph.liveEdges_)
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
    }

    // replace this graph with an independent copy of the source graph
    template <typename VertexData, typename EdgeData, typename GraphData>
    Graph<VertexData, EdgeData, GraphData> &
    Graph<VertexData, EdgeData, GraphData>::operator=(const Graph &sourceGraph)
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
                                                                         liveEdges_(std::move(sourceGraph.liveEdges_))
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

        // create the VertexId
        VertexId id{graphId_, slotIndex, vertexSlots_[slotIndex].generation};
        // store the ID's position in the slot's liveIndex
        vertexSlots_[slotIndex].liveIndex = liveVertices_.size();
        // move the just created ID to the currently active vertices (rather than copying)
        liveVertices_.push_back(std::move(id));

        // since the ID was moved, access it at its new location
        return liveVertices_.back();
    }
}