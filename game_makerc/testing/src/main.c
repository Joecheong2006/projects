#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include <string.h>

#include "game_test.h"
#include "core/log.h"
#include "core/assert.h"

#include "basic/memallocate.h"

#include "application_setup.h"
#include "entry_point.h"

#include "basic/vector.h"
#include "basic/string.h"

typedef struct {
    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSwapchainKHR swap_chain;
    vector(VkImage) swap_chain_images;
    vector(VkImageView) swap_chain_image_views;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;

    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;

    VkDebugUtilsMessengerEXT debug_messenger;
} vulkan_test;

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_ERROR("\t%s\n", pCallbackData->pMessage);
    }
    else {
        LOG_DEBUG("\t%s\n", pCallbackData->pMessage);
    }
        
    return VK_FALSE;
}

static i32 enable_validation_layers(const char** layer_names, u32 layers_count) {
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    VkLayerProperties layers[layer_count];
    vkEnumerateInstanceLayerProperties(&layer_count, layers);

    for (u32 i = 0; i < layers_count; i++) {
        i32 layer_founded = 0;
        for (u32 j = 0; j < layer_count; j++) {
            if (strcmp(layers[j].layerName, layer_names[i]) == 0) {
                layer_founded = 1;
                break;
            }
        }
        if (!layer_founded) {
            return 0;
        }
            
    }
    return 1;
}

static const vector(cstring) get_vulkan_required_instance_extensions() {
    const vector(cstring) result = make_vector();

    u32 glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    for (u32 i = 0; i < glfw_extension_count; i++) {
        vector_push(result, make_string(glfw_extensions[i]));
    }

    return result;
}

static void populate_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT* create_info);
static i32 create_vulkan_instance(vulkan_test* vt, const char** validation_layer_names, u32 validation_layer_count) {
    VkApplicationInfo app_info;
    memset(&app_info, 0, sizeof(VkApplicationInfo));
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "test";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info;
    memset(&create_info, 0, sizeof(VkInstanceCreateInfo));
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    i32 validation_layers_available = 0;
    if (validation_layer_names) {
        validation_layers_available = enable_validation_layers(validation_layer_names, validation_layer_count);
    }

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    const vector(cstring) glfw_extensions = get_vulkan_required_instance_extensions();
    if (validation_layers_available) {
        create_info.enabledLayerCount = validation_layer_count;
        create_info.ppEnabledLayerNames = validation_layer_names;
        populate_debug_create_info(&debug_create_info);
        create_info.pNext = &debug_create_info;

        vector_push(glfw_extensions, make_string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
    }

    const u32 glfw_extension_count = vector_size(glfw_extensions);
    
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;

    LOG_TRACE("\t%s\n", validation_layers_available ? "validation layers are available" : "validation layers are not available");

    if (vkCreateInstance(&create_info, NULL, &vt->instance) != VK_SUCCESS) {
        return 0;
    }

    for (u32 i = 0; i < glfw_extension_count; i++) {
        free_string(glfw_extensions[i]);
    }
    free_vector(glfw_extensions);
    return 1;
}


static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func) {
        func(instance, debugMessenger, pAllocator);
    }
}

static void populate_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT* create_info) {
    memset(create_info, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
    create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info->pfnUserCallback = debug_callback;
    create_info->pUserData = NULL;
}

static i32 setup_vulkan_debug_messenger(vulkan_test* vt) {
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_create_info(&create_info);

    if (CreateDebugUtilsMessengerEXT(vt->instance, &create_info, NULL, &vt->debug_messenger) == VK_SUCCESS) {
        return 1;
    }

    return 0;
}

static i32 create_vulkan_surface(vulkan_test* vt) {
    if (glfwCreateWindowSurface(vt->instance, vt->window, NULL, &vt->surface) != VK_SUCCESS) {
        return 0;
    }
    return 1;
}

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    vector(VkSurfaceFormatKHR) formats;
    vector(VkPresentModeKHR) present_modes;
} swap_chain_support_details;

static VkSurfaceFormatKHR choose_swap_surface_format(const vector(VkSurfaceFormatKHR) formats) {
    for_vector(formats, i, 0) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }
    return formats[0];
}

