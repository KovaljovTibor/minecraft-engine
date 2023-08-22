#include "shared.inl"

DAXA_DECL_PUSH_CONSTANT(DrawPush, push)

#if DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_VERTEX

layout(location = 0) out f32vec3 out_color;

void main() {
  out_color = deref(push.vertices[gl_VertexIndex]).color;
  gl_Position = push.modelViewProjection * vec4(deref(push.vertices[gl_VertexIndex]).pos, 1.0);
}

#elif DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_FRAGMENT

layout(location = 0) in f32vec3 in_color;

layout(location = 0) out vec4 color;

void main() {
    color = vec4(in_color, 1.0);
}

#endif