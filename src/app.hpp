#include <daxa/daxa.hpp>
#include <daxa/types.hpp>
#include <glm/fwd.hpp>
#include <memory>
using namespace daxa::types;
#include <daxa/utils/pipeline_manager.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_NATIVE_INCLUDE_NONE
using HWND = void*;
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#include <GLFW/glfw3native.h>

#include <cstring>

#include "shared.inl"
#include "camera.hpp"
#include "chunk.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <unordered_map>

#include "textures.hpp"

auto get_native_platform() -> daxa::NativeWindowPlatform {
    switch (glfwGetPlatform()) {
        case GLFW_PLATFORM_WIN32:
            return daxa::NativeWindowPlatform::WIN32_API;
        case GLFW_PLATFORM_X11:
            return daxa::NativeWindowPlatform::XLIB_API;
        case GLFW_PLATFORM_WAYLAND:
            return daxa::NativeWindowPlatform::WAYLAND_API;
        default:
            return daxa::NativeWindowPlatform::UNKNOWN;
    }
}

auto get_native_handle(GLFWwindow* glfw_window_ptr)
    -> daxa::NativeWindowHandle {
#if defined(_WIN32)
    return glfwGetWin32Window(glfw_window_ptr);
#elif defined(__linux__)
    switch (get_native_platform()) {
        case daxa::NativeWindowPlatform::WAYLAND_API:
            return reinterpret_cast<daxa::NativeWindowHandle>(
                glfwGetWaylandWindow(glfw_window_ptr));
        case daxa::NativeWindowPlatform::XLIB_API:
        default:
            return reinterpret_cast<daxa::NativeWindowHandle>(
                glfwGetX11Window(glfw_window_ptr));
    }
#endif
}

struct App {
    GLFWwindow* glfw_window_ptr = {};

    daxa::Instance instance = {};
    daxa::Device device = {};
    daxa::Swapchain swapchain = {};
    daxa::PipelineManager pipeline_manager = {};
    std::shared_ptr<daxa::RasterPipeline> raster_pipeline = {};
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> chunks = {};
    daxa::ImageId depthBuffer = {};
    std::unique_ptr<Textures> texture = {};

    // noise generator - generates random values - used for world generation
    FastNoise::SmartNode<> generator = []() {
        auto OpenSimplex = FastNoise::New<FastNoise::OpenSimplex2>();
        auto FractalFBm = FastNoise::New<FastNoise::FractalFBm>();
        FractalFBm->SetSource(OpenSimplex);
        FractalFBm->SetGain(0.280f);
        FractalFBm->SetOctaveCount(4);
        FractalFBm->SetLacunarity(4.0f);
        auto DomainScale = FastNoise::New<FastNoise::DomainScale>();
        DomainScale->SetSource(FractalFBm);
        DomainScale->SetScale(0.86f);
        auto PosationOutput = FastNoise::New<FastNoise::PositionOutput>();
        PosationOutput->Set<FastNoise::Dim::Y>(6.72f);
        auto add = FastNoise::New<FastNoise::Add>();
        add->SetLHS(DomainScale);
        add->SetRHS(PosationOutput);
        return add;
    }();

    u32 size_x = 800, size_y = 600;
    bool minimized = false;
    bool paused = false;

    ControlledCamera3D camera = {};

    f64 current_frame = glfwGetTime();
    f64 last_frame = current_frame;
    f64 delta_time{};

    App() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfw_window_ptr = glfwCreateWindow(size_x, size_y, "minecraft clone", nullptr, nullptr);
        glfwSetWindowUserPointer(glfw_window_ptr, this);

