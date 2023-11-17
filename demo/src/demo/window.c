#include "demo/application.h"
#include "demo/log.h"
#include "demo/window.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cimgui.h>
#include <cimgui_impl.h>

#include "sake/macro.h"

struct _window
{
    GLFWwindow *glfw_window;
};

static struct _window _window = {0};

enum demo_error demo_window_init(struct demo_window_info *info)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (info->fullscreen)
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    _window.glfw_window = glfwCreateWindow(
        info->default_width,
        info->default_height,
        info->application_name, NULL, NULL);
    if (!_window.glfw_window)
    {
        DEMO_LOG_ERROR(DEMO_ERROR_GLFW, "Failed to create window");
        return DEMO_ERROR_GLFW;
    }

    return DEMO_ERROR_NONE;
}

void demo_window_show(void)
{
    glfwShowWindow(_window.glfw_window);
}

void demo_window_get_extent(int *width, int *height)
{
    glfwGetFramebufferSize(_window.glfw_window, width, height);
}

bool demo_window_should_close(void)
{
    return glfwWindowShouldClose(_window.glfw_window);
}

void demo_window_wait_events(void)
{
    glfwWaitEvents();
}

enum demo_error demo_window_create_surface(void *instance, void *surface)
{
    if (glfwCreateWindowSurface(*(VkInstance*) instance, _window.glfw_window, NULL, (VkSurfaceKHR*) surface) != VK_SUCCESS)
    {
        DEMO_LOG_ERROR(DEMO_ERROR_GLFW, "Failed to create window surface");
        return DEMO_ERROR_GLFW;
    }
    return DEMO_ERROR_NONE;
}

const char** demo_window_get_required_extensions(uint32_t* count)
{
    return glfwGetRequiredInstanceExtensions(count);
}

void demo_window_poll_events(void)
{
    glfwPollEvents();
}

void demo_window_destroy(void)
{
    glfwDestroyWindow(_window.glfw_window);
    glfwTerminate();
}

enum demo_error primsa_window_init_ui(void)
{
    if (!ImGui_ImplGlfw_InitForVulkan(_window.glfw_window, true))
    {
        DEMO_LOG_ERROR(DEMO_ERROR_GLFW, "Failed to instantiate GLFw for ImGUI");
        return DEMO_ERROR_GLFW; 
    }
    return DEMO_ERROR_NONE;
}

void demo_window_refresh_ui(void)
{
    ImGui_ImplGlfw_NewFrame();
}

void demo_window_destroy_ui(void)
{
    ImGui_ImplGlfw_Shutdown();
}