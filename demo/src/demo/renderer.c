#include <stdalign.h>
#include <string.h>
#include <stdalign.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>
#include <math.h>

#include <vulkan/vulkan.h>
#include <cglm/cglm.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_VULKAN
#include <cimgui_impl.h>

#include <sake/macro.h>

#include "demo/window.h"
#include "demo/log.h"

struct _backend_instance
{
  VkInstance vk_instance;
  VkSurfaceKHR vk_surface;
#ifndef NDEBUG
  VkDebugUtilsMessengerEXT vk_debug_messenger;
#endif
};

struct _backend_device
{
  VkPhysicalDevice vk_physical_device;
  VkDevice vk_device;
  VkPhysicalDeviceProperties vk_physical_device_properties;
  VkQueue vk_graphic_queue;
  uint32_t vk_graphic_queue_index;
  VkQueue vk_present_queue;
  uint32_t vk_present_queue_index;
};

struct _backend_swapchain
{
  VkSwapchainKHR vk_swapchain;
  VkExtent2D vk_extent;
  VkImage *vk_images;
  VkImageView *vk_image_views;
};

struct _backend_sync
{
  VkFence *vk_render_fence;
  VkSemaphore *vk_present_semaphore;
  VkSemaphore *vk_render_semaphore;
};

struct _backend_ui
{
  VkDescriptorPool vk_descriptorpool;
  VkCommandPool vk_commandpool;
  VkCommandBuffer *vk_commandbuffers;
  VkRenderPass vk_renderpass;
  VkFramebuffer *vk_framebuffers;
};

struct _backend
{
  uint64_t frame_count;
  uint32_t current_frame_in_flight;

  struct _backend_instance instance;
  struct _backend_device device;
  struct _backend_swapchain swapchain;
  struct _backend_sync sync;
  struct _backend_ui ui;
};

struct _backend_info
{
  const char *vertex_shader_path;
  const char *frag_shader_path;

  uint32_t max_frames_in_flight;
  VkPhysicalDeviceFeatures features_requested;
  const char **device_extensions;
  VkSurfaceFormatKHR vk_surface_format;
  VkPresentModeKHR vk_present_mode;
  uint32_t array_layer_count;
  VkSurfaceTransformFlagBitsKHR transform;
  VkSwapchainCreateFlagBitsKHR create_flags;
  VkImageUsageFlags image_usage_flags;
  VkCompositeAlphaFlagBitsKHR composite_alpha;
  VkBool32 clipped;
  VkSampleCountFlagBits sample_count;

#ifndef NDEBUG
  const char **validation_layers;
  const char **debug_extensions;
#endif
};

static enum demo_error _backend_instance_init(void);

static enum demo_error _backend_instance_create_instance(void);

static enum demo_error _backend_instance_create_surface(void);

#ifndef NDEBUG
static enum demo_error _backend_instance_create_debug_instance(void);

static VKAPI_ATTR VkBool32 VKAPI_CALL _backend_instance_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                                       VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                                       const VkDebugUtilsMessengerCallbackDataEXT *cb_data,
                                                                       void *user_data);
#endif

static void _backend_instance_destroy(void);

static enum demo_error _backend_device_init(void);

static enum demo_error _backend_device_pick_physical_device(void);

static bool _backend_device_is_physical_device_suitable(VkPhysicalDevice vk_physical_device);

static int32_t _backend_device_graphic_queue_index(VkPhysicalDevice physical_device);

