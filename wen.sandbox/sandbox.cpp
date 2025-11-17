#include "Wen/Wen_application.h"
#include <Wen_entry.h>

class Sandbox : public Application {
  public:
  FORCE_INLINE Sandbox(): Application("Sandbox", 800, 600) {}
  FORCE_INLINE ~Sandbox() = default;
};

Application* create_application() {
  return new Sandbox;
}