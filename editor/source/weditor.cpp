#include "weditor/weditor.hpp"
#include <wen/wen.hpp>

#include <flecs.h>

namespace wen::editor {
static void InitializeEditor(flecs::world& world) {
  world.import <wen::system::Engine>();

  world.import <WindowSystem>();
  world.import <DockspaceSystem>();
  world.import <OverlaySystem>();
}
}