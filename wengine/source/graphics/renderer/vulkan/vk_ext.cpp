#include "vk_ext.hpp"

void vk_ext_ctx_init(VkDevice device, WenVkEXTContext *ext_ctx)
{
	WEN_LOAD_VK_EXT_PROC(ext_ctx, device, vkCreateShadersEXT);
	WEN_LOAD_VK_EXT_PROC(ext_ctx, device, vkDestroyShaderEXT);
	WEN_LOAD_VK_EXT_PROC(ext_ctx, device, vkCmdBindShadersEXT);
	WEN_LOAD_VK_EXT_PROC(ext_ctx, device, vkCmdSetPolygonModeEXT);
}
