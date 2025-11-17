#include <Wen_entry.h>

class SBGame: public Application {
  public:
  FORCE_INLINE SBGame() = default;
  FORCE_INLINE ~SBGame() = default;
};

Application* create_application() {
  return new SBGame;
}