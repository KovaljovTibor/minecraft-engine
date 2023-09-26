#pragma once
#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#include <daxa/daxa.inl>

struct Vertex {
    daxa_f32vec3 pos;
    daxa_f32vec3 color;
    daxa_u32 id;
    daxa_f32vec2 uv;
};

DAXA_DECL_BUFFER_PTR(Vertex)

struct DrawPush {
    daxa_f32mat4x4 modelViewProjection;
    daxa_BufferPtr(Vertex) vertices;
    daxa_ImageViewId textures;
    daxa_SamplerId texturesSampler;
};

