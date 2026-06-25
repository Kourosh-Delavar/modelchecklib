#pragma once

#include <vector>
#include <map>
#include <queue>
#include <utility>

namespace modelchecklib::algorithms {

    class SynchronousProductBuilder {
        public:
            /**
             * @brief Multiplies a System and Automaton together on their matching actions.
             * 
             * @tparam System The system model
             * @tparam Automaton The property automaton 
             * @tparam Product The type of Automaton to output
             * 
             * @param sys The system to verify 
             * @param aut The property automaton that specifies the rules
             * @return Product The composed product graph
             */
            template<
                typename System,
                typename Automaton,
                typename Product
            >
            static Product build(const System& sys, const Automaton& aut) {
                Product product_graph;

                using StateId = typename System::StateId;
                using PairState = std::pair<StateId, StateId>; // (SystemState, AutomatonState)

                // maps the pair to the new ID in our graph
                std::map<PairState, StateId> state_map;
                std::queue<PairState> worklist;

                // id generator 
                StateId next_product_id = 0;

                // pair up all initial states
                for (StateId s_init : sys.getInitialStates()) {
                    for (StateId a_init : aut.getInitialStates()) {
                        PairState start_pair = {s_init, a_init};

                        // create the composed state
                        typename Product::State new_state;
                        new_state.id = next_product_id++;
                        new_state.setInitial(True);
                        
                        // the product state is accepting if the automaton state is accepting 
                        if (aut.getState(a_init).isAccepting()) {
                            new_state.setAccepting(True);
                        }

                        // add the state to the product graph
                        product_graph.addState(new_state);

                        // push the next work
                        state_map[start_pair] = new_state.id;
                        worklist.push(start_pair);
                    }
                }

                // build the rest of the graph
                while (!worklist.empty()) {
                    PairState current = worklist.front();
                    worklist.pop();

                    StateId current_product_id = state_map[current];
                    StateId sys_state = current.first;
                    StateId aut_state = current.second;

                    // find all possible paths the system can take
                    for (const auto& sys_trans : sys.getOutgoingTransitions(sys_state)) {

                        // find all possible paths the automaton can take
                        for (const auto& aut_trans : aut.getOutgoingTransitions(aut_state)) {

                            // synchronization (they must be match on the action)
                            if (sys_trans.action == aut_trans.action) {
                                PairState next_pair = {sys_trans.to, aut_trans.to};

                                // if this composed state is not seen before, create it
                                if (state_map.find(next_pair) == state_map.end()) {
                                    // create the composed state
                                    typename Product::State new_state;
                                    new_state.id = next_product_id++;

                                    if (aut.getState(next_pair.second).isAccepting()) {
                                        new_state.setAccepting(true);
                                    }

                                    // add the state to the product graph
                                    product_graph.addState(new_state);

                                    // push the next work
                                    state_map[next_pair] = new_state.id;
                                    worklist.push(next_pair);
                                }

                                // create the edges in the product graph
                                typename Product::TransType product_trans;
                                product_trans.from = current_product_id;
                                product_trans.to = state_map[next_pair];
                                product_trans.action = sys_trans.action;

                                product_graph.addTransition(product_trans);
                            }
                        }
                    }
                }

                return product_graph;
            }
    };
}