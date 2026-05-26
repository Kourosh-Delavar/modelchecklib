#pragma one

#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace modelchecklib {

    using StateId = std::uint32_t;
    using TransId = std::uint32_t;
    using APId = std::uint8_t;
    using ActionId = std::uint16_t;

    inline constexpr StateId INVALID_STATE = std::numeric_limits<StateId>::max();
    inline constexpr TransId INVALID_TRANS = std::numeric_limits<TransId>::max();
    inline constexpr APId INVALID_AP = std::numeric_limits<APId>::max();
    inline constexpr ActionId INVALID_ACTION = std::numeric_limits<ActionId>::max();
    inline constexpr std::size_t MAX_AP = 64;

    // APSet 
    using APSet = std::uint64_t;

    inline constexpr APSet APSET_EMPTY = 0ULL;
    inline constexpr APSet APSET_FULL = ~0ULL;

    [[nodiscard]] constexpr APSet apBitMask(APId id) noexcept {
        return 1ULL << id;
    }

    [[nodiscard]] constexpr APSet apSetAdd(APSet s, APId id) noexcept {
        return s | apBitMask(id);
    }

    [[nodiscard]] constexpr APSet apSetRemove(APSet s, APId id) noexcept {
        return s & ~apBitMask(id);
    }

    [[nodiscard]] constexpr bool apSetContains(APSet s, APId id) noexcept {
        return (s >> id) & 1ULL;
    }

    [[nodiscard]] constexpr APSet apSetUnion(APSet a, APId b) noexcept {
        return a | b;
    }

    [[nodiscard]] constexpr APSet apSetIntersect(APSet a, APId b) noexcept {
        return a & b;
    }

    [[nodiscard]] constexpr APSet apSetDiff(APSet a, APId b) noexcept {
        return a & ~b;
    }

    [[nodiscard]] constexpr int apSetCount(APSet s) noexcept {
        int n = 0;
        while (s) { s &= s-1; ++n;}
        return n;
    }

    // StateFlags 
    enum StateFlags : uint8_t {
        FLAG_NONE = 0x00,
        FLAG_INITIAL = 0x01,
        FLAG_ACCEPTING = 0x02,
        FLAG_DEAD = 0x04,
        FLAG_VISITED = 0x08,
        FLAG_ON_STACK = 0x10,
        FLAG_SEED = 0x20,   
    };

    enum AcceptanceKind : uint8_t {
        //     
    };

    // Type-size assertions
    static_assert(sizeof(StateId) == 4, "StateId must be 4 bytes");
    static_assert(sizeof(TransId) == 4, "TransId must be 4 bytes");
    static_assert(sizeof(APId) == 1, "APId must be 1 bytes");
    static_assert(sizeof(APSet) == 8, "APSet must be 8 bytes");
    static_assert(sizeof(ActionId) == 2, "ActionId must be 2 bytes");
}