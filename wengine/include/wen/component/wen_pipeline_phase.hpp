#ifndef WEN_COMPONENT_WEN_PIPELINE_PHASE_HPP_
#define WEN_COMPONENT_WEN_PIPELINE_PHASE_HPP_

#include "wen/wen_defines.hpp"
#include <flecs.h>

namespace wen::component {
/** Custom pipeline phase. */
struct WEN_API OnProcessEvent{}; //** process event pipeline phase.*/
struct WEN_API OnDraw{};         //** draw pipeline phase.*/
struct WEN_API OnUpdate{};       //** update pipeline phase.*/
struct WEN_API OnBeginDraw{};    //** Begin draw pipeline phase.*/
struct WEN_API OnEndDraw{};      //** End draw pipeline phase.*/

class WEN_API PipelinePhaseComponent {
public:
  explicit PipelinePhaseComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_PIPELINE_PHASE_HPP_
