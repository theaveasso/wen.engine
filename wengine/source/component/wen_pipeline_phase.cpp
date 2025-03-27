#include "wen/component/wen_pipeline_phase.hpp"

namespace wen::component {
PipelinePhaseComponent::PipelinePhaseComponent(flecs::world& world) {
  world.module<PipelinePhaseComponent>();

  world.component<OnProcessEvent>().add(flecs::Phase);
  world.component<OnDraw>().add(flecs::Phase);
  world.component<OnUpdate>().add(flecs::Phase);
  world.component<OnBeginDraw>().add(flecs::Phase);
  world.component<OnEndDraw>().add(flecs::Phase);
}
} // namespace wen::component
