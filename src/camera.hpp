#pragma once

#include <daxa/types.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>

#define GLM_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace daxa::types;

struct Camera3D {
	f32 fov = 90.0f, aspect = 1.0f;
	f32 nearClip = 0.1f, farClip = 1000.0f;
	glm::mat4 projectionMat = {};
	glm::mat4 translationMat = {};
	glm::mat4 rotationMat = {};

	void resize(i32 sizeX, i32 sizeY);

	void setPosition(glm::vec3 position);

	void setRotation(f32 x, f32 y);

	glm::mat4 getViewProjection();

	glm::mat4 getView();
};

namespace input {
    struct Keybinds {
        i32 move_pz, move_nz;
        i32 move_px, move_nx;
        i32 move_py, move_ny;
        i32 toggle_pause;
        i32 toggle_sprint;
    };

    static inline constexpr Keybinds DEFAULT_KEYBINDS {
        .move_pz = GLFW_KEY_W,
        .move_nz = GLFW_KEY_S,
        .move_px = GLFW_KEY_A,
        .move_nx = GLFW_KEY_D,
        .move_py = GLFW_KEY_SPACE,
        .move_ny = GLFW_KEY_LEFT_CONTROL,
        .toggle_pause = GLFW_KEY_RIGHT_ALT,
        .toggle_sprint = GLFW_KEY_LEFT_SHIFT,
    };
}

struct ControlledCamera3D {
    Camera3D camera{};
    input::Keybinds keybinds = input::DEFAULT_KEYBINDS;
    glm::vec3 position{0, 0, 0}, velocity{}, rotation{};
    f32 speed = 30.0f, mouse_sensitivity = 0.1f;
    f32 sprint_speed = 8.0f;
    f32 sine_rot_x = 0, cosine_rot_x = 1;

    struct MoveFlags {
        uint8_t px : 1, py : 1, pz : 1, nx : 1, ny : 1, nz : 1, sprint : 1;
    } move{};

    void update(f32 dt);
    void on_key(i32 key, i32 action);
    void on_mouse_move(f32 delta_x, f32 delta_y);
};