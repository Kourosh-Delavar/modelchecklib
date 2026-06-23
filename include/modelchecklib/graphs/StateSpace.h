#pragma once

#include "modelchecklib/core/Types.h"
#include "modelchecklib/graphs/State.h"
#include "modelchecklib/graphs/Transition.h"

#include <vector>
#include <stdexcept>
#include <cstdint>

namespace modelchecklib {

    template<
        typename Model,
        typename TransType = UnlabeledTrans<Model>
    >
    class StateSpace {
        public:
            using StateId = typename Model::StateId;
            using State = StateNode<Model>;

        protected:
            std::vector<State> states_; // state ID is also the index of the state object in the vector
            std::vector<std::vector<TransType>> outgoingTransitions_;
            std::vector<std::vector<TransType>> incomingTransitions_; // TODO: remove this as it might be redundant

        public:
            StateSpace() = default;
            virtual ~StateSpace() = default;
        
        // optional to use
        void reserve(std::size_t capacity) {
            states_.reserve(capacity);
            outgoingTransitions_.reserve(capacity);
            incomingTransitions_.reserve(capacity);
        }

        virtual void addState(const State& state) {
            if (state.id != states_.size()) {
                throw std::invalid_argument("StateSpace: StateId must be added sequentially");
            }

            states_.push_back(state);
            outgoingTransitions_.emplace_back();
            incomingTransitions_.emplace_back();
        }

        virtual void addTransition(const TransType& trans) {
            if (trans.from >= states_.size() || trans.to >= states_.size()) {
                throw std::out_of_range("StateSpace: Transition references non-existent StateId.");
            }

            outgoingTransitions_[trans.from].push_back(trans);
            incomingTransitions_[trans.to].push_back(trans);
        }

        [[nodiscard]] inline bool hasState(StateId id) const noexcept {
            return id < states_.size();
        }

        [[nodiscard]] inline const State& getState(StateId id) const noexcept {
            return states_[id];
        }

        // get mutable reference 
        [[nodiscard]] inline State& getStateMut(StateId id) noexcept {
            return states_[id];
        }

        [[nodiscard]] inline const std::vector<TransType>& getOutgoingTransitions(StateId id) const noexcept {
            return outgoingTransitions_[id];
        }

        [[nodiscard]] inline const std::vector<TransType>& getIncomingTransitions(StateId id) const noexcept {
            return incomingTransitions_[id];
        }

        [[nodiscard]] inline std::size_t stateCount() const noexcept {
            return states_.size();
        }

        virtual void clear() noexcept {
            states_.clear();
            outgoingTransitions_.clear();
            incomingTransitions_.clear();
        }
    };
}