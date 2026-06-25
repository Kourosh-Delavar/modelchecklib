#pragma once

#include "modelchecklib/graphs/StateSpace.h"
#include "modelchecklib/graphs/Transition.h"
#include <vector>

namespace modelchecklib::automata {
    
    template<
        typename Model,
        typename TransType = LabeledTrans<Model>
    >
    class BuchiAutomaton : public graphs::StateSpace<Model, TransType> {
        public:
            using Base = graphs::StateSpace<Model, TransType>;
            using StateId = typename Model::StateId;
            using State = typename Base::State;

        private:
            std::vector<StateId> initial_states_;
            std::vector<StateId> accepting_states_;

        public:
            BuchiAutomaton() = default;
            virtual ~BuchiAutomaton() = default;

            void addState(const State& state) override {
                Base::addState(state);

                if (state.isInitial()) {
                    initial_states_.push_back(state);
                }

                if (state.isAccepting()) {
                    accepting_states_.push_back(state);
                }
            }

            [[nodiscard]] const std::vector<StateId>& getInitialStates() const noexcept {
                return initial_states_;
            }

            [[nodiscard]] const std::vector<StateId>& getAcceptingStates() const noexcept {
                return accepting_states_;
            }

            void clear() noexcept override {
                Base::clear();
                initial_states_.clear();
                accepting_states_.clear();
            }
    };
}