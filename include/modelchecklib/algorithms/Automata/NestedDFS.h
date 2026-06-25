#pragma once 

#include <vector>
#include <algorithm>

namespace modelchecklib::algorithms {

    template<typename Automaton>
    struct Result {
        bool has_cycle;
        std::vector<typename Automaton::StateId> state_trace;
        std::vector<typename Automaton::StateId> cycle_trace;
    };

    template<typename Automaton>
    Result<Automaton> checkEmptiness(const Automaton& automaton) {
        using StateId = typename Automaton::StateId;

        Result<Automaton> result{
            false,
            {},
            {}
        };

        if (automaton.stateCount() == 0) return result;

        std::vector<bool> visited_outer(automaton.stateCount(), false);
        std::vector<bool> visited_inner(automaton.stateCount(), false);

        // stacks to keep track of the outer and inner search paths
        std::vector<StateId> outer_stack;
        std::vector<StateId> inner_stack;

        bool cycle_found = false;

        // inner search
        auto inner_dfs = [&](auto& self, StateId current, StateId target_seed) -> bool {
            if (cycle_found) return;

            visited_inner[current] = true;
            inner_stack.push_back(current);

            for (const auto& trans : automaton.getOutgoingTransitions(current)) {
                if (cycle_found) return;

                if (trans.to == target_seed) {
                    cycle_found = true;
                    inner_stack.push_back(trans.to);
                    return;
                }

                if (!visited_inner[trans.to]) {
                    self(self, trans.to, target_seed);
                }
            }

            if (!cycle_found) inner_stack.pop_back();
        };

        // outer search
        auto outer_dfs = [&](auto& self, StateId current) -> void {
            if (cycle_found) return;

            visited_outer[current] = true;
            outer_stack.push_back(current);

            for (const auto& trans : automaton.getOutgoingTransitions(current)) {
                if (cycle_found) return;

                if (!visited_outer[trans.to]) {
                    self(self, trans.to);
                }
            }

            if (!cycle_found && automaton.getState(current).isAccepting()) {
                inner_dfs(inner_dfs, current, current);

                if (cycle_found) {
                    result.has_cycle = true;
                    result.state_trace = outer_stack;
                    result.cycle_trace = inner_stack;
                }
            }

            if (!cycle_found) outer_stack.pop_back();
        };

        // start the engine
        for (StateId init_state : automaton.getInitialStates()) {
            if (!visited_outer[init_state]) {
                outer_dfs(outer_dfs, init_state);
                if (cycle_found) break; // stop if a violation found
            }
        }

        return result;
    }
}