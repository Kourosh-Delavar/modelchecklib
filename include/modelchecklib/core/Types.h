#pragma once

#include <bitset>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

namespace modelchecklib {

    enum class APStoragePolicy {
        Bitset,
        StringList
    };

    template<
        std::size_t NumAP_ = 64,
        APStoragePolicy StoragePolicy_ = APStoragePolicy::Bitset,
        typename StateIdT = std::uint32_t,
        typename TransIdT = std::uint32_t,
        typename APIdT = std::uint16_t,
        typename ActionIdT = std::uint16_t
    >    
    struct Model {
        static constexpr std::size_t NumAP = NumAP_;
        static constexpr APStoragePolicy StoragePolicy = StoragePolicy_;
        
        using StateId = StateIdT;
        using TransId = TransIdT;
        using ActionId = ActionIdT;
        using APId = std::conditional_t<
            StoragePolicy == APStoragePolicy::StringList, std::string, APIdT
        >;

        static constexpr StateId INVALID_STATE = std::numeric_limits<StateId>::max();
        static constexpr TransId INVALID_TRANS = std::numeric_limits<TransId>::max();
        static constexpr ActionId INVALID_ACTION = std::numeric_limits<ActionId>::max();    

        inline static const APId INVALID_AP = []() {
            if constexpr (std::is_same_v<APId, std::string>) {
                return std::string("");
            } else {
                return std::numeric_limits<APId>::max();
            }
        } (); 
    };

    template<typename Model>
    class APBitsetImpl {
        private:
            std::bitset<Model::NumAP> mask;

        public:
            using APId = typename Model::APId;

            constexpr APBitsetImpl() noexcept = default;

            [[nodiscard]] static APBitsetImpl full() noexcept {
                APBitsetImpl s;
                s.mask.set();
                return s;
            }
 
            void add(APId id) { mask.set(id); }
            void remove(APId id) { mask.reset(id); }
            void clear() noexcept { mask.reset(); }

            [[nodiscard]] bool contains(APId id) const { return mask.test(id); }
            [[nodiscard]] std::size_t count() const { return mask.count(); }
            [[nodiscard]] bool isEmpty() const noexcept { return mask.none(); }

            // APBitsetImpl operators
            [[nodiscard]] bool operator==(const APBitsetImpl& other) const noexcept {
                return mask == other.mask;
            }
            [[nodiscard]] bool operator!=(const APBitsetImpl& other) const noexcept {
                return mask != other.mask;
            }
            APBitsetImpl& operator|=(const APBitsetImpl& other) noexcept {
                mask |= other.mask;
                return *this;
            }
            APBitsetImpl& operator&=(const APBitsetImpl& other) noexcept {
                mask &= other.mask;
                return *this;
            }
    };

    template<typename Model>
    class APStringListImpl {
        private:
            std::vector<std::string> active_aps; 

        public:
            using APId = std::string;

            constexpr APStringListImpl() noexcept = default;
 
            void add(const APId& id) {
                auto it = std::lower_bound(active_aps.begin(), active_aps.end(), id);
                if (it == active_aps.end() || *it != id) {
                    active_aps.insert(it, id);
                }
            }

            void remove(const APId& id) {
                auto it = std::lower_bound(active_aps.begin(), active_aps.end(), id);
                if (it != active_aps.end() && *it == id) {
                    active_aps.erase(it);
                }
            }
            
            void clear() noexcept { active_aps.clear(); }

            [[nodiscard]] bool contains(const APId& id) const {
                return std::binary_search(active_aps.begin(), active_aps.end(), id);
            }

            [[nodiscard]] std::size_t count() const noexcept { return active_aps.size(); }
            [[nodiscard]] bool isEmpty() const noexcept { return active_aps.empty(); }

            // APStringListImpl operators
            [[nodiscard]] bool operator==(const APStringListImpl& other) const noexcept {
                return active_aps == other.active_aps;
            }
            [[nodiscard]] bool operator!=(const APStringListImpl& other) const noexcept {
                return active_aps != other.active_aps;
            }
            APStringListImpl& operator|=(const APStringListImpl& other) noexcept {
                std::vector<std::string> res;
                std::set_union(
                    active_aps.begin(), active_aps.end(),
                    other.active_aps.begin(), other.active_aps.end(),
                    std::back_inserter(res)
                );
                active_aps = std::move(res);
                return *this;
            }
            APStringListImpl& operator&=(const APStringListImpl& other) noexcept {
                std::vector<std::string> res;
                std::set_intersection(
                    active_aps.begin(), active_aps.end(),
                    other.active_aps.begin(), other.active_aps.end(),
                    std::back_inserter(res)
                );
                active_aps = std::move(res);
                return *this;
            }
    };

    template<typename Model>
    using APSet = std::conditional_t<
        Model::StoragePolicy == APStoragePolicy::Bitset,
        APBitsetImpl<Model>,
        APStringListImpl<Model>
    >;

    enum class StateFlags : std::uint8_t {
        NONE       = 0x00,
        INITIAL    = 0x01,
        ACCEPTING  = 0x02,
        DEAD       = 0x04,
        VISITED    = 0x08,
        ON_STACK   = 0x10,
        SEED       = 0x20,   
    };

    // StateFlags operators 
    [[nodiscard]] constexpr StateFlags operator|(StateFlags a, StateFlags b) noexcept {
        return static_cast<StateFlags>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
    }
    [[nodiscard]] constexpr StateFlags operator&(StateFlags a, StateFlags b) noexcept {
        return static_cast<StateFlags>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
    }
    [[nodiscard]] constexpr StateFlags operator~(StateFlags a) noexcept {
        return static_cast<StateFlags>(~static_cast<std::uint8_t>(a));
    }
    constexpr StateFlags& operator|=(StateFlags& a, StateFlags b) noexcept { a = a | b; return a; }
    constexpr StateFlags& operator&=(StateFlags& a, StateFlags b) noexcept { a = a & b; return a; }

    [[nodiscard]] constexpr bool hasFlag(StateFlags current, StateFlags target) noexcept {
        return (current & target) == target;
    }

    enum class AcceptanceCondition : std::uint8_t {
        NONE = 0x00,
        BUCHI = 0x01,
        GENERALIZED_BUCHI = 0x02,

        // more conditions
    };
}