        glfwSetWindowSizeCallback(
            glfw_window_ptr, [](GLFWwindow* window_ptr, i32 sx, i32 sy) {
                auto& app =
                    *reinterpret_cast<App*>(glfwGetWindowUserPointer(window_ptr));
                app.resize(static_cast<u32>(sx), static_cast<u32>(sy));
            });
        glfwSetCursorPosCallback(
            glfw_window_ptr, [](GLFWwindow* window_ptr, f64 x, f64 y) {
                auto& app =
                    *reinterpret_cast<App*>(glfwGetWindowUserPointer(window_ptr));
                app.on_mouse_move(static_cast<f32>(x), static_cast<f32>(y));
            });
        glfwSetScrollCallback(
            glfw_window_ptr, [](GLFWwindow* window_ptr, f64 x, f64 y) {
                auto& app =
                    *reinterpret_cast<App*>(glfwGetWindowUserPointer(window_ptr));
                app.on_mouse_scroll(static_cast<f32>(x), static_cast<f32>(y));
            });
        glfwSetMouseButtonCallback(
            glfw_window_ptr, [](GLFWwindow* window_ptr, i32 key, i32 action, i32) {
                auto& app =
                    *reinterpret_cast<App*>(glfwGetWindowUserPointer(window_ptr));
                app.on_mouse_button(key, action);
            });
        glfwSetKeyCallback(glfw_window_ptr, [](GLFWwindow* window_ptr, i32 key, i32,
                                               i32 action, i32) {
            auto& app =
                *reinterpret_cast<App*>(glfwGetWindowUserPointer(window_ptr));
            app.on_key(key, action);
        });

        instance = daxa::create_instance(daxa::InstanceInfo{
            //.enable_validation = true
        });

        device = instance.create_device(daxa::DeviceInfo{
            .name = "my gpu"
        });

        swapchain = device.create_swapchain(daxa::SwapchainInfo{
            .native_window = get_native_handle(glfw_window_ptr),
            .present_mode = daxa::PresentMode::IMMEDIATE,
            .image_usage = daxa::ImageUsageFlagBits::TRANSFER_DST,
            .name = "swapchain"
        });

        pipeline_manager = daxa::PipelineManager(daxa::PipelineManagerInfo {
            .device = device,
            .shader_compile_options = {
                .root_paths = {
                    DAXA_SHADER_INCLUDE_DIR,
                    "./",
                },
                .language = daxa::ShaderLanguage::GLSL,
                .enable_debug_info = true,
            },
            .name = "pipeline_manager",
        });

        depthBuffer = device.create_image({
            .format = daxa::Format::D32_SFLOAT,
            .size = {size_x, size_y, 1},
            .usage = daxa::ImageUsageFlagBits::DEPTH_STENCIL_ATTACHMENT,
        });

        raster_pipeline = pipeline_manager.add_raster_pipeline(daxa::RasterPipelineCompileInfo {
            .vertex_shader_info = daxa::ShaderCompileInfo {
                .source = daxa::ShaderSource { daxa::ShaderFile { .path = "src/shader.glsl" }, },
            },
            .fragment_shader_info = daxa::ShaderCompileInfo {
                .source = daxa::ShaderSource { daxa::ShaderFile { .path = "src/shader.glsl" }, },
            },
            .color_attachments = {{ .format = swapchain.get_format() }},
            .depth_test = {
                .depth_attachment_format = daxa::Format::D32_SFLOAT,
                .enable_depth_test = true,
                .enable_depth_write = true,
            },
            .raster = {
                .face_culling = daxa::FaceCullFlagBits::NONE
            },
            .push_constant_size = sizeof(DrawPush),
        }).value();

        static constexpr i32 worldSizeX = 16;
        static constexpr i32 worldSizeY = 1;
        static constexpr i32 worldSizeZ = 16;

        u32 chunkAmount = 0;

        for (i32 x = -worldSizeX; x <= worldSizeX; x++) {
            for (i32 y = -worldSizeY; y <= worldSizeY; y++) {
                for (i32 z = -worldSizeZ; z <= worldSizeZ; z++) {
                    chunkAmount++;
                    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(device, glm::ivec3{x, y, z}, generator);
                    this->chunks.insert({glm::ivec3{x, y, z}, std::move(chunk)});
                }
            }
        }

        camera.camera.resize(size_x, size_y);

