#pragma once

#include "modelchecklib/algorithms/Core/GraphSearch/BFS.h"
#include <vector>
#include <algorithm>

namespace modelchecklib::algorithms {

    template<typename Graph>
    struct InvariantResult {
        bool holds;
        std::vector<typename Graph::StateId> counterexample_trace;
    };

    template<typename Graph>
    InvariantResult<Graph> checkInvariantForward(const Graph& graph, typename Graph::APId bad_ap) {
        using StateId = typename Graph::StateId;

        InvariantResult<Graph> result{ // default as holds (true)
            true,
            {}
        };

        if (graph.stateCount() == 0) return result;

        // parent_map tells us which state discovered the violation
        std::vector<StateId> parent_map(graph.stateCount(), Graph::INVALID_STATE);

        // visitor lambda function
        auto visitor = [&](StateId current, StateId predecessor) -> bool {
            if (parent_map[current] == Graph::INVALID_STATE) {
                parent_map[current] = predecessor;
            }

            if (graph.getState(current).hasAP(bad_ap)) {
                result.holds = false;

                StateId trace_node = current;
                while (trace_node != parent_map[trace_node]) {
                    result.counterexample_trace.push_back(trace_node);
                    trace_node = parent_map[trace_node];
                }
                result.counterexample_trace.push_back(trace_node); // add the initial state

                std::reverse(result.counterexample_trace.begin(), result.counterexample_trace.end());

                return false; // found a violation, stop the search
            }
            return true; // no violation found, continue the search 
        };

        auto forward_navigator = [](const Graph& g, StateId current, auto yield) {
            for (const auto& trans : g.getOutgoingTransitions(current)) {
                yield(trans.to); 
            }
        };

        breadthFirstSearch(graph, graph.getInitialStates(), visitor, forward_navigator);

        return result;
    }

    template<typename Graph>
    InvariantResult<Graph> checkInvariantBackward(const Graph&, typename Graph::APId bad_ap) {
        using StateId = typename Graph.StateId;

        InvariantResult<Graph> result{ // default as holds (true)
            true,
            {}
        };

        if (graph.stateCount() == 0) return result;

        // find all the states that violate the invariant
        std::vector<StateId> bad_states;
        for (StateId i = 0; i < graph.stateCount(); ++i) {
            if (graph.getState(i).hasAP(bad_ap)) {
                bad_states.push_back(i);
            }
        }

        if (bad_states.empty()) return result; // if no bad states found, return default result

        std::vector<StateId> parent_map(graph.stateCount(), Graph::INVALID_STATE);

        auto visitor = [&](StateId current, StateId predecessor) -> bool {
            if (parent_map[current] == Graph::INVALID_STATE) {
                parent_map[current] = predecessor;
            }

            if (graph.getState(current).isInitial()) {
                result.holds = false;
                
                StateId trace_node = current;
                while (trace_node != parent_map[trace_node]) {
                    result.counterexample_trace.push_back(trace_node);
                    trace_node = parent_map[trace_node];
                }
                result.counterexample_trace.push_back(trace_node); // add the initial state
                
                return false; // found a violation, stop the search
            }
            return true; // no violation found, continue the search
        };

        auto backward_navigator = [](const Graph& g, StateId current, auto yield) {
            for (const auto& trans : g.getIncomingTransitions(current)) {
                yield(trans.from); 
            }
        };

        breadthFirstSearch(graph, bad_states, visitor, backward_navigator);

        return result;
    }
}