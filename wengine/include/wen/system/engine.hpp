#ifndef WEN_SYSTEM_ENGINE_HPP_
#define WEN_SYSTEM_ENGINE_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

namespace wen::system {
class WEN_API_EXPORT Engine {
public:
  explicit Engine(flecs::world& world);

private:
};
} // namespace wen::system

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_SYSTEM_ENGINE_HPP_