static int32_t _backend_device_present_queue_index(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

static bool _backend_device_check_extension_support(VkPhysicalDevice physical_device,
                                                    const char **desired_extensions,
                                                    uint32_t desired_extension_count);

static bool _backend_device_check_feature_support(VkPhysicalDeviceFeatures supported, VkPhysicalDeviceFeatures requested);

static enum demo_error _backend_device_create_logical_device(void);

static void _backend_device_destroy(void);

static void _backend_device_wait_idle(void);

static enum demo_error _backend_swapchain_init(void);

static bool _backend_swapchain_check_surface_format_support(void);

static bool _backend_swapchain_check_present_mode_support(void);

static bool _backend_swapchain_check_image_count_support(void);

static bool _backend_swapchain_check_array_layer_count_support(void);

static VkExtent2D _backend_swapchain_get_current_extent(void);

static enum demo_error _backend_swapchain_recreate(void);

static void _backend_swapchain_destroy(void);

static enum demo_error _backend_sync_init(void);

static void _backend_sync_destroy(void);

static struct _backend _backend = {0};

static struct _backend_info _backend_info = {
    .vertex_shader_path = "default.vert.spv",
    .frag_shader_path = "default.frag.spv",

    .max_frames_in_flight = 3,
    .features_requested = {
        .samplerAnisotropy = VK_TRUE},
    .vk_surface_format = {.format = VK_FORMAT_B8G8R8A8_SRGB, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
    .array_layer_count = 1,
    .clipped = VK_TRUE,
    .composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .create_flags = 0,
    .image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    .transform = 0,
    .vk_present_mode = VK_PRESENT_MODE_FIFO_KHR,
    .sample_count = VK_SAMPLE_COUNT_1_BIT,
    .device_extensions = (const char *[]){"VK_KHR_swapchain"},
#ifndef NDEBUG
    .validation_layers = (const char *[]){"VK_LAYER_KHRONOS_validation"},
    .debug_extensions = (const char *[]){"VK_EXT_debug_utils"}
#endif
};

enum demo_error demo_renderer_init(void)
{
  enum demo_error status;

  status = _backend_instance_init();
  if (status != DEMO_ERROR_NONE)
    return status;

  status = _backend_device_init();
  if (status != DEMO_ERROR_NONE)
    return status;

  status = _backend_swapchain_init();
  if (status != DEMO_ERROR_NONE)
    return status;

  status = _backend_sync_init();
  if (status != DEMO_ERROR_NONE)
    return status;

  return DEMO_ERROR_NONE;
}

enum demo_error demo_renderer_draw(void)
{
  VkResult result = VK_SUCCESS;

  _backend.current_frame_in_flight = _backend.frame_count % _backend_info.max_frames_in_flight;

  if (vkWaitForFences(_backend.device.vk_device, 1, &_backend.sync.vk_render_fence[_backend.current_frame_in_flight], VK_TRUE, 1000000000) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to wait render fence");
    return DEMO_ERROR_VK;
  }

  uint32_t swapchainImageIndex = 0;
  result = vkAcquireNextImageKHR(
      _backend.device.vk_device,
      _backend.swapchain.vk_swapchain,
      1000000000,
      _backend.sync.vk_present_semaphore[_backend.current_frame_in_flight],
      VK_NULL_HANDLE,
      &swapchainImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_TIMEOUT)
    return _backend_swapchain_recreate();

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to acquire next image");
    return DEMO_ERROR_VK;
  }

  if (vkResetFences(_backend.device.vk_device, 1, &_backend.sync.vk_render_fence[_backend.current_frame_in_flight]) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to reset render fence");
    return DEMO_ERROR_VK;
  }

  /* Render ImGUI */
  {
    if (vkResetCommandBuffer(_backend.ui.vk_commandbuffers[_backend.current_frame_in_flight], 0) != VK_SUCCESS)
    {
      DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to reset command buffer");
      return DEMO_ERROR_VK;
    }

    VkCommandBufferBeginInfo commandbufferbegin_info = {0};
    commandbufferbegin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandbufferbegin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(_backend.ui.vk_commandbuffers[_backend.current_frame_in_flight], &commandbufferbegin_info) != VK_SUCCESS)
    {
      DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to reset begin command buffer");
      return DEMO_ERROR_VK;
    }

    VkClearValue clearValue = {0};
    clearValue.color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderpassbegin_info = {0};
    renderpassbegin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpassbegin_info.renderPass = _backend.ui.vk_renderpass;
    renderpassbegin_info.renderArea.offset = (VkOffset2D){.x = 0, .y = 0};
    renderpassbegin_info.renderArea.extent = _backend.swapchain.vk_extent;
    renderpassbegin_info.clearValueCount = 1;
    renderpassbegin_info.pClearValues = &clearValue;
    renderpassbegin_info.framebuffer = _backend.ui.vk_framebuffers[swapchainImageIndex];
    vkCmdBeginRenderPass(_backend.ui.vk_commandbuffers[_backend.current_frame_in_flight], &renderpassbegin_info, VK_SUBPASS_CONTENTS_INLINE);

    ImGui_ImplVulkan_RenderDrawData(igGetDrawData(),
                                    _backend.ui.vk_commandbuffers[_backend.current_frame_in_flight],
                                    VK_NULL_HANDLE);

    vkCmdEndRenderPass(_backend.ui.vk_commandbuffers[_backend.current_frame_in_flight]);

    if (vkEndCommandBuffer(_backend.ui.vk_commandbuffers[_backend.current_frame_in_flight]) != VK_SUCCESS)
    {
      DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to reset end command buffer");
      return DEMO_ERROR_VK;
    }
  }

  VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit_info = {0};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &_backend.ui.vk_commandbuffers[_backend.current_frame_in_flight];
  submit_info.pWaitDstStageMask = &waitStage;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &_backend.sync.vk_present_semaphore[_backend.current_frame_in_flight];
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &_backend.sync.vk_render_semaphore[_backend.current_frame_in_flight];
  if (vkQueueSubmit(_backend.device.vk_graphic_queue, 1, &submit_info, _backend.sync.vk_render_fence[_backend.current_frame_in_flight]) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to submit command buffer to the queue");
    return DEMO_ERROR_VK;
  }

  VkPresentInfoKHR present_info = {0};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pSwapchains = &_backend.swapchain.vk_swapchain;
  present_info.swapchainCount = 1;
  present_info.pWaitSemaphores = &_backend.sync.vk_render_semaphore[_backend.current_frame_in_flight];
  present_info.waitSemaphoreCount = 1;
  present_info.pImageIndices = &swapchainImageIndex;
  result = vkQueuePresentKHR(_backend.device.vk_graphic_queue, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    return _backend_swapchain_recreate();

  if (result != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to present rendered images");
    return DEMO_ERROR_VK;
  }

  _backend.frame_count++;

  return DEMO_ERROR_NONE;
}

void demo_renderer_wait_idle(void)
{
  _backend_device_wait_idle();
}

void demo_renderer_destroy(void)
{
  _backend_sync_destroy();
  _backend_swapchain_destroy();
  _backend_device_destroy();
  _backend_instance_destroy();
}

enum demo_error demo_renderer_init_ui(void)
{
  /* Descriptor pool */
  const uint32_t resource_count = 512;
  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, resource_count},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, resource_count},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, resource_count},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, resource_count},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, resource_count},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, resource_count},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, resource_count},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, resource_count},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, resource_count},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, resource_count},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, resource_count}};

  VkDescriptorPoolCreateInfo pool_info = {0};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = resource_count;
  pool_info.poolSizeCount = sizeof(pool_sizes) / sizeof(VkDescriptorPoolSize);
  pool_info.pPoolSizes = pool_sizes;

  if (vkCreateDescriptorPool(_backend.device.vk_device, &pool_info, NULL, &_backend.ui.vk_descriptorpool) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create descriptor pool");
    return DEMO_ERROR_VK;
  }

  /* Render Pass */
  VkAttachmentDescription color_attachment = {0};
  color_attachment.format = _backend_info.vk_surface_format.format;
  color_attachment.samples = _backend_info.sample_count;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;
  if (vkCreateRenderPass(_backend.device.vk_device, &render_pass_info, NULL, &_backend.ui.vk_renderpass) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create render pass");
    return DEMO_ERROR_VK;
  }

  /* Command Pool */
  VkCommandPoolCreateInfo commandpool_info = {0};
  commandpool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandpool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandpool_info.queueFamilyIndex = _backend.device.vk_graphic_queue_index;
  if (vkCreateCommandPool(_backend.device.vk_device, &commandpool_info, NULL, &_backend.ui.vk_commandpool) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create command pool");
    return DEMO_ERROR_VK;
  }

  /* Command Buffers */
  _backend.ui.vk_commandbuffers = malloc(_backend_info.max_frames_in_flight * sizeof(VkCommandBuffer));
  if (_backend.ui.vk_commandbuffers == NULL)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Failed to allocate memory");
    return DEMO_ERROR_MEMORY;
  }

  VkCommandBufferAllocateInfo commandbuffers_info = {0};
  commandbuffers_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandbuffers_info.commandPool = _backend.ui.vk_commandpool;
  commandbuffers_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandbuffers_info.commandBufferCount = _backend_info.max_frames_in_flight;
  if (vkAllocateCommandBuffers(_backend.device.vk_device, &commandbuffers_info, _backend.ui.vk_commandbuffers) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create command buffer");
    return DEMO_ERROR_VK;
  }

  /* Frame Buffers */
  _backend.ui.vk_framebuffers = malloc(_backend_info.max_frames_in_flight * sizeof(VkFramebuffer));
  if (_backend.ui.vk_framebuffers == NULL)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Failed to allocate memory");
    return DEMO_ERROR_MEMORY;
  }

  for (uint32_t i = 0; i < _backend_info.max_frames_in_flight; i++)
  {
    VkFramebufferCreateInfo framebuffer_info = {0};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = _backend.ui.vk_renderpass;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.width = _backend.swapchain.vk_extent.width;
    framebuffer_info.height = _backend.swapchain.vk_extent.height;
    framebuffer_info.layers = 1;
    framebuffer_info.pAttachments = &_backend.swapchain.vk_image_views[i];
    vkCreateFramebuffer(_backend.device.vk_device, &framebuffer_info, NULL, &_backend.ui.vk_framebuffers[i]);
  }

  /* ImGUI */
  ImGui_ImplVulkan_InitInfo vulkan_impl_info = {0};
  vulkan_impl_info.Instance = _backend.instance.vk_instance;
  vulkan_impl_info.PhysicalDevice = _backend.device.vk_physical_device;
  vulkan_impl_info.Device = _backend.device.vk_device;
  vulkan_impl_info.QueueFamily = _backend.device.vk_graphic_queue_index;
  vulkan_impl_info.Queue = _backend.device.vk_graphic_queue;
  vulkan_impl_info.PipelineCache = VK_NULL_HANDLE;
  vulkan_impl_info.DescriptorPool = _backend.ui.vk_descriptorpool;
  vulkan_impl_info.Allocator = NULL;
  vulkan_impl_info.MinImageCount = _backend_info.max_frames_in_flight;
  vulkan_impl_info.ImageCount = _backend_info.max_frames_in_flight;
  vulkan_impl_info.MSAASamples = _backend_info.sample_count;
  if (!ImGui_ImplVulkan_Init(&vulkan_impl_info, _backend.ui.vk_renderpass))
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to instantiate Vulkan for ImGUI");
    return DEMO_ERROR_VK;
  }

  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = _backend.ui.vk_commandpool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandbuffer;
  vkAllocateCommandBuffers(_backend.device.vk_device, &allocInfo, &commandbuffer);

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandbuffer, &beginInfo);

  ImGui_ImplVulkan_CreateFontsTexture(commandbuffer);

  vkEndCommandBuffer(commandbuffer);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandbuffer;

  vkQueueSubmit(_backend.device.vk_graphic_queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(_backend.device.vk_graphic_queue);
  vkFreeCommandBuffers(_backend.device.vk_device, _backend.ui.vk_commandpool, 1, &commandbuffer);

  return DEMO_ERROR_NONE;
}

