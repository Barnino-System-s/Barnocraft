#pragma once

#include "vector3i.hpp"
#include "world_constants.hpp"
#include <string>
#include <functional>

namespace Barnocraft::World {

    struct ChunkCoordinates {
        int32_t x = 0;
        int32_t z = 0;

        ChunkCoordinates() = default;
        ChunkCoordinates(int32_t x, int32_t z) : x(x), z(z) {}

        std::string to_string() const {
            return "Chunk(" + std::to_string(x) + ", " + std::to_string(z) + ")";
        }

        bool operator==(const ChunkCoordinates& other) const {
            return x == other.x && z == other.z;
        }

        bool operator!=(const ChunkCoordinates& other) const {
            return !(*this == other);
        }

        bool equals(const ChunkCoordinates& other) const {
            return *this == other;
        }

        size_t hash() const {
            return (static_cast<size_t>(x) * 397) ^ static_cast<size_t>(z);
        }

        // Convert world position to chunk coordinates
        static ChunkCoordinates from_world_position(float worldX, float worldZ) {
            int32_t chunkX = static_cast<int32_t>(std::floor(worldX / static_cast<float>(CHUNK_SIZE_X)));
            int32_t chunkZ = static_cast<int32_t>(std::floor(worldZ / static_cast<float>(CHUNK_SIZE_Z)));
            return ChunkCoordinates(chunkX, chunkZ);
        }

        static ChunkCoordinates from_world_position_int(int32_t worldX, int32_t worldZ) {
            int32_t chunkX = worldX / CHUNK_SIZE_X;
            if (worldX < 0 && worldX % CHUNK_SIZE_X != 0) {
                chunkX--;
            }
            int32_t chunkZ = worldZ / CHUNK_SIZE_Z;
            if (worldZ < 0 && worldZ % CHUNK_SIZE_Z != 0) {
                chunkZ--;
            }
            return ChunkCoordinates(chunkX, chunkZ);
        }

        // Get local position within chunk from world position
        static Vector3i get_local_position(const Vector3i& worldPos) {
            int32_t localX = worldPos.x % CHUNK_SIZE_X;
            if (localX < 0) localX += CHUNK_SIZE_X;
            
            int32_t localY = worldPos.y % CHUNK_SIZE_Y;
            if (localY < 0) localY += CHUNK_SIZE_Y;
            
            int32_t localZ = worldPos.z % CHUNK_SIZE_Z;
            if (localZ < 0) localZ += CHUNK_SIZE_Z;
            
            return Vector3i(localX, localY, localZ);
        }
    };

} // namespace Barnocraft::World

// Hash specialization for std::unordered_map
namespace std {
    template<> struct hash<Barnocraft::World::ChunkCoordinates> {
        size_t operator()(const Barnocraft::World::ChunkCoordinates& c) const {
            return c.hash();
        }
    };
}
