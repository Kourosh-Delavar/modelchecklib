#pragma once

#include "modelchecklib/graphs/StateSpace.h"
#include "modelchecklib/graphs/Transition.h"
#include <vector>
#include <stdexcept>

namespace modelchecklib::automata {

    template<
        typename Model,
        typename TransType = LabeledTrans<Model>
    >
    class DFA : public graphs::StateSpace<Model, TransType>{
        public:
            using Base = graphs::StateSpace<Model, TransType>;
            using StateId = typename Model::StateId;
            using State = typename Base::State;

        private:
            StateId initial_state_ = Model::INVALID_STATE; // a DFA only has one initial state
            std::vector<StateId> accepting_states_;

        public:
            DFA() = default;
            virtual ~DFA() = default;

            void addState(const State& state) override {
                Base::addState(state);

                if (state.isInitial()) {
                    if (initial_state != Model::INVALID_STATE) {
                        throw std::logic_error("DFA: A Deterministic Finite Automaton cannot have multiple states.");
                    }
                    initial_state_ = state.id;
                }

                if (state.isAccepting()) {
                    accepting_states_.push_back(state);                
                }
            }

            void addTransition(const TransType& trans) override {
                // check for transitions with the same action label
                if (trans.from < this->states_.size()) {
                    for (const auto& existing_trans : this->outgoingTransitions_[trans.from]) {
                        if (existing_trans.action == trans.action) {
                            throw std::logic_error("DFA: Nondeterminism occured. State already has an outgoing transition for this action.");
                        }
                    }
                }

                Base::addTransition(trans);
            }

            // returns a single state (returned as a vector for API consistency)
            [[nodiscard]] std::vector<StateId> getInitialStates() const {
                if (initial_state_ == Model::INVALID_STATE) return {};
                return {initial_state_};
            }

            [[nodiscard]] const std::vector<StateId>& getAcceptingStates() const noexcept {
                return accepting_states_;
            }

            void clear() noexcept override {
                Base::clear();
                initial_state_ = Model::INVALID_STATE;
                accepting_states_.clear();
            }
    };
}