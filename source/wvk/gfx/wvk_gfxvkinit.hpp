#pragma once

#include "vk_mem_alloc.h"
#include "wvk/common/wvk_pch.hpp"
#include <vulkan/vulkan_core.h>

namespace wvk::init
{
inline VmaAllocatorCreateInfo vma_allocator_create_info(
    VkInstance                instance,
    VkPhysicalDevice          gpu,
    VkDevice                  device,
    const VmaVulkanFunctions *fp = nullptr)
{
	VmaAllocatorCreateInfo create_info = {};
	create_info.instance               = instance;
	create_info.physicalDevice         = gpu;
	create_info.device                 = device;
	create_info.flags                  = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	create_info.pVulkanFunctions       = fp;
	return create_info;
}

inline VmaAllocationCreateInfo vma_allocation_create_info(
    VmaMemoryUsage           usage     = VMA_MEMORY_USAGE_AUTO,
    VkMemoryPropertyFlags    required  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    VmaAllocationCreateFlags flags     = 0,
    VkMemoryPropertyFlags    preferred = 0)
{
	VmaAllocationCreateInfo info = {};
	info.usage                   = usage;
	info.requiredFlags           = required;
	info.flags                   = flags;
	info.preferredFlags          = preferred;
	return info;
}

inline VkSamplerCreateInfo sampler_create_info(
    VkFilter             magFilter,
    VkFilter             minFilter,
    VkSamplerAddressMode addressModeU,
    VkSamplerAddressMode addressModeV,
    VkSamplerAddressMode addressModeW,
    VkSamplerMipmapMode  mipmapMod        = VK_SAMPLER_MIPMAP_MODE_LINEAR,
    VkCompareOp          compareOp        = VK_COMPARE_OP_NEVER,
    float                minLod           = 0.0f,
    float                maxLod           = 0.0f,
    bool                 compareOpEnable  = VK_FALSE,
    bool                 anisotropyEnable = VK_FALSE,
    float                maxAnisotropy    = 0)
{
	VkSamplerCreateInfo info = {};
	info.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.pNext               = VK_NULL_HANDLE;
	info.flags               = 0;
	info.magFilter           = magFilter;
	info.minFilter           = minFilter;
	info.mipmapMode          = mipmapMod;
	info.addressModeU        = addressModeU;
	info.addressModeV        = addressModeV;
	info.addressModeW        = addressModeW;
	info.anisotropyEnable    = anisotropyEnable;
	info.maxAnisotropy       = maxAnisotropy;
	info.compareEnable       = compareOpEnable;
	info.compareOp           = compareOp;
	info.minLod              = minLod;
	info.maxLod              = maxLod;
	return info;
}

inline VkImageSubresourceRange image_subresource_range(
    VkImageAspectFlags aspectMask,
    uint32_t           levelCount = 1,
    uint32_t           layerCount = 1)
{
	VkImageSubresourceRange subresource = {};
	subresource.aspectMask              = aspectMask;
	subresource.baseMipLevel            = 0;
	subresource.levelCount              = levelCount;
	subresource.baseArrayLayer          = 0;
	subresource.layerCount              = layerCount;
	return subresource;
}

inline VkImageCreateInfo image_create_info(
    VkFormat          format,
    VkExtent3D        extent,
    VkImageUsageFlags usages)
{
	VkImageCreateInfo create_info = {};
	create_info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create_info.pNext             = VK_NULL_HANDLE;
	create_info.format            = format;
	create_info.imageType         = VK_IMAGE_TYPE_2D;
	create_info.extent            = extent;
	create_info.mipLevels         = 1;
	create_info.arrayLayers       = 1;
	create_info.usage             = usages;
	create_info.samples           = VK_SAMPLE_COUNT_1_BIT;
	create_info.tiling            = VK_IMAGE_TILING_OPTIMAL;
	create_info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
	return create_info;
}

inline VkImageViewCreateInfo image_view_create_info(
    VkFormat                format,
    VkImage                 attachment,
    VkImageSubresourceRange subresource_range)
{
	VkImageViewCreateInfo info = {};
	info.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext                 = VK_NULL_HANDLE;
	info.viewType              = VK_IMAGE_VIEW_TYPE_2D;
	info.image                 = attachment;
	info.format                = format;
	info.components.r          = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.g          = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.b          = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.a          = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.subresourceRange      = subresource_range;
	return info;
}

inline VkImageBlit2 image_blit_2(
    VkExtent2D srcExtent,
    VkExtent2D dstExtent)
{
	VkImageBlit2 info = {};
	info.sType        = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	info.pNext        = VK_NULL_HANDLE;

	info.srcOffsets[1].x = WVK_CAST(int32_t, srcExtent.width);
	info.srcOffsets[1].y = WVK_CAST(int32_t, srcExtent.height);
	info.srcOffsets[1].z = 1;

	info.dstOffsets[1].x = WVK_CAST(int32_t, dstExtent.width);
	info.dstOffsets[1].y = WVK_CAST(int32_t, dstExtent.height);
	info.dstOffsets[1].z = 1;

	info.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	info.srcSubresource.baseArrayLayer = 0;
	info.srcSubresource.layerCount     = 1;
	info.srcSubresource.mipLevel       = 0;

	info.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	info.dstSubresource.baseArrayLayer = 0;
	info.dstSubresource.layerCount     = 1;
	info.dstSubresource.mipLevel       = 0;
	return info;
}

inline VkImageMemoryBarrier2 image_memory_barrier_2(
    VkPipelineStageFlags2 srcStageMask,
    VkAccessFlags2        srcAccessMask,
    VkPipelineStageFlags2 dstStageMask,
    VkAccessFlags2        dstAccessMask,
    VkImage               image,
    VkImageAspectFlags    aspectMask,
    VkImageLayout         oldLayout,
    VkImageLayout         newLayout,
    uint32_t              srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    uint32_t              dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED)
{
	VkImageSubresourceRange subresource = image_subresource_range(aspectMask);

	VkImageMemoryBarrier2 info = {};
	info.sType                 = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	info.pNext                 = VK_NULL_HANDLE;
	info.srcStageMask          = srcStageMask;
	info.srcAccessMask         = srcAccessMask;
	info.dstStageMask          = dstStageMask;
	info.dstAccessMask         = dstAccessMask;
	info.oldLayout             = oldLayout;
	info.newLayout             = newLayout;
	info.srcQueueFamilyIndex   = srcQueueFamilyIndex;
	info.dstQueueFamilyIndex   = dstQueueFamilyIndex;
	info.image                 = image;
	info.subresourceRange      = subresource;
	return info;
}

inline VkDependencyInfo dependency_info(
    std::span<const VkImageMemoryBarrier2> barrier_2)
{
	VkDependencyInfo info        = {};
	info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	info.pNext                   = VK_NULL_HANDLE;
	info.dependencyFlags         = 0;
	info.imageMemoryBarrierCount = WVK_CAST(uint32_t, barrier_2.size());
	info.pImageMemoryBarriers    = barrier_2.data();
	return info;
}

inline VkBlitImageInfo2 blit_image_info_2(
    VkImage             src,
    VkImage             dst,
    const VkImageBlit2 *region,
    VkFilter            filter = VK_FILTER_NEAREST)
{
	VkBlitImageInfo2 info = {};
	info.sType            = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	info.pNext            = VK_NULL_HANDLE;
	info.srcImage         = src;
	info.dstImage         = dst;
	info.srcImageLayout   = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	info.dstImageLayout   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	info.filter           = filter;
	info.regionCount      = 1;
	info.pRegions         = region;
	return info;
}

inline VkCommandBufferSubmitInfo command_buffer_submit_info(
    VkCommandBuffer cmd)
{
	VkCommandBufferSubmitInfo info = {};
	info.sType                     = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	info.pNext                     = VK_NULL_HANDLE;
	info.commandBuffer             = cmd;
	info.deviceMask                = 0;
	return info;
}

inline VkSubmitInfo2 submit_info_2(
    const VkCommandBufferSubmitInfo *bufferSubmitInfo,
    const VkSemaphoreSubmitInfo     *signal,
    const VkSemaphoreSubmitInfo     *wait)
{
	VkSubmitInfo2 info            = {};
	info.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	info.pNext                    = VK_NULL_HANDLE;
	info.waitSemaphoreInfoCount   = (wait == VK_NULL_HANDLE) ? 0u : 1u;
	info.pWaitSemaphoreInfos      = wait;
	info.signalSemaphoreInfoCount = (signal == VK_NULL_HANDLE) ? 0u : 1u;
	info.pSignalSemaphoreInfos    = signal;
	info.commandBufferInfoCount   = 1;
	info.pCommandBufferInfos      = bufferSubmitInfo;
	return info;
}

inline VkPresentInfoKHR present_info(
    const VkSwapchainKHR *swapchain,
    const VkSemaphore    *semaphore,
    const uint32_t       *imageIndices)
{
	VkPresentInfoKHR info   = {};
	info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.pNext              = VK_NULL_HANDLE;
	info.swapchainCount     = 1;
	info.pSwapchains        = swapchain;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores    = semaphore;
	info.pImageIndices      = imageIndices;
	return info;
}

inline VkSemaphoreSubmitInfo semaphore_submit_info(
    VkSemaphore           semaphore,
    VkPipelineStageFlags2 stage)
{
	VkSemaphoreSubmitInfo info = {};
	info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	info.semaphore             = semaphore;
	info.stageMask             = stage;
	return info;
}

inline VkSemaphoreCreateInfo semaphore_create_info()
{
	VkSemaphoreCreateInfo create_info = {};
	create_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	create_info.pNext                 = VK_NULL_HANDLE;
	return create_info;
}

inline VkFenceCreateInfo fence_create_info()
{
	VkFenceCreateInfo create_info = {};
	create_info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	create_info.pNext             = VK_NULL_HANDLE;
	create_info.flags             = VK_FENCE_CREATE_SIGNALED_BIT;
	return create_info;
}

inline VkCommandPoolCreateInfo command_pool_create_info(uint32_t queue_family_index)
{
	VkCommandPoolCreateInfo create_info = {};
	create_info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.pNext                   = VK_NULL_HANDLE;
	create_info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	create_info.queueFamilyIndex        = queue_family_index;
	return create_info;
}

inline VkCommandBufferAllocateInfo command_buffer_allocate_info(
    uint32_t      buffer_count,
    VkCommandPool command_pool)
{
	VkCommandBufferAllocateInfo allocate_info = {};
	allocate_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate_info.pNext                       = VK_NULL_HANDLE;
	allocate_info.commandPool                 = command_pool;
	allocate_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate_info.commandBufferCount          = buffer_count;
	return allocate_info;
}

inline VkCommandBufferBeginInfo command_buffer_begin_info()
{
	VkCommandBufferBeginInfo info = {};
	info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext                    = VK_NULL_HANDLE;
	info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	return info;
}

inline VkWriteDescriptorSet write_descriptor_set(
    VkDescriptorType              type,
    uint32_t                      binding    = 0,
    uint32_t                      id         = 0,
    VkDescriptorSet               set        = VK_NULL_HANDLE,
    const VkDescriptorImageInfo  *imageInfo  = VK_NULL_HANDLE,
    const VkDescriptorBufferInfo *bufferInfo = VK_NULL_HANDLE)
{
	VkWriteDescriptorSet info = {};
	info.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	info.pNext                = VK_NULL_HANDLE;
	info.descriptorType       = type;
	info.dstBinding           = binding;
	info.dstArrayElement      = id;
	info.descriptorCount      = 1;
	info.dstSet               = set;
	info.pBufferInfo          = bufferInfo;
	info.pImageInfo           = imageInfo;
	return info;
}

inline VkDescriptorImageInfo descriptor_image_info(
    VkSampler     sampler     = VK_NULL_HANDLE,
    VkImageView   imageView   = VK_NULL_HANDLE,
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED)
{
	VkDescriptorImageInfo info = {};
	info.sampler               = sampler;
	info.imageView             = imageView;
	info.imageLayout           = imageLayout;
	return info;
}

inline VkDescriptorSetLayoutBinding descriptor_set_layout_binding(
    uint32_t              binding,
    VkDescriptorType      type,
    VkShaderStageFlagBits stage,
    uint32_t              descriptorCount = 1)
{
	VkDescriptorSetLayoutBinding info = {};
	info.binding                      = binding;
	info.stageFlags                   = stage;
	info.descriptorCount              = descriptorCount;
	info.descriptorType               = type;
	return info;
}

inline VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(
    const void                                   *next,
    std::span<const VkDescriptorSetLayoutBinding> binding,
    VkDescriptorSetLayoutCreateFlags              flags)
{
	VkDescriptorSetLayoutCreateInfo create_info = {};
	create_info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	create_info.pNext                           = next;
	create_info.bindingCount                    = WVK_CAST(uint32_t, binding.size());
	create_info.pBindings                       = binding.data();
	create_info.flags                           = flags;
	return create_info;
}

inline VkDescriptorSetAllocateInfo descriptor_set_allocate_info(
    VkDescriptorPool             pool,
    const VkDescriptorSetLayout *setLayout)
{
	VkDescriptorSetAllocateInfo allocate_info = {};
	allocate_info.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.pNext                       = VK_NULL_HANDLE;
	allocate_info.descriptorPool              = pool;
	allocate_info.pSetLayouts                 = setLayout;
	allocate_info.descriptorSetCount          = 1;
	return allocate_info;
}

inline VkDescriptorSetVariableDescriptorCountAllocateInfo descriptor_set_variable_descriptor_count_allocate_info(
    const uint32_t *maxBinding)
{
	VkDescriptorSetVariableDescriptorCountAllocateInfo info = {};
	info.sType                                              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
	info.descriptorSetCount                                 = 1;
	info.pDescriptorCounts                                  = maxBinding;
	return info;
}

inline VkDescriptorPoolCreateInfo descriptor_pool_create_info(
    uint32_t                              max_sets,
    std::span<const VkDescriptorPoolSize> poolSizes)
{
	VkDescriptorPoolCreateInfo info = {};
	info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.pNext                      = VK_NULL_HANDLE;
	info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	info.maxSets                    = max_sets;
	info.poolSizeCount              = WVK_CAST(uint32_t, poolSizes.size());
	info.pPoolSizes                 = poolSizes.data();
	return info;
}

inline VkDescriptorSetLayoutBindingFlagsCreateInfo descriptor_set_layout_binding_flags_create_info(
    std::span<const VkDescriptorBindingFlags> bindingFlags)
{
	VkDescriptorSetLayoutBindingFlagsCreateInfo info = {};
	info.sType                                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	info.pNext                                       = VK_NULL_HANDLE;
	info.bindingCount                                = WVK_CAST(uint32_t, bindingFlags.size());
	info.pBindingFlags                               = bindingFlags.data();
	return info;
}

inline VkPipelineLayoutCreateInfo pipeline_layout_create_info(
    std::span<const VkDescriptorSetLayout> setLayout,
    std::span<const VkPushConstantRange>   pushConstant)
{
	VkPipelineLayoutCreateInfo info = {};
	info.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pNext                      = VK_NULL_HANDLE;
	info.setLayoutCount             = WVK_CAST(uint32_t, setLayout.size());
	info.pSetLayouts                = setLayout.data();
	info.pushConstantRangeCount     = WVK_CAST(uint32_t, pushConstant.size());
	info.pPushConstantRanges        = pushConstant.data();
	return info;
}

inline VkRenderingAttachmentInfo rendering_attachment_info(
    VkClearValue        clearValue,
    VkImageView         imageView,
    VkImageLayout       imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VkAttachmentLoadOp  loadOp      = VK_ATTACHMENT_LOAD_OP_LOAD,
    VkAttachmentStoreOp storeOp     = VK_ATTACHMENT_STORE_OP_STORE)
{
	VkRenderingAttachmentInfo info = {};
	info.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	info.pNext                     = nullptr;
	info.loadOp                    = loadOp;
	info.storeOp                   = storeOp;
	info.imageView                 = imageView;
	info.imageLayout               = imageLayout;
	info.clearValue                = clearValue;
	return info;
}

inline VkRenderingInfo rendering_info(
    VkRect2D                                   renderArea,
    uint32_t                                   layerCount        = 1,
    VkRenderingFlags                           flags             = 0,
    std::span<const VkRenderingAttachmentInfo> colorAttachments  = {},
    const VkRenderingAttachmentInfo           *depthAttachment   = nullptr,
    const VkRenderingAttachmentInfo           *stencilAttachment = nullptr)
{
	VkRenderingInfo info      = {};
	info.renderArea           = renderArea;
	info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
	info.flags                = flags;
	info.layerCount           = layerCount;
	info.pNext                = VK_NULL_HANDLE;
	info.colorAttachmentCount = WVK_CAST(uint32_t, colorAttachments.size());
	info.pColorAttachments    = colorAttachments.data();
	info.pDepthAttachment     = depthAttachment;
	info.pStencilAttachment   = stencilAttachment;
	return info;
}

inline VkPipelineRenderingCreateInfo rendering_state(
    std::span<const VkFormat> colorAttachmentFormats,
    VkFormat                  depthFormat   = VK_FORMAT_UNDEFINED,
    VkFormat                  stencilFormat = VK_FORMAT_UNDEFINED)
{
	VkPipelineRenderingCreateInfo info = {};
	info.sType                         = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	info.pNext                         = VK_NULL_HANDLE;
	info.colorAttachmentCount          = WVK_CAST(uint32_t, colorAttachmentFormats.size());
	info.pColorAttachmentFormats       = colorAttachmentFormats.data();
	info.depthAttachmentFormat         = depthFormat;
	info.stencilAttachmentFormat       = stencilFormat;
	return info;
}

inline VkShaderModuleCreateInfo shader_module_create_info(std::span<const uint32_t> codes)
{
	VkShaderModuleCreateInfo info = {};
	info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize                 = WVK_CAST(uint32_t, codes.size()) * sizeof(uint32_t);
	info.pCode                    = codes.data();
	return info;
}

inline VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(
    VkShaderModule        module,
    VkShaderStageFlagBits stage,
    const char           *name = "main")
{
	VkPipelineShaderStageCreateInfo info = {};
	info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.pNext                           = VK_NULL_HANDLE;
	info.stage                           = stage;
	info.module                          = module;
	info.pName                           = name;
	return info;
}

inline VkPipelineVertexInputStateCreateInfo vertex_input_state(
    std::span<const VkVertexInputBindingDescription>   inputBinding   = {},
    std::span<const VkVertexInputAttributeDescription> inputAttribute = {})
{
	VkPipelineVertexInputStateCreateInfo info = {};
	info.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	info.pNext                                = VK_NULL_HANDLE;
	info.vertexBindingDescriptionCount        = WVK_CAST(uint32_t, inputBinding.size());
	info.pVertexBindingDescriptions           = inputBinding.data();
	info.vertexAttributeDescriptionCount      = WVK_CAST(uint32_t, inputAttribute.size());
	info.pVertexAttributeDescriptions         = inputAttribute.data();
	return info;
}

inline VkPipelineInputAssemblyStateCreateInfo input_assembly_state(
    VkPrimitiveTopology topology)
{
	VkPipelineInputAssemblyStateCreateInfo info = {};
	info.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.pNext                                  = VK_NULL_HANDLE;
	info.topology                               = topology;
	info.primitiveRestartEnable                 = VK_FALSE;
	return info;
}

inline VkPipelineViewportStateCreateInfo viewport_state()
{
	VkPipelineViewportStateCreateInfo info = {};
	info.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	info.pNext                             = VK_NULL_HANDLE;
	info.viewportCount                     = 1;
	info.scissorCount                      = 1;
	return info;
}

inline VkPipelineRasterizationStateCreateInfo rasterization_state(
    VkCullModeFlags cullMode,
    VkFrontFace     frontFace)
{
	VkPipelineRasterizationStateCreateInfo info = {};
	info.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	info.pNext                                  = VK_NULL_HANDLE;
	info.flags                                  = 0;
	info.depthClampEnable                       = VK_FALSE;
	info.rasterizerDiscardEnable                = VK_FALSE;
	info.polygonMode                            = VK_POLYGON_MODE_FILL;
	info.cullMode                               = cullMode;
	info.frontFace                              = frontFace;
	info.depthBiasEnable                        = VK_FALSE;
	info.depthBiasConstantFactor                = 0.0f;
	info.depthBiasClamp                         = 0.0f;
	info.depthBiasSlopeFactor                   = 0.0f;
	info.lineWidth                              = 1.0f;
	return info;
}

inline VkPipelineMultisampleStateCreateInfo multisample_state(
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT)
{
	VkPipelineMultisampleStateCreateInfo info = {};
	info.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info.pNext                                = VK_NULL_HANDLE;
	info.flags                                = 0;
	info.rasterizationSamples                 = sampleCount;
	info.sampleShadingEnable                  = VK_FALSE;
	info.minSampleShading                     = 1.0f;
	info.pSampleMask                          = VK_NULL_HANDLE;
	info.alphaToCoverageEnable                = VK_FALSE;
	info.alphaToOneEnable                     = VK_FALSE;
	return info;
}

inline VkPipelineDepthStencilStateCreateInfo depth_stencil_state(
    bool        depthTestEnable,
    bool        depthWriteEnable,
    VkCompareOp depthCompareOp)
{
	VkPipelineDepthStencilStateCreateInfo info = {};
	info.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	info.pNext                                 = VK_NULL_HANDLE;
	info.flags                                 = 0;
	info.depthTestEnable                       = depthTestEnable;
	info.depthWriteEnable                      = depthWriteEnable;
	info.depthCompareOp                        = depthCompareOp;
	info.depthBoundsTestEnable                 = VK_FALSE;
	info.stencilTestEnable                     = VK_FALSE;
	info.front                                 = {};
	info.back                                  = {};
	info.minDepthBounds                        = 0.0f;
	info.maxDepthBounds                        = 1.0f;
	return info;
}

inline VkPipelineColorBlendAttachmentState color_blend_attachment_state()
{
	VkPipelineColorBlendAttachmentState state = {};
	state.colorWriteMask                      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	state.blendEnable                         = VK_TRUE;
	state.srcColorBlendFactor                 = VK_BLEND_FACTOR_SRC_ALPHA;
	state.dstColorBlendFactor                 = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	state.colorBlendOp                        = VK_BLEND_OP_ADD;
	state.srcAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	state.alphaBlendOp                        = VK_BLEND_OP_ADD;
	return state;
}

inline VkPipelineColorBlendStateCreateInfo color_blend_state(
    std::span<const VkPipelineColorBlendAttachmentState> attachments)
{
	VkPipelineColorBlendStateCreateInfo info = {};
	info.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info.pNext                               = VK_NULL_HANDLE;
	info.flags                               = 0;
	info.logicOpEnable                       = VK_FALSE;
	info.logicOp                             = VK_LOGIC_OP_COPY;
	info.attachmentCount                     = WVK_CAST(uint32_t, attachments.size());
	info.pAttachments                        = attachments.data();
	info.blendConstants[0]                   = 0.0f;
	info.blendConstants[1]                   = 0.0f;
	info.blendConstants[2]                   = 0.0f;
	info.blendConstants[3]                   = 1.0f;
	return info;
}

inline VkPipelineDynamicStateCreateInfo dynamic_state(
    std::span<const VkDynamicState> dynamicStates)
{
	VkPipelineDynamicStateCreateInfo info = {};
	info.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	info.pNext                            = VK_NULL_HANDLE;
	info.flags                            = 0;
	info.dynamicStateCount                = WVK_CAST(uint32_t, dynamicStates.size());
	info.pDynamicStates                   = dynamicStates.data();
	return info;
}

inline VkBufferCreateInfo buffer_create_info(
    VkDeviceSize       size,
    VkBufferUsageFlags usages)
{
	VkBufferCreateInfo info = {};
	info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.pNext              = VK_NULL_HANDLE;
	info.size               = size;
	info.usage              = usages;
	info.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;
	return info;
}
}        // namespace wvk::init
