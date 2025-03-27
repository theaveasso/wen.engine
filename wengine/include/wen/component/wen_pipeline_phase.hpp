#ifndef WEN_COMPONENT_PIPELINE_PHASE_HPP_
#define WEN_COMPONENT_PIPELINE_PHASE_HPP_

#include "wen/wen_defines.hpp"
#include <flecs.h>

namespace wen::component {

/** Custom pipeline phase. */
struct WEN_API_EXPORT OnProcessEvent{}; //** process event pipeline phase.*/
struct WEN_API_EXPORT OnDraw{};         //** draw pipeline phase.*/
struct WEN_API_EXPORT OnUpdate{};       //** update pipeline phase.*/
struct WEN_API_EXPORT OnBeginDraw{};    //** Begin draw pipeline phase.*/
struct WEN_API_EXPORT OnEndDraw{};      //** End draw pipeline phase.*/

class WEN_API_EXPORT PipelinePhaseComponent {
public:
  explicit PipelinePhaseComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_PIPELINE_PHASE_HPP_
