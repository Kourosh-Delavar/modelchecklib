#pragma once

#include "modelchecklib/core/Types.h"
#include <functional>

namespace modelchecklib {

    template<typename Model>
    struct UnlabeledTrans {
        using StateId = typename Model::StateId;

        StateId from;
        StateId to;

        constexpr UnlabeledTrans() noexcept
            : from(Model::INVALID_STATE), 
              to(Model::INVALID_STATE) {}
        
        constexpr UnlabeledTrans(StateId from, StateId to) noexcept
            : from(from), 
              to(to) {}

        // UnlabeledTrans operators
        [[nodiscard]] constexpr bool operator==(const UnlabeledTrans& t) const noexcept {
            return from == t.from && to == t.to;
        }

        [[nodiscard]] constexpr bool operator!=(const UnlabeledTrans& t) const noexcept {
            return from != t.from || (from == t.from && to != t.to);
        }
    };

    template<typename Model>
    struct LabeledTrans {
        using StateId = typename Model::StateId;
        using ActionId = typename Model::ActionId;

        StateId from;
        StateId to;
        ActionId action;

        constexpr LabeledTrans() noexcept
            : from(Model::INVALID_STATE), 
              to(Model::INVALID_STATE),
              action(Model::INVALID_ACTION) {}

        constexpr LabeledTrans(StateId from, StateId to, ActionId action) noexcept
            : from(from), 
              to(to), 
              action(action) {}

        // LabeledTrans operators
        [[nodiscard]] constexpr bool operator==(const LabeledTrans& t) const noexcept {
            return from == t.from && to == t.to && action == t.action;
        }
        [[nodiscard]] constexpr bool operator!=(const LabeledTrans& t) const noexcept {
            return !(*this == t);
        }
    };

    template<typename Model>
    struct ProbTrans {
        using StateId = typename Model::StateId;
        using ActionId = typename Model::ActionId;
        using Prob = uint8_t;  // using fixed-point integer representation? to be asked later // fraction

        StateId from;
        StateId to;
        ActionId action;
        Prob prob;

        constexpr ProbTrans() noexcept {
            : from(Model::INVALID_STATE),
              to(Model::INVALID_STATE),
              action(Model::INVALID_ACTION),
              prob(0) {}
        }

        constexpr ProbTrans(StateId from, StateId to, ActionId action, Prob prob) noexcept
            : from(from),
              to(to),
              action(action),
              prob(prob) {}

        // ProbTrans operators
        [[nodiscard]] constexpr bool operator==(const ProbTrans& t) const noexcept {
            return from == t.from && to == t.to && action == t.action && prob == t.prob;
        }
        [[nodiscard]] constexpr bool operator!=(const ProbTrans& t) const noexcept {
            return !(*this == t);
        }
    };
}