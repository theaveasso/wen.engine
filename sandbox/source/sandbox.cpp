#include <wen/wen.hpp>

void init_component(flecs::world& world);

int main(int /* argc */, char** /* argv */) {
  wen::engine e;
  e.run();

  return 0;
}
