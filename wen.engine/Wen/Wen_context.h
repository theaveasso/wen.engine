#pragma once

#include "Wen_pch.h"
#include "Wen_interface.h"

struct WEN_API AppContext {
  FORCE_INLINE AppContext() = default;
  FORCE_INLINE ~AppContext() = default; 

  template<typename T>
  T* get_layer() {
    for (auto& layer: layers) {
      if (auto casted = dynamic_cast<T*>(layer.get())) {
        return casted;
      }
    }

    return nullptr;
  }

  template<typename T, typename... Args>
  T& attach_layer(Args&&... args) {
    auto layer = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = layer.get();
    layers.push_back(std::move(layer));

    return *ptr;
  }

  AppContext(const AppContext&) = delete;
  AppContext& operator=(const AppContext&) = delete;

  std::vector<std::unique_ptr<AppInterface>> layers;
};