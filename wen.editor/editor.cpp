#include "Wen/Wen_application.h"
#include <Wen_entry.h>

class Editor: public Application {
  public:
  FORCE_INLINE Editor(): Application("Editor", 800, 600) {}
  FORCE_INLINE ~Editor() = default;
};

Application* create_application() {
  return new Editor;
}