static VkPresentModeKHR choose_swap_surface_present_mode(const vector(VkPresentModeKHR) present_modes) {
    for_vector(present_modes, i, 0) {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_modes[i];
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR* capabilities, GLFWwindow* window) {
    if (capabilities->currentExtent.width != (u32)(-1)) {
        return capabilities->currentExtent;
    }
    i32 w, h;
    glfwGetFramebufferSize(window, &w, &h);
    VkExtent2D extent;
    extent.width = glm_clamp((u32)w, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
    extent.height = glm_clamp((u32)h, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);
    return extent;
}

static swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
    swap_chain_support_details details = { .formats = make_vector(), .present_modes = make_vector() };
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    u32 format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);
    if (format_count != 0) {
        vector_resize(details.formats, format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats);
    }

    u32 present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, NULL);
    if (present_mode_count != 0) {
        vector_resize(details.present_modes, present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes);
    }
        
    return details;
}

typedef struct { u32 index; i32 exist; } queue_index;
typedef struct {
    queue_index graphics_family;
    queue_index present_family;
} queue_family_indices;

i32 queue_family_fulfil_requirement(queue_family_indices* indices) {
    return indices->graphics_family.exist & indices->present_family.exist;
}

queue_family_indices find_required_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface) {
    queue_family_indices indices;
    memset(&indices, 0, sizeof(queue_family_indices));

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
    VkQueueFamilyProperties properties[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, properties);
    for (u32 i = 0; i < queue_family_count; i++) {
        if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family.index = i;
            indices.graphics_family.exist = 1;
        }
        VkBool32 present_support = 0;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support) {
            indices.present_family.index = i;
            indices.present_family.exist = 1;
        }
        if (queue_family_fulfil_requirement(&indices)) {
            break;
        }
    }

    return indices;
}

static i32 check_device_extension_support(VkPhysicalDevice device, const char** extension_names, u32 extension_count) {
    u32 query_extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &query_extension_count, NULL);
    VkExtensionProperties query_extensions[query_extension_count];
    vkEnumerateDeviceExtensionProperties(device, NULL, &query_extension_count, query_extensions);

    for (u32 i = 0; i < extension_count; i++) {
        i32 extension_found = 0;
        for (u32 j = 0; j < query_extension_count; j++) {
            if (strcmp(extension_names[i], query_extensions[j].extensionName) == 0) {
                extension_found = 1;
                break;
            }
        }
        if (!extension_found) {
            return 0;
        }
    }

    return 1;
}

static i32 is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface, const char** extension_names, u32 extension_count) {
    queue_family_indices indices = find_required_queue_families(device, surface);
    i32 extensions_supported = check_device_extension_support(device, extension_names, extension_count);
    i32 swap_chain_adequate = 0;
    if (extensions_supported) {
        swap_chain_support_details details = query_swap_chain_support(device, surface);
        swap_chain_adequate = !vector_is_empty(details.formats) && !vector_is_empty(details.present_modes);
        free_vector(details.formats);
        free_vector(details.present_modes);
    }
    return queue_family_fulfil_requirement(&indices) && extensions_supported && swap_chain_adequate;
}

static void select_supported_physical_device(vulkan_test* vt, const char** extension_names, u32 extension_count) {
    vt->physical_device = VK_NULL_HANDLE;

    u32 device_count = 0;
    vkEnumeratePhysicalDevices(vt->instance, &device_count, NULL);

    if (device_count == 0) {
        return;
    }
    VkPhysicalDevice devices[device_count];
    vkEnumeratePhysicalDevices(vt->instance, &device_count, devices);

    for (u32 i = 0; i < device_count; i++) {
        if (is_device_suitable(devices[i], vt->surface, extension_names, extension_count)) {
            vt->physical_device = devices[i];
            break;
        }
    }
}