void demo_renderer_refresh_ui(void)
{
  ImGui_ImplVulkan_NewFrame();
}

void demo_renderer_destroy_ui(void)
{
  ImGui_ImplVulkan_Shutdown();

  vkDestroyDescriptorPool(_backend.device.vk_device, _backend.ui.vk_descriptorpool, NULL);

  if (_backend.ui.vk_framebuffers)
  {
    for (uint32_t i = 0; i < _backend_info.max_frames_in_flight; i++)
    {
      vkDestroyFramebuffer(_backend.device.vk_device, _backend.ui.vk_framebuffers[i], NULL);
    }
    free(_backend.ui.vk_framebuffers);
  }

  if (_backend.ui.vk_commandbuffers)
  {
    vkFreeCommandBuffers(_backend.device.vk_device, _backend.ui.vk_commandpool, _backend_info.max_frames_in_flight, _backend.ui.vk_commandbuffers);
    free(_backend.ui.vk_commandbuffers);
  }

  vkDestroyCommandPool(_backend.device.vk_device, _backend.ui.vk_commandpool, NULL);
  vkDestroyRenderPass(_backend.device.vk_device, _backend.ui.vk_renderpass, NULL);
}

static enum demo_error _backend_instance_init(void)
{
  enum demo_error status;

  status = _backend_instance_create_instance();
  if (status != DEMO_ERROR_NONE)
    return status;

#ifndef NDEBUG
  status = _backend_instance_create_debug_instance();
  if (status != DEMO_ERROR_NONE)
    return status;
#endif

