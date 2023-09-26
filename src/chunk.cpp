#include <cstring>
#include <daxa/types.hpp>
#include <daxa/utils/math_operators.hpp>

#include "chunk.hpp"
#include "shared.inl"

using namespace daxa::math_operators;

static constexpr inline u32 CUBE_VERTS = 36;
static constexpr inline u32 CUBE_VERTS_SIZE = CUBE_VERTS * sizeof(Vertex);
static constexpr inline u32 CHUNK_VERTS = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * CUBE_VERTS;
static constexpr inline u32 CHUNK_VERTS_SIZE = CHUNK_VERTS * sizeof(Vertex);

Chunk::Chunk(daxa::Device _device, const glm::ivec3 &_chunkPos, const FastNoise::SmartNode<> &generator) : device{
        _device}, pos{_chunkPos} {
    std::vector<float> noiseOutput(16 * 16 * 16);
    generator->GenUniformGrid3D(noiseOutput.data(), 16 * pos.z, 16 * pos.y, 16 * pos.x, 16, 16, 16, 0.05f, 1337);

    std::vector<Vertex> vertices;
    vertices.reserve(CHUNK_VERTS);

    int index = 0;

    for (u32 x = 0; x < CHUNK_SIZE; x++) {
        for (u32 y = 0; y < CHUNK_SIZE; y++) {
            for (u32 z = 0; z < CHUNK_SIZE; z++) {
                if (noiseOutput[index++] <= 0.0f) {
                    blockIds[x][y][z] = BlockID::Stone;
                } else {
                    blockIds[x][y][z] = BlockID::Air;
                }
            }
        }
    }

    for(u32 x = 0; x < CHUNK_SIZE; x++) {
        for(u32 y = 0; y < CHUNK_SIZE; y++) {
            for(u32 z = 0; z < CHUNK_SIZE; z++) {
                if(blockIds[x][y][z] != BlockID::Stone) { continue; }
                f32 f_x = static_cast<f32>(x);
                f32 f_y = static_cast<f32>(y);
                f32 f_z = static_cast<f32>(z);

                f32 r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                f32 g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                f32 b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

                f32vec3 col = {r, g, b};
                glm::ivec3 voxel_pos = { x, y, z };

                std::vector<Vertex> block_verts = {};

                u32 id = 4;

                if(getVoxel(voxel_pos + glm::ivec3{ 0, 0, -1 }) == BlockID::Air) {
                    std::vector<Vertex> face = {
                            Vertex {{-0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 0.0f } },
                            Vertex {{ 0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 1.0f } },
                            Vertex {{-0.5f + f_x,  0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                            Vertex {{-0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 0.0f } },
                    };

                    block_verts.insert(block_verts.end(), face.begin(), face.end());
                }

                if(getVoxel(voxel_pos + glm::ivec3{ 0, 0, +1 }) == BlockID::Air) {
                    std::vector<Vertex> face = {
                            Vertex {{-0.5f + f_x, -0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 0.0f } },
                            Vertex {{ 0.5f + f_x, -0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 1.0f } },
                            Vertex {{-0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                            Vertex {{-0.5f + f_x, -0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 0.0f } },
                    };

                    block_verts.insert(block_verts.end(), face.begin(), face.end());
                }

                if(getVoxel(voxel_pos + glm::ivec3{ -1, 0, 0 }) == BlockID::Air) {
                    std::vector<Vertex> face = {
                            Vertex {{-0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                            Vertex {{-0.5f + f_x,  0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 1.0f } },
                            Vertex {{-0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                            Vertex {{-0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                            Vertex {{-0.5f + f_x, -0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 0.0f } },
                            Vertex {{-0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                    };

                    block_verts.insert(block_verts.end(), face.begin(), face.end());
                }

                if(getVoxel(voxel_pos + glm::ivec3{ +1, 0, 0 }) == BlockID::Air) {
                    std::vector<Vertex> face = {
                            Vertex {{ 0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x, -0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 0.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                    };

                    block_verts.insert(block_verts.end(), face.begin(), face.end());
                }

                if(getVoxel(voxel_pos + glm::ivec3{ 0, -1, 0 }) == BlockID::Air) {
                    std::vector<Vertex> face = {
                            Vertex {{-0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x, -0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                            Vertex {{ 0.5f + f_x, -0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                            Vertex {{-0.5f + f_x, -0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 0.0f } },
                            Vertex {{-0.5f + f_x, -0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                    };

                    block_verts.insert(block_verts.end(), face.begin(), face.end());
                }

                if(getVoxel(voxel_pos + glm::ivec3{ 0, +1, 0 }) == BlockID::Air) {
                    std::vector<Vertex> face = {
                            Vertex {{-0.5f + f_x,  0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 1.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                            Vertex {{ 0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 1.0f, 0.0f } },
                            Vertex {{-0.5f + f_x,  0.5f + f_y,  0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 0.0f } },
                            Vertex {{-0.5f + f_x,  0.5f + f_y, -0.5f + f_z},      { col.x, col.y, col.z }, id, { 0.0f, 1.0f } },
                    };

                    block_verts.insert(block_verts.end(), face.begin(), face.end());
                }

                vertices.insert(vertices.end(), block_verts.begin(), block_verts.end());
            }
        }
    }


    std::cout << vertices.size() << std::endl;
    chunkSize = vertices.size();
    renderable = chunkSize != 0;

    this->faceBuffer = device.create_buffer(
            {.size = CHUNK_VERTS_SIZE, .allocate_info = daxa::MemoryFlagBits::DEDICATED_MEMORY,});

    daxa::BufferId staging_buffer = device.create_buffer(
            {.size = CHUNK_VERTS_SIZE, .allocate_info = daxa::MemoryFlagBits::HOST_ACCESS_RANDOM,});

    auto *buffer_ptr = device.get_host_address_as<Vertex>(staging_buffer);

    std::memcpy(buffer_ptr, vertices.data(), CHUNK_VERTS_SIZE);

    daxa::CommandList command_list = device.create_command_list({.name = "my command list"});

    command_list.copy_buffer_to_buffer(
            {.src_buffer = staging_buffer, .dst_buffer = faceBuffer, .size = CHUNK_VERTS_SIZE});

    command_list.complete();
    device.submit_commands({.command_lists = {std::move(command_list)},});
    device.wait_idle();
    device.destroy_buffer(staging_buffer);
}

Chunk::~Chunk() {
    device.destroy_buffer(faceBuffer);
}

BlockID Chunk::getVoxel(const glm::ivec3 &p) {
    if(p.x < 0 || p.x >= CHUNK_SIZE) {
        return BlockID::Air;
    }
    if(p.y < 0 || p.y >= CHUNK_SIZE) {
        return BlockID::Air;
    }
    if(p.z < 0 || p.z >= CHUNK_SIZE) {
        return BlockID::Air;
    }
    return blockIds[p.x][p.y][p.z];
}