        texture = std::make_unique<Textures>(device);
    }

    ~App() {
        device.wait_idle();
        device.destroy_image(depthBuffer);
        glfwDestroyWindow(glfw_window_ptr);
        glfwTerminate();
    }

    void update() {
        while (!glfwWindowShouldClose(glfw_window_ptr)) {
            glfwPollEvents();

            current_frame = glfwGetTime();
            delta_time = current_frame - last_frame;
            last_frame = current_frame;

            camera.camera.setPosition(camera.position);
            camera.camera.setRotation(camera.rotation.x, camera.rotation.y);
            camera.update(delta_time);

            render();
        }
    }

    void render() {
        daxa::ImageId swapchain_image = swapchain.acquire_next_image();
        if(swapchain_image.is_empty()) { return; }

        daxa::CommandList cmd_list = device.create_command_list({
            .name = "render command list"
        });

        cmd_list.begin_renderpass( daxa::RenderPassBeginInfo {
            .color_attachments = { daxa::RenderAttachmentInfo {
                .image_view = swapchain_image.default_view(),
                .load_op = daxa::AttachmentLoadOp::CLEAR,
                .clear_value = std::array<float, 4>{0.2f, 0.4f, 1.0f, 1.0f},
            }},
            .depth_attachment = {{
                .image_view = depthBuffer.default_view(),
                .load_op = daxa::AttachmentLoadOp::CLEAR,
                .clear_value = daxa::DepthValue{1.0f, 0},
            }},
            .render_area = {.x = 0, .y = 0, .width = size_x, .height = size_y},
        });

        cmd_list.set_pipeline(*raster_pipeline);

        for (const auto& [key, chunk]: chunks) {
            glm::mat4 model = glm::translate(glm::mat4{1.0f}, glm::vec3{chunk->pos * 16});
            glm::mat4 mvp = camera.camera.getViewProjection() * model;
            if (chunk->renderable) {
                cmd_list.push_constant(DrawPush {
                        .modelViewProjection = *reinterpret_cast<f32mat4x4*>(&mvp),
                        .vertices = device.get_device_address(chunk->faceBuffer),
                        .textures = texture->atlas_texture_array.default_view(),
                        .texturesSampler = texture->atlas_sampler
                });
                cmd_list.draw(daxa::DrawInfo { .vertex_count = chunk->chunkSize });
            }
        }

        cmd_list.end_renderpass();

        cmd_list.complete();

        device.submit_commands({
            .command_lists = {std::move(cmd_list)},
            .wait_binary_semaphores = {swapchain.get_acquire_semaphore()},
            .signal_binary_semaphores = {swapchain.get_present_semaphore()},
            .signal_timeline_semaphores = {{swapchain.get_gpu_timeline_semaphore(), swapchain.get_cpu_timeline_value()}},
        });

        device.present_frame({
            .wait_binary_semaphores = {swapchain.get_present_semaphore()},
            .swapchain = swapchain,
        });
    }

    void resize(u32 x, u32 y) {
        minimized = (x == 0 || y == 0);
        if (!minimized) {
            swapchain.resize();
            size_x = swapchain.get_surface_extent().x;
            size_y = swapchain.get_surface_extent().y;
            camera.camera.resize(size_x, size_y);

            device.destroy_image(depthBuffer);

            depthBuffer = device.create_image({
                .format = daxa::Format::D32_SFLOAT,
                .size = {size_x, size_y, 1},
                .usage = daxa::ImageUsageFlagBits::DEPTH_STENCIL_ATTACHMENT,
            });
        }
    }

    void on_mouse_move(f32 x, f32 y) {
        if (!paused) {
            f32 center_x = static_cast<f32>(size_x / 2);
            f32 center_y = static_cast<f32>(size_y / 2);
            auto offset = glm::vec2{x - center_x, center_y - y};
            camera.on_mouse_move(offset.x, offset.y);
            glfwSetCursorPos(glfw_window_ptr, static_cast<f64>(center_x), static_cast<f64>(center_y));
        }
    }

    void on_mouse_scroll(f32 x, f32 y) {}

    void on_mouse_button(int key, int action) {}

    void on_key(int key, int action) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            toggle_pause();
        }
        if (!paused) {
            camera.on_key(key, action);
        }
    }

    void toggle_pause() {
        glfwSetCursorPos(glfw_window_ptr, static_cast<f64>(size_x / 2), static_cast<f64>(size_y / 2));
        glfwSetInputMode(glfw_window_ptr, GLFW_CURSOR, paused ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        glfwSetInputMode(glfw_window_ptr, GLFW_RAW_MOUSE_MOTION, paused);
        paused = !paused;
    }
};