#include "camera.hpp"
#include <numbers>

void Camera3D::resize(i32 sizeX, i32 sizeY) {
    aspect = static_cast<f32>(sizeX) / static_cast<f32>(sizeY);
    projectionMat = glm::perspective(glm::radians(fov), aspect, nearClip, farClip);
    projectionMat[1][1] *= -1.0f;
}

void Camera3D::setPosition(glm::vec3 position) {
    translationMat = glm::translate(glm::mat4(1), position);
}

void Camera3D::setRotation(f32 x, f32 y) {
    rotationMat = glm::rotate(glm::rotate(glm::mat4(1), y, {1, 0, 0}), x, {0, 1, 0});
}

glm::mat4 Camera3D::getViewProjection() {
    return projectionMat * rotationMat * translationMat;
}

glm::mat4 Camera3D::getView() {
    return translationMat * rotationMat;
}

void ControlledCamera3D::update(f32 dt) {
    auto delta_pos = speed * dt;
    if (move.sprint)
        delta_pos = sprint_speed;
    if (move.px)
        position.z += sine_rot_x * delta_pos, position.x += cosine_rot_x * delta_pos;
    if (move.nx)
        position.z -= sine_rot_x * delta_pos, position.x -= cosine_rot_x * delta_pos;
    if (move.pz)
        position.x -= sine_rot_x * delta_pos, position.z += cosine_rot_x * delta_pos;
    if (move.nz)
        position.x += sine_rot_x * delta_pos, position.z -= cosine_rot_x * delta_pos;
    if (move.py)
        position.y -= delta_pos;
    if (move.ny)
        position.y += delta_pos;

    constexpr auto MAX_ROT = std::numbers::pi_v<f32> / 2;
    if ( rotation.y > MAX_ROT)
         rotation.y = MAX_ROT;
    if ( rotation.y < -MAX_ROT)
         rotation.y = -MAX_ROT;
}

void ControlledCamera3D::on_key(i32 key, i32 action) {
    if (key == keybinds.move_pz)
        move.pz = action != 0;
    if (key == keybinds.move_nz)
        move.nz = action != 0;
    if (key == keybinds.move_px)
        move.px = action != 0;
    if (key == keybinds.move_nx)
        move.nx = action != 0;
    if (key == keybinds.move_py)
        move.py = action != 0;
    if (key == keybinds.move_ny)
        move.ny = action != 0;
    if (key == keybinds.toggle_sprint)
        move.sprint = action != 0;
}

void ControlledCamera3D::on_mouse_move(f32 delta_x, f32 delta_y) {
    rotation.x += delta_x * mouse_sensitivity * 0.0001f * camera.fov;
    rotation.y -= delta_y * mouse_sensitivity * 0.0001f * camera.fov;
    sine_rot_x = std::sin( rotation.x);
    cosine_rot_x = std::cos( rotation.x);
}