#pragma one

#include "modelchecklib/core/Types.h"
#include <functional>

namespace modelchecklib {

    struct UnlabeledTrans {
        StateId from;
        StateId to;

        constexpr UnlabeledTrans() noexcept
            : from(INVALID_STATE), to(INVALID_STATE) {}
        
        constexpr UnlabeledTrans(StateId from, StateId to) noexcept
            : from(from), to(to) {}

        [[nodiscard]] constexpr bool operator== (const UnlabeledTrans& ut) const noexcept {
            return from == ut.from && to == ut.to;
        }

        [[nodiscard]] constexpr bool operator!= (const UnlabeledTrans& ut) const noexcept {
            return from != ut.from || (from == ut.from && to != ut.to);
        }

        [[nodiscard]] constexpr bool operator< (const UnlabeledTrans& ut) const noexcept {
            return from < ut.from || (from == ut.from && to < ut.to);
        }
    };

    struct LabeledTrans {
        StateId from;
        StateId to;
        ActionId action;
        uint16_t _pad;

        constexpr LabeledTrans() noexcept
            : from(INVALID_STATE), to(INVALID_STATE), action(INVALID_ACTION), _pad{0} {}

        constexpr LabeledTrans(StateId from, StateId to, ActionId action, uint16_t _pad) noexcept
            : from(from), to(to), action(action), _pad{0} {}

        [[nodiscard]] constexpr bool operator== (const LabeledTrans& ut) const noexcept {
            return from == ut.from && to == ut.to && action == ut.action;
        }

        [[nodiscard]] constexpr bool operator!= (const LabeledTrans& ut) const noexcept {
            return !(*this == ut);
        }

        [[nodiscard]] constexpr bool operator< (const LabeledTrans& ut) const noexcept {
            if (from   != ut.from) return from < ut.from;
            if (action != ut.action) return action < ut.action;
            return to < ut.to;
        }
    };

    struct ProbTrans {
        StateId from;
        StateId to;
        ActionId action;
        uint32_t prob;
        uint16_t _pad;
    };
    

    // Type-size assertion
        static_assert(sizeof(LabeledTrans), "LabeledTrans must be 12 bytes");
        static_assert(sizeof(UnlabeledTrans), "UnlabeledTrans must be 8 bytes");
        static_assert(sizeof(ProbTrans), "ProbTrans must be 16 bytes");

    // Type assertion 
    // ...
}