  status = _backend_instance_create_surface();
  if (status != DEMO_ERROR_NONE)
    return status;

  return DEMO_ERROR_NONE;
}

static enum demo_error _backend_instance_create_instance(void)
{
  VkApplicationInfo vk_app_info = {0};
  vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  vk_app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info = {0};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &vk_app_info;

  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions;
  glfw_extensions = demo_window_get_required_extensions(&glfw_extension_count);

#ifdef NDEBUG
  create_info.enabledExtensionCount = glfw_extension_count;
  create_info.ppEnabledExtensionNames = glfw_extensions;
  create_info.enabledLayerCount = 0;
#else
  uint32_t all_extension_count = glfw_extension_count + sizeof(_backend_info.debug_extensions) / sizeof(char *);
  const char *all_extensions[all_extension_count];
  for (uint32_t i = 0; i < glfw_extension_count; i++)
    all_extensions[i] = glfw_extensions[i];
  for (uint32_t i = 0; i < sizeof(_backend_info.debug_extensions) / sizeof(char *); i++)
    all_extensions[glfw_extension_count + i] = _backend_info.debug_extensions[i];

  create_info.enabledExtensionCount = all_extension_count;
  create_info.ppEnabledExtensionNames = (const char **)all_extensions;
  create_info.enabledLayerCount = sizeof(_backend_info.validation_layers) / sizeof(char *);
  create_info.ppEnabledLayerNames = _backend_info.validation_layers;

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
  debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debug_create_info.pfnUserCallback = _backend_instance_debug_callback;
  debug_create_info.pUserData = NULL;

  create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
#endif

  if (vkCreateInstance(&create_info, NULL, &_backend.instance.vk_instance) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create instance");
    return DEMO_ERROR_VK;
  }

  return DEMO_ERROR_NONE;
}

static enum demo_error _backend_instance_create_surface(void)
{
  return demo_window_create_surface(&_backend.instance.vk_instance, &_backend.instance.vk_surface);
}

#ifndef NDEBUG
static enum demo_error _backend_instance_create_debug_instance(void)
{
  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
  debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debug_create_info.pfnUserCallback = _backend_instance_debug_callback;
  debug_create_info.pUserData = NULL;

  PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      _backend.instance.vk_instance,
      "vkCreateDebugUtilsMessengerEXT");

  if (!func)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to set up debug messenger");
    return DEMO_ERROR_VK;
  }

  if (func(_backend.instance.vk_instance, &debug_create_info, NULL, &_backend.instance.vk_debug_messenger) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to set up debug messenger");
    return DEMO_ERROR_VK;
  }

  return DEMO_ERROR_NONE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL _backend_instance_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *cb_data,
    void *user_data)
{
  SAKE_MACRO_UNUSED(message_severity);
  SAKE_MACRO_UNUSED(message_type);
  SAKE_MACRO_UNUSED(cb_data);
  SAKE_MACRO_UNUSED(user_data);

  DEMO_LOG_DEBUG("[VULKAN] %s\n", cb_data->pMessage);
  return VK_FALSE;
}
#endif

static void _backend_instance_destroy(void)
{
#ifndef NDEBUG
  PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      _backend.instance.vk_instance,
      "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL)
    func(_backend.instance.vk_instance, _backend.instance.vk_debug_messenger, NULL);
#endif
  vkDestroySurfaceKHR(_backend.instance.vk_instance, _backend.instance.vk_surface, NULL);
  vkDestroyInstance(_backend.instance.vk_instance, NULL);
}

static enum demo_error _backend_device_init(void)
{
  enum demo_error status;

  status = _backend_device_pick_physical_device();
  if (status != DEMO_ERROR_NONE)
  {
    return status;
  }
  status = _backend_device_create_logical_device();
  if (status != DEMO_ERROR_NONE)
  {
    return status;
  };

  return DEMO_ERROR_NONE;
}

static enum demo_error _backend_device_pick_physical_device(void)
{
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(_backend.instance.vk_instance, &device_count, NULL);
  if (!device_count)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to find GPUs with Vulkan support");
    return DEMO_ERROR_VK;
  }

  DEMO_LOG_DEBUG("Device count: %u\n", device_count);
  VkPhysicalDevice devices[device_count];
  vkEnumeratePhysicalDevices(_backend.instance.vk_instance, &device_count, devices);

  for (uint32_t i = 0; i < device_count; i++)
  {
    if (_backend_device_is_physical_device_suitable(devices[i]) == true)
    {
      _backend.device.vk_physical_device = devices[i];
      vkGetPhysicalDeviceProperties(_backend.device.vk_physical_device, &_backend.device.vk_physical_device_properties);
      DEMO_LOG_DEBUG("Physical device: %s\n", _backend.device.vk_physical_device_properties.deviceName);
      return DEMO_ERROR_NONE;
    }
  }

  DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to find a suitable GPU");
  return DEMO_ERROR_VK;
}

