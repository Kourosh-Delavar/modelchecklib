#pragma once

#include <vector>

namespace modelchecklib::algorithms {

    template<typename Graph, typename Visitor>
    void breadthFirstSearch(const Graph& graph,
                          const std::vector<typename Graph::StateId>& initial_states,
                          Visitor visitor) // visitor is a custom function to perform desired logics during traversal
    {
        using StateId = typename Graph::StateId;

        if (initial_states.empty() || graph.stateCount() == 0) return;

        std::vector<bool> visited(graph.stateCount(), false);

        std::vector<std::pair<StateId, StateId>> queue;

        std::size_t head = 0; // the front of the queue

        for (StateId init_id : initial_states) {
            queue.push_back({init_id, init_id});
            visited[init_id]  = true;
        } 

        while (head < queue.size()) {
            auto [current, predecessor] = queue[head]; // pull from the front
            head++; // update the head instead of erasing memory

            if (!visitor(current, predecessor)) return;  // if visitor function returned false, stop (a violation found)

            for (const auto& trans : graph.getOutgoingTransitions(current)) {
                if (!visited[trans.to]) {
                    visited[trans.to] = true;
                    queue.push_back({trans.to, current}); // push unvisited successors to the back of the queue
                }
            }
        }    
    }
}