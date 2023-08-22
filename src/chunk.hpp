#pragma once

#include <array>
#include <daxa/daxa.hpp>
#include <daxa/device.hpp>
#include <glm/glm.hpp>

using namespace daxa::types;

enum struct BlockID: u32 {
    Air,
    Grass, 
    Dirt,
    Stone
};

struct Chunk {
    Chunk(daxa::Device _device, const glm::ivec3& _chunkPos);
    ~Chunk();

    daxa::BufferId vertexBuffer = {};
    u32 vertexCount = {};
    daxa::Device device = {};
    glm::ivec3 chunkPos = {};

    std::array<std::array<std::array<BlockID, 16>, 16>, 16> blockIds = {};
};

