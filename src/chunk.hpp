#pragma once

#include <array>
#include <daxa/daxa.hpp>
#include <daxa/device.hpp>
#include <glm/glm.hpp>
#include <FastNoise/FastNoise.h>

using namespace daxa::types;

static constexpr i32 CHUNK_SIZE = 16;

enum struct BlockID: u32 {
    Air,
    Grass, 
    Dirt,
    Stone
};

struct Chunk {
    Chunk(daxa::Device _device, const glm::ivec3& _chunkPos, const FastNoise::SmartNode<> &generator);
    ~Chunk();

    BlockID getVoxel(const glm::ivec3 &p);

    daxa::BufferId faceBuffer;
    u32 chunkSize;
    daxa::Device &device;
    bool renderable = false;
    glm::ivec3 pos = {};

    std::array<std::array<std::array<BlockID, 16>, 16>, 16> blockIds = {};
};

