#pragma once

#include "modelchecklib/core/Types.h"
#include <functional>

namespace modelchecklib {

    template<typename Model>
    struct StateNode {
        using StateId = typename Model::StateId;
        using APId = typename Model::APId;

        APSet<Model> label;
        StateId id;
        StateFlags flags;

        constexpr StateNode() noexcept
            : label(),
              id(Model::INVALID_STATE),
              flags(StateFlags::NONE) {}

        constexpr StateNode(StateId i, APSet<Model> lbl, StateFlags f = StateFlags::NONE) noexcept
            : label(lbl),
              id(i),
              flags(f) {}


        // flags accessors   
        [[nodiscard]] constexpr bool isNone() const noexcept {
            return flags == StateFlags.NONE;
        }

        [[nodiscard]] constexpr bool isInitial() const noexcept {
            return flags == StateFlags::INITIAL;
        }

        [[nodiscard]] constexpr bool isAccepting() const noexcept {
            return flags == StateFlags::ACCEPTING;
        }

        [[nodiscard]] constexpr bool isDead() const noexcept {
            return flags == StateFlags::DEAD;
        }

        [[nodiscard]] constexpr bool isVisited() const noexcept {
            return flags == StateFlags::VISITED;
        }

        [[nodiscard]] constexpr bool isOnStack() const noexcept {
            return flags == StateFlags::ON_STACK;
        }

        [[nodiscard]] constexpr bool isSeed() const noexcept {
            return flags == StateFlags::SEED;
        }

        // flags mutators
        constexpr void setFlag(StateFlags f) noexcept {
            flags |= f;
        }

        constexpr void clearFlag(StateFlags f) noexcept {
            flags &= ~f; 
        }

        // AP accessors
        [[nodiscard]] constexpr bool hasAP(const APId& ap) const noexcept {
            return label.contains(ap);
        }

        // StateNode operators 
        [[nodiscard]] constexpr bool operator==(const StateNode& sn) const noexcept {
            return id == sn.id;
        }
        [[nodiscard]] constexpr bool operator!=(const StateNode& sn) const noexcept {
            return id != sn.id;
        }
        [[nodiscard]] constexpr bool operator<(const StateNode& sn) const noexcept {
            return id < sn.id;
        }
    };    
}    

// hash implementation 
namespace std {
    template <typename Model>
    struct hash<modelchecklib::StateNode<Model>> {
        [[nodiscard]] std::size_t operator()(const modelchecklib::StateNode<Model>& s) const noexcept {
            return std::hash<typename Model::StateId>{}(s.id);
        }
    };
}