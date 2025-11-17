#include "Wen/Wen_application.h"
#include <Wen_entry.h>

class TestLayer: public AppInterface {
  public:
  virtual void on_update(float deltatime) override {
    WEN_INFO("test layer on update");
  };
  virtual void on_draw() override {
    WEN_INFO("test layer on draw");
  };
};

class Sandbox : public Application {
  public:
  FORCE_INLINE Sandbox(): Application("Sandbox", 800, 600) {
    m_context->attach_layer<TestLayer>();
  }
  FORCE_INLINE ~Sandbox() = default;
};

Application* create_application() {
  return new Sandbox;
}