static i32 create_logical_device(vulkan_test* vt, const char** validation_layer_names, u32 validation_layer_count, const char** extension_names, u32 extension_count) {
    queue_family_indices indices = find_required_queue_families(vt->physical_device, vt->surface);

    u32 queue_create_info_count = sizeof(queue_family_indices) / sizeof(queue_index);
    VkDeviceQueueCreateInfo queue_create_infos[queue_create_info_count];
    u32 unique_queue_families[] = { indices.graphics_family.index, indices.present_family.index };

    for (u32 i = 0; i < queue_create_info_count; i++) {
        for (u32 j = i + 1; j < queue_create_info_count;) {
            if (unique_queue_families[i] == unique_queue_families[j]) {
                unique_queue_families[j] = unique_queue_families[--queue_create_info_count];
                continue;
            }
            j++;
        }
    }

    f32 queue_priority = 1.0f;
    for (u32 i = 0; i < queue_create_info_count; i++) {
        VkDeviceQueueCreateInfo queue_create_info;
        memset(&queue_create_info, 0, sizeof(VkDeviceQueueCreateInfo));
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = unique_queue_families[i];
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos[i] = queue_create_info;
    }

    VkPhysicalDeviceFeatures device_features;
    memset(&device_features, VK_FALSE, sizeof(VkPhysicalDeviceFeatures));

    VkDeviceCreateInfo create_info;
    memset(&create_info, 0, sizeof(VkDeviceCreateInfo));
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos;
    create_info.queueCreateInfoCount = queue_create_info_count;
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = extension_count;
    create_info.ppEnabledExtensionNames = extension_names;

    if (validation_layer_names) {
        create_info.enabledLayerCount = validation_layer_count;
        create_info.ppEnabledLayerNames = validation_layer_names;
    }

    if (vkCreateDevice(vt->physical_device, &create_info, NULL, &vt->device) != VK_SUCCESS) {
        return 0;
    }

    vkGetDeviceQueue(vt->device, indices.graphics_family.index, 0, &vt->graphics_queue);
    vkGetDeviceQueue(vt->device, indices.present_family.index, 0, &vt->present_queue);
    
    return 1;
}

static i32 create_swap_chain(vulkan_test* vt) {
    swap_chain_support_details details = query_swap_chain_support(vt->physical_device, vt->surface);

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(details.formats);
    VkPresentModeKHR present_mode = choose_swap_surface_present_mode(details.present_modes);
    VkExtent2D extent = choose_swap_extent(&details.capabilities, vt->window);

    u32 image_count = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount) {
        image_count = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info;
    memset(&create_info, 0, sizeof(VkSwapchainCreateInfoKHR));
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.surface = vt->surface;

    queue_family_indices indices = find_required_queue_families(vt->physical_device, vt->surface);
    u32 queue_indices[] = { indices.graphics_family.index, indices.present_family.index };

    if (indices.graphics_family.index != indices.present_family.index) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_indices;
    }
    else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = NULL;
    }

    create_info.preTransform = details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vt->device, &create_info, NULL, &vt->swap_chain) != VK_SUCCESS) {
        return 0;
    }

    vkGetSwapchainImagesKHR(vt->device, vt->swap_chain, &image_count, NULL);
    vector_resize(vt->swap_chain_images, image_count);
    vkGetSwapchainImagesKHR(vt->device, vt->swap_chain, &image_count, vt->swap_chain_images);
    vt->swap_chain_image_format = surface_format.format;
    vt->swap_chain_extent = extent;

    return 1;
}

