#include <cstring>
#include <daxa/types.hpp>
#include <daxa/utils/math_operators.hpp>

#include "chunk.hpp"
#include "shared.inl"

using namespace daxa::math_operators;

Chunk::Chunk(daxa::Device _device, const glm::ivec3& _chunkPos) {
    this->device = _device;
    this->chunkPos = _chunkPos * 16;

    std::vector<Vertex> vertices = {};

    for (u32 x = 0; x < 16; x++) {
        for (u32 y = 0; y < 16; y++) {
            for (u32 z = 0; z < 16; z++) {
                if (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) < 0.5) { 
                    blockIds[x][y][z] = BlockID::Air;
                } else {
                    blockIds[x][y][z] = BlockID::Grass;
                }
            }
        }
    }

    for (u32 x = 0; x < 16; x++) {
        for (u32 y = 0; y < 16; y++) {
            for (u32 z = 0; z < 16; z++) {
                if (blockIds[x][y][z] == BlockID::Air) { continue; }

                f32vec3 position = {static_cast<f32>(x + chunkPos.x), static_cast<f32>(y + chunkPos.y), static_cast<f32>(z + chunkPos.z)};
                f32vec3 color = {static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand())
                 / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX)};

                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f,  0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f, -0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f,  0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f, -0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f, -0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f, -0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f, -0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f,  0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f, -0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f,  0.5f,  0.5f}, color});
                vertices.push_back(Vertex{position + f32vec3{-0.5f,  0.5f, -0.5f}, color});
            }
        }
    }

    vertexBuffer = device.create_buffer(daxa::BufferInfo {
        .size = static_cast<u32>(vertices.size() * sizeof(Vertex)),
        .allocate_info = daxa::MemoryFlagBits::HOST_ACCESS_RANDOM,
        .name = "our_buffer"
    });

    auto ptr = device.get_host_address_as<float>(vertexBuffer);
    std::memcpy(ptr, vertices.data(), vertices.size() * sizeof(Vertex));

    vertexCount = vertices.size();
}

Chunk::~Chunk() {
    device.destroy_buffer(vertexBuffer);
}