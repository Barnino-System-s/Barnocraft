#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace Barnocraft::World {

    struct Vector3i {
        int32_t x = 0;
        int32_t y = 0;
        int32_t z = 0;

        Vector3i() = default;
        Vector3i(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z) {}

        std::string to_string() const {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
        }

        bool operator==(const Vector3i& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector3i& other) const {
            return !(*this == other);
        }

        bool equals(const Vector3i& other) const {
            return *this == other;
        }

        size_t hash() const {
            size_t result = static_cast<size_t>(x);
            result = (result * 397) ^ static_cast<size_t>(y);
            result = (result * 397) ^ static_cast<size_t>(z);
            return result;
        }
    };

} // namespace Barnocraft::World

// Hash specialization for std::unordered_map
namespace std {
    template<> struct hash<Barnocraft::World::Vector3i> {
        size_t operator()(const Barnocraft::World::Vector3i& v) const {
            return v.hash();
        }
    };
}