static i32 create_image_views(vulkan_test* vt) {
    vector_resize(vt->swap_chain_image_views, vector_size(vt->swap_chain_images));

    for_vector(vt->swap_chain_image_views, i, 0) {
        VkImageViewCreateInfo create_info;
        memset(&create_info, 0, sizeof(VkImageViewCreateInfo));
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = vt->swap_chain_images[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = vt->swap_chain_image_format;

        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount= 1;

        if (vkCreateImageView(vt->device, &create_info, NULL, &vt->swap_chain_image_views[i]) != VK_SUCCESS) {
            return 0;
        }
    }
    return 1;
}

static i32 create_render_pass(vulkan_test* vt) {
    VkAttachmentDescription color_attachment;
    memset(&color_attachment, 0, sizeof(VkAttachmentDescription));
    color_attachment.format = vt->swap_chain_image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref;
    memset(&color_attachment_ref, 0, sizeof(VkAttachmentReference));
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    memset(&subpass, 0, sizeof(VkSubpassDescription));
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info;
    memset(&render_pass_info, 0, sizeof(VkRenderPassCreateInfo));
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    if (vkCreateRenderPass(vt->device, &render_pass_info, NULL, &vt->render_pass) != VK_SUCCESS) {
        return 0;
    }

    return 1;
}

vector(char) load_file(const char* path, const char* fmt) {
    FILE* file = fopen(path, fmt);
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    i32 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    vector(char) out = make_vector();
    vector_resize(out, size);
    fread(out, 1, size, file);

    fclose(file);
    return out;
}

static i32 create_shader_module(vulkan_test* vt, VkShaderModule* shader_module, const vector(char) code) {
    VkShaderModuleCreateInfo create_info;
    memset(&create_info, 0, sizeof(VkShaderModuleCreateInfo));
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pCode = (const u32*)code;
    create_info.codeSize = vector_size(code);

    if (vkCreateShaderModule(vt->device, &create_info, NULL, shader_module) != VK_SUCCESS) {
        return 0;
    }
    return 1;
}

i32 create_graphics_pipeline(vulkan_test* vt) {
    const char* vert_file = "res/shaders/vert.spv";
    const char* frag_file = "res/shaders/frag.spv";
    const vector(char) vert = load_file(vert_file, "rb");
    const vector(char) frag = load_file(frag_file, "rb");

    if (!vert) {
        LOG_FATAL("\t%s %s\n", "failed to load file", vert_file);
        return 0;
    }

    if (!frag) {
        LOG_FATAL("\t%s %s\n", "failed to load file", frag_file);
        return 0;
    }

    VkShaderModule vert_shader_module;
    if (!create_shader_module(vt, &vert_shader_module, vert)) {
        return 0;
    }

    VkShaderModule frag_shader_module;
    if (!create_shader_module(vt, &frag_shader_module, frag)) {
        vkDestroyShaderModule(vt->device, vert_shader_module, NULL);
        return 0;
    }

    free_vector(vert);
    free_vector(frag);

    VkPipelineShaderStageCreateInfo shader_stages[2];
    memset(&shader_stages[0], 0, sizeof(VkPipelineShaderStageCreateInfo));
    shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = vert_shader_module;
    shader_stages[0].pName = "main";

    memset(&shader_stages[1], 0, sizeof(VkPipelineShaderStageCreateInfo));
    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = frag_shader_module;
    shader_stages[1].pName = "main";

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    memset(&vertex_input_info, 0, sizeof(VkPipelineVertexInputStateCreateInfo));
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = NULL;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = NULL;

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state;
    memset(&dynamic_state, 0, sizeof(VkPipelineDynamicStateCreateInfo));
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = sizeof(dynamic_states) / sizeof(VkDynamicState);
    dynamic_state.pDynamicStates = dynamic_states;

    VkPipelineViewportStateCreateInfo viewport_state;
    memset(&viewport_state, 0, sizeof(VkPipelineViewportStateCreateInfo));
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    memset(&input_assembly, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizer;
    memset(&rasterizer, 0, sizeof(VkPipelineRasterizationStateCreateInfo));
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling;
    memset(&multisampling, 0, sizeof(VkPipelineMultisampleStateCreateInfo));
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = NULL;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment;
    memset(&color_blend_attachment, 0, sizeof(VkPipelineColorBlendAttachmentState));
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
#if 1
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
#else
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
#endif

    VkPipelineColorBlendStateCreateInfo color_blending;
    memset(&color_blending, 0, sizeof(VkPipelineColorBlendStateCreateInfo));
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0;
    color_blending.blendConstants[1] = 0.0;
    color_blending.blendConstants[2] = 0.0;
    color_blending.blendConstants[3] = 0.0;

    VkPipelineLayoutCreateInfo pipeline_layout_info;
    memset(&pipeline_layout_info, 0, sizeof(VkPipelineLayoutCreateInfo));
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = NULL;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = NULL;

    if (vkCreatePipelineLayout(vt->device, &pipeline_layout_info, NULL, &vt->pipeline_layout) != VK_SUCCESS) {
        vkDestroyShaderModule(vt->device, vert_shader_module, NULL);
        vkDestroyShaderModule(vt->device, frag_shader_module, NULL);
        return 0;
    }

    VkGraphicsPipelineCreateInfo pipeline_info;
    memset(&pipeline_info, 0, sizeof(VkGraphicsPipelineCreateInfo));
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;

    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = NULL;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = &dynamic_state;

    pipeline_info.layout = vt->pipeline_layout;
    pipeline_info.renderPass = vt->render_pass;
    pipeline_info.subpass = 0;

    if (vkCreateGraphicsPipelines(vt->device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &vt->graphics_pipeline) != VK_SUCCESS) {
        vkDestroyShaderModule(vt->device, vert_shader_module, NULL);
        vkDestroyShaderModule(vt->device, frag_shader_module, NULL);
        return 0;
    }

    vkDestroyShaderModule(vt->device, vert_shader_module, NULL);
    vkDestroyShaderModule(vt->device, frag_shader_module, NULL);
    return 1;
}

i32 vulkan_test_init(void* self) {
    vulkan_test* vt = self;
    memset(vt, 0, sizeof(vulkan_test));

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    vt->window = glfwCreateWindow(960, 640, "vulkan test", NULL, NULL);

    const char* validation_layer_names[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    const u32 validation_layer_count = sizeof(validation_layer_names) / sizeof(char*);

    if (!create_vulkan_instance(vt, validation_layer_names, validation_layer_count)) {
        LOG_FATAL("\t%s\n", "failed to create vuklan instance");
        return 0;
    }
    LOG_INFO("\t%s\n", "create vuklan instance successfully");

    if (!setup_vulkan_debug_messenger(vt)) {
        LOG_WARN("\t%s\n", "failed to create vulkan debug messenger");
    }
    else {
        LOG_INFO("\t%s\n", "create vulkan debug messenger successfully");
    }

    if (!create_vulkan_surface(vt)) {
        LOG_FATAL("\t%s\n", "failed to create vuklan surface");
        return 0;
    }
    LOG_INFO("\t%s\n", "create vuklan surface successfully");

    const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    const u32 device_extension_count = sizeof(device_extensions) / sizeof(char*);

    select_supported_physical_device(vt, device_extensions, device_extension_count);
    if (vt->physical_device == VK_NULL_HANDLE) {
        LOG_FATAL("\t%s\n", "not founding any desired physical device");
        return 0;
    }
    LOG_INFO("\t%s\n", "vulkan physical device selected");

    if (!create_logical_device(vt, validation_layer_names, validation_layer_count, device_extensions, device_extension_count)) {
        LOG_FATAL("\t%s\n", "failed to create device");
        return 0;
    }
    LOG_INFO("\t%s\n", "create device sucessfully");

    vt->swap_chain_images = make_vector();
    if (!create_swap_chain(vt)) {
        LOG_FATAL("\t%s\n", "failed to create swap chain");
        free_vector(vt->swap_chain_images);
        return 0;
    }
    LOG_INFO("\t%s\n", "create swap chain successfully");

    vt->swap_chain_image_views = make_vector();
    if (!create_image_views(vt)) {
        LOG_FATAL("\t%s\n", "failed to create image views");
        free_vector(vt->swap_chain_image_views);
        return 0;
    }
    LOG_INFO("\t%s\n", "create image views successfully");

    if (!create_render_pass(vt)) {
        LOG_TRACE("\t%s\n", "failed to create render pass");
        return 0;
    }
    LOG_INFO("\t%s\n", "create render pass successfully");

    if (!create_graphics_pipeline(vt)) {
        LOG_TRACE("\t%s\n", "failed to create graphics pipeline");
        return 0;
    }
    LOG_INFO("\t%s\n", "create graphics pipeline successfully");

    return 1;
}

i32 vulkan_test_is_running(void* self) {
    vulkan_test* vt = self;
    return 0;
    return !glfwWindowShouldClose(vt->window);
}

void vulkan_test_update(void* self) {
    vulkan_test* vt = self;

    glfwPollEvents();
}

void vulkan_test_terminate(void* self) {
    vulkan_test* vt = self;

    for_vector(vt->swap_chain_image_views, i, 0) {
        vkDestroyImageView(vt->device, vt->swap_chain_image_views[i], NULL);
    }

    vkDestroyPipeline(vt->device, vt->graphics_pipeline, NULL);
    vkDestroyRenderPass(vt->device, vt->render_pass, NULL);
    vkDestroyPipelineLayout(vt->device, vt->pipeline_layout, NULL);
    vkDestroySwapchainKHR(vt->device, vt->swap_chain, NULL);
    vkDestroyDevice(vt->device, NULL);
    DestroyDebugUtilsMessengerEXT(vt->instance, vt->debug_messenger, NULL);
    vkDestroySurfaceKHR(vt->instance, vt->surface, NULL);
    vkDestroyInstance(vt->instance, NULL);
    glfwDestroyWindow(vt->window);
    glfwTerminate();

    if (vt->swap_chain_images) {
        free_vector(vt->swap_chain_images);
    }

    if (vt->swap_chain_image_views) {
        free_vector(vt->swap_chain_image_views);
    }

    i32 leak_count = check_memory_leak();
    LOG_TRACE("\tleak_count = %d\n", leak_count);
}

application_setup create_application() {

    return game_test_start();
    static vulkan_test test;
    return (application_setup) {
        .app = &test,
        .on_initialize = vulkan_test_init,
        .is_running = vulkan_test_is_running,
        .on_update = vulkan_test_update,
        .on_terminate = vulkan_test_terminate,
    };
}
