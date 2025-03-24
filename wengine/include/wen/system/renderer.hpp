#ifndef WEN_SYSTEM_RENDERER_HPP_
#define WEN_SYSTEM_RENDERER_HPP_

#include <flecs.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

namespace wen::system {
class RendererSystem {
public:
  RendererSystem(flecs::world& world);
};
} // namespace wen::system

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_SYSTEM_RENDERER_HPP_
