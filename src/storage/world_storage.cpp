#include "storage/world_storage.hpp"
#include "world/chunk.hpp"
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace Barnocraft::Storage {

    WorldStorage::WorldStorage(const std::string& worldName) {
        worldPath = "saves/" + worldName + "/";
        fs::create_directories(worldPath);
    }

    WorldStorage::~WorldStorage() {}

    bool WorldStorage::saveChunk(const World::Chunk& chunk) {
        try {
            std::string chunkPath = getChunkPath(chunk.coordinates);
            fs::create_directories(fs::path(chunkPath).parent_path());

            const auto& data = chunk.getBlockData();
            
            std::ofstream file(chunkPath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            file.write(reinterpret_cast<const char*>(data.data()), data.size());
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }

    bool WorldStorage::loadChunk(World::Chunk& chunk, const World::ChunkCoordinates& coordinates) {
        try {
            std::string chunkPath = getChunkPath(coordinates);
            
            if (!fs::exists(chunkPath)) {
                return false;
            }

            std::ifstream file(chunkPath, std::ios::binary | std::ios::ate);
            if (!file) {
                return false;
            }

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint8_t> data(size);
            if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
                return false;
            }

            // Create chunk with loaded data
            // Note: This is a simplified version - in practice you'd need to handle this differently
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }

    bool WorldStorage::chunkExists(const World::ChunkCoordinates& coordinates) const {
        return fs::exists(getChunkPath(coordinates));
    }

    bool WorldStorage::deleteChunk(const World::ChunkCoordinates& coordinates) {
        try {
            std::string chunkPath = getChunkPath(coordinates);
            if (fs::exists(chunkPath)) {
                fs::remove(chunkPath);
                return true;
            }
            return false;
        } catch (const std::exception& e) {
            return false;
        }
    }

    std::string WorldStorage::getChunkPath(const World::ChunkCoordinates& coordinates) const {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s/chunks/%d/%d.dat", 
                 worldPath.c_str(), coordinates.x, coordinates.z);
        return std::string(buffer);
    }

} // namespace Barnocraft::Storage