static bool _backend_device_is_physical_device_suitable(VkPhysicalDevice vk_physical_device)
{
  if (_backend_device_graphic_queue_index(vk_physical_device) < 0)
    return false;

  if (_backend_device_present_queue_index(vk_physical_device, _backend.instance.vk_surface) < 0)
    return false;

  if (!_backend_device_check_extension_support(vk_physical_device,
                                               _backend_info.device_extensions,
                                               sizeof(_backend_info.device_extensions) / sizeof(char *)))
    return false;

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, _backend.instance.vk_surface, &format_count, NULL);
  if (!format_count)
    return false;

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, _backend.instance.vk_surface, &present_mode_count, NULL);
  if (!present_mode_count)
    return false;

  VkFormatProperties format_properties;
  vkGetPhysicalDeviceFormatProperties(vk_physical_device, _backend_info.vk_surface_format.format, &format_properties);
  if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
    return false;

  vkGetPhysicalDeviceFormatProperties(vk_physical_device, VK_FORMAT_R8G8B8A8_UNORM, &format_properties);
  if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
    return false;

  VkPhysicalDeviceFeatures supported;
  vkGetPhysicalDeviceFeatures(vk_physical_device, &supported);
  if (!_backend_device_check_feature_support(supported, _backend_info.features_requested))
    return false;

  return true;
}

