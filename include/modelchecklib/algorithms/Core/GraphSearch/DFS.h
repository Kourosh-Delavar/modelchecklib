#pragma once

#include <vector>

namespace modelchecklib::algorithms {

    template<typename Graph, typename Visitor>
    void depthFirstSearch(const Graph& graph,
                          const std::vector<typename Graph::StateId>& initial_states,
                          Visitor visitor) // visitor is a custom function to perform desired logics during traversal
    {
        using StateId = typename Graph::StateId;

        if (initial_states.empty() || graph.stateCount() == 0) return;

        std::vector<bool> visited(graph.stateCount(), false);

        std::vector<std::pair<StateId, StateId>> stack;

        for (StateId init_id : initial_states) {
            stack.push_back({init_id, init_id});
        }

        while (!stack.empty()) {
            auto [current, predecessor] = stack.back();
            stack.pop_back();

            if (visited[current]) {
                continue;
            } 
            visited[current] = true;

            if (!visitor(current, predecessor)) { // if visitor function returned false, stop (a violation found)
                return; 
            }

            for (const auto& trans : graph.getOutgoingTransitions(current)) {
                if (!visited[trans.to]) {
                    stack.push_back({trans.to, current});
                }
            }
        }
    }
}