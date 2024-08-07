#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include <string.h>

#include "game_test.h"
#include "core/log.h"

#include "basic/memallocate.h"

#include "application_setup.h"
#include "entry_point.h"

#include "basic/vector.h"
#include "basic/string.h"

typedef struct {
    GLFWwindow* window;
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
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
i32 init_vulkan(VkInstance* vk_instance, const char** validation_layer_names, u32 validation_layer_count) {
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

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    i32 validation_layers_available = 0;
    if (validation_layer_names) {
        validation_layers_available = enable_validation_layers(validation_layer_names, validation_layer_count);
    }

    const vector(cstring) glfw_extensions = get_vulkan_required_instance_extensions();
    if (validation_layers_available) {
        vector_push(glfw_extensions, make_string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
        create_info.enabledLayerCount = validation_layer_count;
        create_info.ppEnabledLayerNames = validation_layer_names;

        populate_debug_create_info(&debug_create_info);
        create_info.pNext = &debug_create_info;
    }

    const u32 glfw_extension_count = vector_size(glfw_extensions);
    
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;

    if (validation_layers_available) {
        LOG_TRACE("\t%s\n", "validation layers are available");
    }
    else {
        LOG_WARN("\t%s\n", "validation layers are not available");
    }

    u32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

    VkExtensionProperties extensions[extension_count];
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);

    if (vkCreateInstance(&create_info, NULL, vk_instance) != VK_SUCCESS) {
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

static i32 setup_vulkan_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT* debug_messenger) {
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_create_info(&create_info);

    if (CreateDebugUtilsMessengerEXT(instance, &create_info, NULL, debug_messenger) == VK_SUCCESS) {
        return 1;
    }

    return 0;
}

typedef struct { u32 index; i32 exist; } queue_index;
typedef struct {
    queue_index graphics_family;
} queue_family_indices;

i32 queue_family_fulfil_requirement(queue_family_indices* indices) {
    return indices->graphics_family.exist;
}

queue_family_indices find_required_queue_families(VkPhysicalDevice device) {
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
        if (queue_family_fulfil_requirement(&indices)) {
            break;
        }
    }

    return indices;
}

static i32 is_device_suitable(VkPhysicalDevice device) {
    queue_family_indices indices = find_required_queue_families(device);
    return queue_family_fulfil_requirement(&indices);
}

static void select_supported_physical_device(VkInstance instance, VkPhysicalDevice* device) {
    *device = VK_NULL_HANDLE;

    u32 device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    if (device_count == 0) {
        return;
    }
    VkPhysicalDevice devices[device_count];
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    for (u32 i = 0; i < device_count; i++) {
        if (is_device_suitable(devices[i])) {
            *device = devices[i];
            break;
        }
    }
}

static i32 create_logical_device(vulkan_test* vt, const char** validation_layer_names, u32 validation_layer_count) {
    queue_family_indices indices = find_required_queue_families(vt->physical_device);

    f32 queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info;
    memset(&queue_create_info, 0, sizeof(VkDeviceQueueCreateInfo));
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = indices.graphics_family.index;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures device_features;
    memset(&device_features, VK_FALSE, sizeof(VkPhysicalDeviceFeatures));

    VkDeviceCreateInfo create_info;
    memset(&create_info, 0, sizeof(VkDeviceCreateInfo));
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = 0;

    if (validation_layer_names) {
        create_info.enabledLayerCount = validation_layer_count;
        create_info.ppEnabledLayerNames = validation_layer_names;
    }
    else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(vt->physical_device, &create_info, NULL, &vt->device) != VK_SUCCESS) {
        return 0;
    }

    vkGetDeviceQueue(vt->device, indices.graphics_family.index, 0, &vt->graphics_queue);
    
    return 1;
}

i32 vulkan_test_init(void* self) {
    vulkan_test* vt = self;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    vt->window = glfwCreateWindow(960, 640, "vulkan test", NULL, NULL);

    const char* validation_layer_names[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    const u32 validation_layer_count = sizeof(validation_layer_names) / sizeof(char*);

    if (!init_vulkan(&vt->instance, validation_layer_names, validation_layer_count)) {
        LOG_FATAL("\t%s\n", "failed to create vuklan instance");
        return 0;
    }
    LOG_INFO("\t%s\n", "create vuklan instance successfully");

    if (!setup_vulkan_debug_messenger(vt->instance, &vt->debug_messenger)) {
        LOG_WARN("\t%s\n", "failed to create vulkan debug messenger");
    }
    else {
        LOG_INFO("\t%s\n", "create vulkan debug messenger successfully");
    }

    select_supported_physical_device(vt->instance, &vt->physical_device);
    if (vt->physical_device == VK_NULL_HANDLE) {
        LOG_FATAL("\t%s\n", "not founding any desired physical device");
        return 0;
    }
    LOG_INFO("\t%s\n", "vulkan physical device selected");

    if (!create_logical_device(vt, validation_layer_names, validation_layer_count)) {
        LOG_FATAL("\t%s\n", "failed to create device");
        return 0;
    }
    LOG_INFO("\t%s\n", "create device sucessfully");

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

    vkDestroyDevice(vt->device, NULL);
    DestroyDebugUtilsMessengerEXT(vt->instance, vt->debug_messenger, NULL);
    vkDestroyInstance(vt->instance, NULL);
    glfwDestroyWindow(vt->window);
    glfwTerminate();

    i32 leak_count = check_memory_leak();
    LOG_TRACE("\tleak_count = %d\n", leak_count);
}

application_setup create_application() {
    static vulkan_test test;
    return (application_setup) {
        .app = &test,
        .on_initialize = vulkan_test_init,
        .is_running = vulkan_test_is_running,
        .on_update = vulkan_test_update,
        .on_terminate = vulkan_test_terminate,
    };
    

    return game_test_start();
}