static int32_t _backend_device_graphic_queue_index(VkPhysicalDevice physical_device)
{
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
  VkQueueFamilyProperties queue_families[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

  for (uint32_t i = 0; i < queue_family_count; i++)
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      return i;
  return -1;
}

static int32_t _backend_device_present_queue_index(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
  VkQueueFamilyProperties queue_families[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

  for (uint32_t i = 0; i < queue_family_count; i++)
  {
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
    if (present_support == VK_TRUE)
      return i;
  }
  return -1;
}

static bool _backend_device_check_extension_support(VkPhysicalDevice physical_device,
                                                    const char **desired_extensions,
                                                    uint32_t desired_extension_count)
{
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, NULL);
  VkExtensionProperties available_extensions[extension_count];
  vkEnumerateDeviceExtensionProperties(
      physical_device,
      NULL,
      &extension_count,
      available_extensions);

  for (uint32_t i = 0; i < desired_extension_count; i++)
  {
    uint32_t j = 0;
    while (j < extension_count && strcmp(desired_extensions[i], available_extensions[j].extensionName) != 0)
      j++;
    if (strcmp(desired_extensions[i], available_extensions[j].extensionName) == 0)
      DEMO_LOG_DEBUG("Extension device '%s' found\n", desired_extensions[i]);
    else
      return false;
  }

  return true;
}

static bool _backend_device_check_feature_support(VkPhysicalDeviceFeatures supported, VkPhysicalDeviceFeatures requested)
{
  if (requested.robustBufferAccess && !supported.robustBufferAccess)
    return false;
  if (requested.fullDrawIndexUint32 && !supported.fullDrawIndexUint32)
    return false;
  if (requested.imageCubeArray && !supported.imageCubeArray)
    return false;
  if (requested.independentBlend && !supported.independentBlend)
    return false;
  if (requested.geometryShader && !supported.geometryShader)
    return false;
  if (requested.tessellationShader && !supported.tessellationShader)
    return false;
  if (requested.sampleRateShading && !supported.sampleRateShading)
    return false;
  if (requested.dualSrcBlend && !supported.dualSrcBlend)
    return false;
  if (requested.logicOp && !supported.logicOp)
    return false;
  if (requested.multiDrawIndirect && !supported.multiDrawIndirect)
    return false;
  if (requested.drawIndirectFirstInstance && !supported.drawIndirectFirstInstance)
    return false;
  if (requested.depthClamp && !supported.depthClamp)
    return false;
  if (requested.depthBiasClamp && !supported.depthBiasClamp)
    return false;
  if (requested.fillModeNonSolid && !supported.fillModeNonSolid)
    return false;
  if (requested.depthBounds && !supported.depthBounds)
    return false;
  if (requested.wideLines && !supported.wideLines)
    return false;
  if (requested.largePoints && !supported.largePoints)
    return false;
  if (requested.alphaToOne && !supported.alphaToOne)
    return false;
  if (requested.multiViewport && !supported.multiViewport)
    return false;
  if (requested.samplerAnisotropy && !supported.samplerAnisotropy)
    return false;
  if (requested.textureCompressionETC2 && !supported.textureCompressionETC2)
    return false;
  if (requested.textureCompressionASTC_LDR && !supported.textureCompressionASTC_LDR)
    return false;
  if (requested.textureCompressionBC && !supported.textureCompressionBC)
    return false;
  if (requested.occlusionQueryPrecise && !supported.occlusionQueryPrecise)
    return false;
  if (requested.pipelineStatisticsQuery && !supported.pipelineStatisticsQuery)
    return false;
  if (requested.vertexPipelineStoresAndAtomics && !supported.vertexPipelineStoresAndAtomics)
    return false;
  if (requested.fragmentStoresAndAtomics && !supported.fragmentStoresAndAtomics)
    return false;
  if (requested.shaderTessellationAndGeometryPointSize && !supported.shaderTessellationAndGeometryPointSize)
    return false;
  if (requested.shaderImageGatherExtended && !supported.shaderImageGatherExtended)
    return false;
  if (requested.shaderStorageImageExtendedFormats && !supported.shaderStorageImageExtendedFormats)
    return false;
  if (requested.shaderStorageImageMultisample && !supported.shaderStorageImageMultisample)
    return false;
  if (requested.shaderStorageImageReadWithoutFormat && !supported.shaderStorageImageReadWithoutFormat)
    return false;
  if (requested.shaderStorageImageWriteWithoutFormat && !supported.shaderStorageImageWriteWithoutFormat)
    return false;
  if (requested.shaderUniformBufferArrayDynamicIndexing && !supported.shaderUniformBufferArrayDynamicIndexing)
    return false;
  if (requested.shaderSampledImageArrayDynamicIndexing && !supported.shaderSampledImageArrayDynamicIndexing)
    return false;
  if (requested.shaderStorageBufferArrayDynamicIndexing && !supported.shaderStorageBufferArrayDynamicIndexing)
    return false;
  if (requested.shaderStorageImageArrayDynamicIndexing && !supported.shaderStorageImageArrayDynamicIndexing)
    return false;
  if (requested.shaderClipDistance && !supported.shaderClipDistance)
    return false;
  if (requested.shaderCullDistance && !supported.shaderCullDistance)
    return false;
  if (requested.shaderFloat64 && !supported.shaderFloat64)
    return false;
  if (requested.shaderInt64 && !supported.shaderInt64)
    return false;
  if (requested.shaderInt16 && !supported.shaderInt16)
    return false;
  if (requested.shaderResourceResidency && !supported.shaderResourceResidency)
    return false;
  if (requested.shaderResourceMinLod && !supported.shaderResourceMinLod)
    return false;
  if (requested.sparseBinding && !supported.sparseBinding)
    return false;
  if (requested.sparseResidencyBuffer && !supported.sparseResidencyBuffer)
    return false;
  if (requested.sparseResidencyImage2D && !supported.sparseResidencyImage2D)
    return false;
  if (requested.sparseResidencyImage3D && !supported.sparseResidencyImage3D)
    return false;
  if (requested.sparseResidency2Samples && !supported.sparseResidency2Samples)
    return false;
  if (requested.sparseResidency4Samples && !supported.sparseResidency4Samples)
    return false;
  if (requested.sparseResidency8Samples && !supported.sparseResidency8Samples)
    return false;
  if (requested.sparseResidency16Samples && !supported.sparseResidency16Samples)
    return false;
  if (requested.sparseResidencyAliased && !supported.sparseResidencyAliased)
    return false;
  if (requested.variableMultisampleRate && !supported.variableMultisampleRate)
    return false;
  if (requested.inheritedQueries && !supported.inheritedQueries)
    return false;
  return true;
}

static enum demo_error _backend_device_create_logical_device(void)
{
  int32_t graphic_queue, present_queue;
  graphic_queue = _backend_device_graphic_queue_index(_backend.device.vk_physical_device);
  present_queue = _backend_device_present_queue_index(_backend.device.vk_physical_device, _backend.instance.vk_surface);

  int32_t queues[] = {graphic_queue, present_queue};
  int32_t nb_queue = graphic_queue != present_queue ? 2 : 1;
  VkDeviceQueueCreateInfo vk_queue_create_info[nb_queue];
  float queue_priority = 1.0f;
  for (int32_t i = 0; i < nb_queue; i++)
  {
    vk_queue_create_info[i] = (VkDeviceQueueCreateInfo){0};
    vk_queue_create_info[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    vk_queue_create_info[i].queueFamilyIndex = queues[i];
    vk_queue_create_info[i].queueCount = 1;
    vk_queue_create_info[i].pQueuePriorities = &queue_priority;
  }

  VkDeviceCreateInfo create_info = {0};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = nb_queue;
  create_info.pQueueCreateInfos = (const VkDeviceQueueCreateInfo *)&vk_queue_create_info;
  create_info.pEnabledFeatures = &_backend_info.features_requested;
  create_info.enabledExtensionCount = sizeof(_backend_info.device_extensions) / sizeof(char *);
  create_info.ppEnabledExtensionNames = _backend_info.device_extensions;

#ifdef NDEBUG
  create_info.enabledLayerCount = 0;
#else
  create_info.enabledLayerCount = sizeof(_backend_info.validation_layers) / sizeof(char *);
  create_info.ppEnabledLayerNames = _backend_info.validation_layers;
#endif

  if (vkCreateDevice(_backend.device.vk_physical_device, &create_info, NULL, &_backend.device.vk_device) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create logical device");
    return DEMO_ERROR_VK;
  }

  vkGetDeviceQueue(_backend.device.vk_device, graphic_queue, 0, &_backend.device.vk_graphic_queue);
  vkGetDeviceQueue(_backend.device.vk_device, present_queue, 0, &_backend.device.vk_present_queue);
  _backend.device.vk_graphic_queue_index = graphic_queue;
  _backend.device.vk_present_queue_index = present_queue;

  return DEMO_ERROR_NONE;
}

static void _backend_device_destroy(void)
{
  vkDestroyDevice(_backend.device.vk_device, NULL);
}

static void _backend_device_wait_idle(void)
{
  vkDeviceWaitIdle(_backend.device.vk_device);
}

static enum demo_error _backend_swapchain_init(void)
{
  if (!_backend_swapchain_check_surface_format_support())
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "No swapchain support for desired surface format");
    return DEMO_ERROR_VK;
  }

  if (!_backend_swapchain_check_present_mode_support())
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "No swapchain support for desired present mode");
    return DEMO_ERROR_VK;
  }

  if (!_backend_swapchain_check_image_count_support())
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "No swapchain support for desired image count");
    return DEMO_ERROR_VK;
  }

  if (!_backend_swapchain_check_array_layer_count_support())
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "No swapchain support for desired array layer count");
    return DEMO_ERROR_VK;
  }

  _backend.swapchain.vk_extent = _backend_swapchain_get_current_extent();

  VkSwapchainCreateInfoKHR swapchain_create_info = {0};
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.flags = _backend_info.create_flags;
  swapchain_create_info.surface = _backend.instance.vk_surface;
  swapchain_create_info.minImageCount = _backend_info.max_frames_in_flight;
  swapchain_create_info.imageFormat = _backend_info.vk_surface_format.format;
  swapchain_create_info.imageColorSpace = _backend_info.vk_surface_format.colorSpace;
  swapchain_create_info.imageExtent = _backend.swapchain.vk_extent;
  swapchain_create_info.imageArrayLayers = _backend_info.array_layer_count;
  swapchain_create_info.imageUsage = _backend_info.image_usage_flags;

  uint32_t queue_family_indices[] = {_backend.device.vk_graphic_queue_index, _backend.device.vk_present_queue_index};
  if (_backend.device.vk_graphic_queue_index != _backend.device.vk_present_queue_index)
  {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_create_info.queueFamilyIndexCount = 2;
    swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
  }
  else
  {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_backend.device.vk_physical_device,
                                            _backend.instance.vk_surface,
                                            &capabilities);

  swapchain_create_info.preTransform = _backend_info.transform ? _backend_info.transform : capabilities.currentTransform;
  swapchain_create_info.compositeAlpha = _backend_info.composite_alpha;
  swapchain_create_info.presentMode = _backend_info.vk_present_mode;
  swapchain_create_info.clipped = _backend_info.clipped;
  swapchain_create_info.oldSwapchain = NULL;
  if (vkCreateSwapchainKHR(_backend.device.vk_device, &swapchain_create_info, NULL, &_backend.swapchain.vk_swapchain) != VK_SUCCESS)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create swap chain");
    return DEMO_ERROR_VK;
  }

  vkGetSwapchainImagesKHR(_backend.device.vk_device, _backend.swapchain.vk_swapchain, &_backend_info.max_frames_in_flight, NULL);

  _backend.swapchain.vk_images = malloc(_backend_info.max_frames_in_flight * sizeof(VkImage));
  if (_backend.swapchain.vk_images == NULL)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Failed to allocate memory");
    return DEMO_ERROR_MEMORY;
  }

  vkGetSwapchainImagesKHR(_backend.device.vk_device, _backend.swapchain.vk_swapchain, &_backend_info.max_frames_in_flight, _backend.swapchain.vk_images);

  _backend.swapchain.vk_image_views = malloc(_backend_info.max_frames_in_flight * sizeof(VkImageView));
  if (_backend.swapchain.vk_image_views == NULL)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Failed to allocate memory");
    return DEMO_ERROR_MEMORY;
  }

  for (uint32_t i = 0; i < _backend_info.max_frames_in_flight; i++)
  {
    VkImageViewCreateInfo imageview_createinfo = {0};
    imageview_createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageview_createinfo.image = _backend.swapchain.vk_images[i];
    imageview_createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageview_createinfo.format = _backend_info.vk_surface_format.format;
    imageview_createinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageview_createinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageview_createinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageview_createinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageview_createinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageview_createinfo.subresourceRange.baseMipLevel = 0;
    imageview_createinfo.subresourceRange.levelCount = 1;
    imageview_createinfo.subresourceRange.baseArrayLayer = 0;
    imageview_createinfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(_backend.device.vk_device, &imageview_createinfo, NULL, &_backend.swapchain.vk_image_views[i]) != VK_SUCCESS)
    {
      DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create image view");
      return DEMO_ERROR_VK;
    }
  }

  return DEMO_ERROR_NONE;
}

