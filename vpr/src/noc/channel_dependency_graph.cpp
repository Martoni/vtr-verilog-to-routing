
#include "channel_dependency_graph.h"
#include "vtr_assert.h"

#include <stack>

ChannelDependencyGraph::ChannelDependencyGraph(size_t n_links,
                                               const vtr::vector<NocTrafficFlowId, std::vector<NocLinkId>>& traffic_flow_routes) {
    adjacency_list_.clear();
    // In channel dependency graph, vertices represent NoC links.
    // reserve enough space so that all vertices can store their outgoing neighbors
    adjacency_list_.resize(n_links);

    /*
     * A traffic flow travels through some NoC links. In channel dependency graph (CDG),
     * consecutive NoC links travelled by the flow are connected using an edge.
     * More specifically, for each pair of consecutive NoC links in a traffic flow route,
     * there exists a directed edge from the NoC link travelled first to the other one.
     * For example, if traffic flow T travels NoC links L0, L2, and L5 to reach its
     * destination, we need to add (L0, L2) and (L2, L5) edges to CDG.
     */

    // iterate over all traffic flows and populate the channel dependency graph
    for (const auto& traffic_flow_route : traffic_flow_routes) {
        auto prev_link_id = NocLinkId::INVALID();
        for (auto cur_link_id : traffic_flow_route) {
            VTR_ASSERT(prev_link_id != cur_link_id);
            if (prev_link_id != NocLinkId::INVALID()) {
                adjacency_list_[prev_link_id].push_back(cur_link_id);
            }
            prev_link_id = cur_link_id;
        }
    }
}

bool ChannelDependencyGraph::has_cycles() {
    // get the number vertices in CDG
    const size_t n_vertices = adjacency_list_.size();

    // indicates whether a node (NoC link) in CDG is visited during DFS
    vtr::vector<NocLinkId, bool> visited(n_vertices, false);
    // indicates whether a node (NoC links) is currently in stack
    vtr::vector<NocLinkId, bool> on_stack(n_vertices, false);
    // the stack used to perform graph traversal (DFS). Contains to-be-visited vertices
    std::stack<NocLinkId> stack;

    // iterate over all vertices (NoC links)
    for (NocLinkId noc_link_id : adjacency_list_.keys()) {
        // the node (NoC link) has already been visited
        if (visited[noc_link_id]) {
            continue;
        }

        // An un-visited node is found. Add to the stack
        stack.push(noc_link_id);

        // continue the traversal until the stack is empty
        while (!stack.empty()) {
            auto current_vertex_id = stack.top();

            if (!visited[current_vertex_id]) {
                on_stack[current_vertex_id] = true;
                visited[current_vertex_id] = true;
            } else { // the neighboring vertices have already been processed
                // remove it from the stack
                stack.pop();
                on_stack[current_vertex_id] = false;
            }

            // get the outgoing edges of the current vertex
            const auto& neighbor_ids = adjacency_list_[current_vertex_id];

            // iterate over all outgoing neighbors
            for (auto& neighbor_id : neighbor_ids) {
                if (!visited[neighbor_id]) {
                    stack.push(neighbor_id);
                } else if (on_stack[neighbor_id]) { // the current vertex is pointing to one of its ancestors
                    return true;
                }
            }
        }
    }

    // if no vertex in the graph points to at least one of its ancestors, the graph does not have any cycles
    return false;
}