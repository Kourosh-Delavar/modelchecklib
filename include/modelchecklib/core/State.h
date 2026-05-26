#pragma once

#include "modelchecklib/core/Types.h"
#include <functional>

namespace modelchecklib {

    struct StateNode {
        APSet label;
        StateId id;
        uint8_t flags;
        uint8_t _pad[3];

        constexpr StateNode() noexcept
            : label(APSET_EMPTY), id(INVALID_STATE), flags(FLAG_NONE), _pad{0} {}

        constexpr StateNode(StateId i, APSet lbl, uint8_t f = FLAG_NONE) noexcept
            : label(lbl), id(i), flags(f), _pad{} {}

        // Flag accessors   
        [[nodiscard]] constexpr bool isNone() const noexcept {
            return flags & FLAG_NONE;
        }

        [[nodiscard]] constexpr bool isInitial() const noexcept {
            return flags & FLAG_INITIAL;
        }

        [[nodiscard]] constexpr bool isAccepting() const noexcept {
            return flags & FLAG_ACCEPTING;
        }

        [[nodiscard]] constexpr bool isDead() const noexcept {
            return flags & FLAG_DEAD;
        }

        [[nodiscard]] constexpr bool isVisited() const noexcept {
            return flags & FLAG_VISITED;
        }

        [[nodiscard]] constexpr bool isOnStack() const noexcept {
            return flags & FLAG_ON_STACK;
        }

        [[nodiscard]] constexpr bool isSeed() const noexcept {
            return flags & FLAG_SEED;
        }

        // Flag mutators
        constexpr void setFlag(uint8_t f) noexcept {
            flags |= static_cast<uint8_t>(f);
        }

        constexpr void clearFlag(uint8_t f) noexcept {
            flags &= static_cast<uint8_t>(~f); 
        }

        // AP accessors
        [[nodiscard]] constexpr bool hasAP(APId ap) const noexcept {
            return apSetContains(label, ap);
        }

        // StateNode operators 
        [[nodiscard]] constexpr bool operator== (const StateNode& sn) const noexcept {
            return id == sn.id;
        }

        [[nodiscard]] constexpr bool operator!= (const StateNode& sn) const noexcept {
            return id != sn.id;
        }

        [[nodiscard]] constexpr bool operator< (const StateNode& sn) const noexcept {
            return id < sn.id;
        }
    };

    // Type-size assertions 
    static_assert(sizeof(StateNode) == 16, "StateNode must be exactly 16 bytes");
    
    // Type assertions
    // ...
    
}    

// StateNode hash based on id
namespace std {
template <>
struct hash<modelchecklib::StateNode> {
    [[nodiscard]] std::size_t
    operator()(const modelchecklib::StateNode& s) const noexcept {
        return std::hash<modelchecklib::StateId>{}(s.id);
    }
};
}