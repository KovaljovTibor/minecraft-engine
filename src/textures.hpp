#pragma once

#include <daxa/daxa.hpp>
using namespace daxa::types;

struct Textures {
    Textures(daxa::Device &device);
    ~Textures();

    daxa::ImageId atlas_texture_array;
    daxa::SamplerId atlas_sampler;
    daxa::Device& device;
};

