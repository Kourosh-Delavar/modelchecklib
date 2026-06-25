#pragma once

#include "modelchecklib/graphs/StateSpace.h"
#include "modelchecklib/graphs/Transition.h"
#include <vector>

namespace modelchecklib::models {

    template<typename Model>
    class TransitionSystem : public graphs::StateSpace<Model, LabeledTrans<Model>> {
        public:
            using Base = graphs::StateSpace<Model, LabeledTrans<Model>>;
            using StateId = typename Model::StateId;
            using State = typename Base::State;
            using ActionId = typename Model::ActionId;

        private:
            std::vector<StateId> initial_states_;

        public:
            TransitionSystem() = default;
            virtual ~TransitionSystem() = default;

            void addState(const State& state) override {
                Base::addState(state);

                if (state.isInitial()) {
                    initial_states_.push_back(state.id);
                }
            }

            [[nodiscard]] const std::vector<StateId>& getInitialStates() const noexcept {
                return initial_states_;
            }

            void clear() noexcept override {
                Base::clear();
                initial_states_.clear();
            }
    };
}