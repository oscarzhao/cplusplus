#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

using std::vector;
using std::queue;
using std::numeric_limits;

/* This class implements a bit unusual scheme for storing edges of the graph,
 * in order to retrieve the backward edge for a given edge quickly. */
class FlowGraph {
public:
    struct Edge {
        int from, to, capacity, flow;
    };

private:
    /* List of all - forward and backward - edges */
    vector<Edge> edges;

    /* These adjacency lists store only indices of edges in the edges list */
    vector<vector<size_t> > graph;

public:
    explicit FlowGraph(size_t n): graph(n) {}

    void add_edge(int from, int to, int capacity) {
        /* Note that we first append a forward edge and then a backward edge,
         * so all forward edges are stored at even indices (starting from 0),
         * whereas backward edges are stored at odd indices in the list edges */
        Edge forward_edge = {from, to, capacity, 0};
        Edge backward_edge = {to, from, 0, 0};
        graph[from].push_back(edges.size());
        edges.push_back(forward_edge);
        graph[to].push_back(edges.size());
        edges.push_back(backward_edge);
    }

    size_t size() const {
        return graph.size();
    }

    const vector<size_t>& get_ids(int from) const {
        return graph[from];
    }

    const Edge& get_edge(size_t id) const {
        return edges[id];
    }

    void add_flow(size_t id, int flow) {
        /* To get a backward edge for a true forward edge (i.e id is even), we should get id + 1
         * due to the described above scheme. On the other hand, when we have to get a "backward"
         * edge for a backward edge (i.e. get a forward edge for backward - id is odd), id - 1
         * should be taken.
         *
         * It turns out that id ^ 1 works for both cases. Think this through! */
        edges[id].flow += flow;
        edges[id ^ 1].flow -= flow;
    }
    void update_capacity(size_t id, int flow) {
        edges[id].capacity -= flow;
        edges[id ^ 1].capacity += flow;
    }
};

FlowGraph read_data() {
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    FlowGraph graph(vertex_count);
    for (int i = 0; i < edge_count; ++i) {
        int u, v, capacity;
        std::cin >> u >> v >> capacity;
        graph.add_edge(u - 1, v - 1, capacity);
    }
    return graph;
}

// dfs do a breadth-first search to FlowGraph graph, from vertex "from", to vertex
// "to", if found vertext to, return flow
int bfs(FlowGraph& graph, vector<size_t>& paths, int from, int to) {
    queue<size_t> vertexIDs;
    vector<bool> visited(graph.size(), false);
    vector<int> parentEdges(graph.size(), -1);
    vertexIDs.push(from);
    visited[from] = true;
    bool found = false;
    while(!vertexIDs.empty() && !found) {
        size_t vertexID = vertexIDs.front();
        vertexIDs.pop();
        const vector<size_t> edgeIDs = graph.get_ids(vertexID);
        for(vector<size_t>::const_iterator it = edgeIDs.begin(); it != edgeIDs.end(); it++) {
            const FlowGraph::Edge& ed = graph.get_edge(*it);
            if (ed.capacity <= 0 || visited[ed.to]) continue;
            visited[ed.to] = true;
            parentEdges[ed.to] = *it;  // set parentEdges[v] as edge ID which lead to it
            if (ed.to == to) {
                found = true;
                break;
            }
            vertexIDs.push(ed.to);
        }
    }

    if (!found) return 0; // cannot find a valid flow
    int flow = numeric_limits<int>::max();
    while(to != from) {
        size_t edgeID = parentEdges[to];
        const FlowGraph::Edge& ed = graph.get_edge(edgeID);
        if (flow > ed.capacity) flow = ed.capacity;
        paths.push_back(edgeID);
        to = ed.from;
        // std::cout << "from:" << from << ", to:" << to << ", edge:" << edgeID << ", edge.from:" << ed.from << ", edge.to:"<<ed.to <<  std::endl;
    }
    
    return flow;
}

int max_flow(FlowGraph& graph, int from, int to) {
    int flow = 0;

    while(true){
        // calculate this flow
        vector<size_t> paths;
        int thisFlow = bfs(graph, paths, from, to);
        if (thisFlow == 0) return flow;
        flow+= thisFlow;
        for(vector<size_t>::const_iterator it = paths.begin(); it != paths.end(); it++) {
            graph.update_capacity(*it, thisFlow);
        }
    }

    return flow;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    FlowGraph graph = read_data();

    std::cout << max_flow(graph, 0, graph.size() - 1) << "\n";
    return 0;
}