static bool _backend_swapchain_check_surface_format_support(void)
{
  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(_backend.device.vk_physical_device,
                                       _backend.instance.vk_surface,
                                       &format_count,
                                       NULL);

  VkSurfaceFormatKHR formats[format_count];
  vkGetPhysicalDeviceSurfaceFormatsKHR(_backend.device.vk_physical_device,
                                       _backend.instance.vk_surface,
                                       &format_count,
                                       formats);

  for (uint32_t i = 0; i < format_count; i++)
  {
    if ((formats[i].colorSpace == _backend_info.vk_surface_format.colorSpace) &&
        (formats[i].format == _backend_info.vk_surface_format.format))
      return true;
  }

  return false;
}

static bool _backend_swapchain_check_present_mode_support(void)
{
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(_backend.device.vk_physical_device,
                                            _backend.instance.vk_surface,
                                            &present_mode_count,
                                            NULL);

  VkPresentModeKHR present_modes[present_mode_count];
  vkGetPhysicalDeviceSurfacePresentModesKHR(_backend.device.vk_physical_device,
                                            _backend.instance.vk_surface,
                                            &present_mode_count,
                                            present_modes);

  for (uint32_t i = 0; i < present_mode_count; i++)
  {
    if (present_modes[i] == _backend_info.vk_present_mode)
      return true;
  }

  return false;
}

static bool _backend_swapchain_check_image_count_support(void)
{
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_backend.device.vk_physical_device,
                                            _backend.instance.vk_surface,
                                            &capabilities);
  bool supported_min_image_count = _backend_info.max_frames_in_flight >= capabilities.minImageCount;
  bool supported_max_image_count = !capabilities.maxImageCount || _backend_info.max_frames_in_flight <= capabilities.maxImageCount;
  return supported_max_image_count && supported_min_image_count;
}

static bool _backend_swapchain_check_array_layer_count_support(void)
{
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_backend.device.vk_physical_device,
                                            _backend.instance.vk_surface,
                                            &capabilities);
  return _backend_info.array_layer_count <= capabilities.maxImageArrayLayers;
}

