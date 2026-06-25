#pragma once

#include "modelchecklib/graphs/StateSpace.h"
#include "modelchecklib/graphs/Transition.h"
#include <vector>

namespace modelchecklib::models {

    template<typename Model>
    class KripkeStructure : public graphs::StateSpace<Model, UnlabeledTrans<Model>> {
        public:
            using Base = graphs::StateSpace<Model, UnlabeledTrans<Model>>;
            using StateId = typename Model::StateId;
            using State = typename Base::State;

        private:
            std::vector<StateId> initial_states_;

        public:
            KripkeStructure() = default;
            virtual ~KripkeStructure() = default;

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