static VkExtent2D _backend_swapchain_get_current_extent(void)
{
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_backend.device.vk_physical_device,
                                            _backend.instance.vk_surface,
                                            &capabilities);

  if (capabilities.currentExtent.width != UINT_MAX)
  {
    return capabilities.currentExtent;
  }
  else
  {
    int width, height;
    demo_window_get_extent(&width, &height);
    VkExtent2D extent = {.height = height, .width = width};
    extent.width = fmax(capabilities.minImageExtent.width,
                        fmin(capabilities.maxImageExtent.width, extent.width));
    extent.height = fmax(capabilities.minImageExtent.height,
                         fmin(capabilities.maxImageExtent.height, extent.height));
    return extent;
  }
}

static enum demo_error _backend_swapchain_recreate(void)
{
  enum demo_error status;
  int width, height;
  demo_window_get_extent(&width, &height);
  while (height == 0 || width == 0)
  {
    demo_window_get_extent(&width, &height);
    demo_window_wait_events();
  }

  _backend_device_wait_idle();

  if (_backend.ui.vk_framebuffers)
  {
    for (uint32_t i = 0; i < _backend_info.max_frames_in_flight; i++)
      vkDestroyFramebuffer(_backend.device.vk_device, _backend.ui.vk_framebuffers[i], NULL);
    free(_backend.ui.vk_framebuffers);
  }

  _backend_swapchain_destroy();

  status = _backend_swapchain_init();
  if (status != DEMO_ERROR_NONE)
    return status;

  _backend.ui.vk_framebuffers = malloc(_backend_info.max_frames_in_flight * sizeof(VkFramebuffer));
  if (_backend.ui.vk_framebuffers == NULL)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Failed to allocate memory");
    return DEMO_ERROR_MEMORY;
  }

  for (uint32_t i = 0; i < _backend_info.max_frames_in_flight; i++)
  {
    VkFramebufferCreateInfo framebuffer_info = {0};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = _backend.ui.vk_renderpass;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.width = _backend.swapchain.vk_extent.width;
    framebuffer_info.height = _backend.swapchain.vk_extent.height;
    framebuffer_info.layers = 1;
    framebuffer_info.pAttachments = &_backend.swapchain.vk_image_views[i];
    vkCreateFramebuffer(_backend.device.vk_device, &framebuffer_info, NULL, &_backend.ui.vk_framebuffers[i]);
  }

  return DEMO_ERROR_NONE;
}

static void _backend_swapchain_destroy(void)
{
  if (_backend.swapchain.vk_image_views)
  {
    for (uint32_t i = 0; i < _backend_info.max_frames_in_flight; i++)
      vkDestroyImageView(_backend.device.vk_device, _backend.swapchain.vk_image_views[i], NULL);
    free(_backend.swapchain.vk_image_views);
  }

  if (_backend.swapchain.vk_images)
    free(_backend.swapchain.vk_images);

  vkDestroySwapchainKHR(_backend.device.vk_device, _backend.swapchain.vk_swapchain, NULL);
}

static enum demo_error _backend_sync_init(void)
{
  _backend.sync.vk_render_fence = malloc(_backend_info.max_frames_in_flight * sizeof(VkFence));
  if (_backend.sync.vk_render_fence == NULL)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Failed to allocate memory");
    return DEMO_ERROR_MEMORY;
  }

  _backend.sync.vk_render_semaphore = malloc(_backend_info.max_frames_in_flight * sizeof(VkSemaphore));
  if (_backend.sync.vk_render_semaphore == NULL)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Failed to allocate memory");
    return DEMO_ERROR_MEMORY;
  }

  _backend.sync.vk_present_semaphore = malloc(_backend_info.max_frames_in_flight * sizeof(VkSemaphore));
  if (_backend.sync.vk_present_semaphore == NULL)
  {
    DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Failed to allocate memory");
    return DEMO_ERROR_MEMORY;
  }

  VkFenceCreateInfo fence_info = {0};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  VkSemaphoreCreateInfo semaphore_info = {0};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  for (uint32_t i = 0; i < _backend_info.max_frames_in_flight; i++)
  {
    if (vkCreateFence(_backend.device.vk_device, &fence_info, NULL, &_backend.sync.vk_render_fence[i]) != VK_SUCCESS)
    {
      DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create render fence");
      return DEMO_ERROR_VK;
    }
    if (vkCreateSemaphore(_backend.device.vk_device, &semaphore_info, NULL, &_backend.sync.vk_render_semaphore[i]) != VK_SUCCESS)
    {
      DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create render semaphore");
      return DEMO_ERROR_VK;
    }
    if (vkCreateSemaphore(_backend.device.vk_device, &semaphore_info, NULL, &_backend.sync.vk_present_semaphore[i]) != VK_SUCCESS)
    {
      DEMO_LOG_ERROR(DEMO_ERROR_VK, "Failed to create present semaphore");
      return DEMO_ERROR_VK;
    }
  }

  return DEMO_ERROR_NONE;
}

static void _backend_sync_destroy(void)
{
  for (uint32_t i = 0; i < _backend_info.max_frames_in_flight; i++)
  {
    vkDestroyFence(_backend.device.vk_device, _backend.sync.vk_render_fence[i], NULL);
    vkDestroySemaphore(_backend.device.vk_device, _backend.sync.vk_render_semaphore[i], NULL);
    vkDestroySemaphore(_backend.device.vk_device, _backend.sync.vk_present_semaphore[i], NULL);
  }
}