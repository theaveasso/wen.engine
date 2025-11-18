#pragma once

#include "Wen_core.h"
#include "Wen_pch.h"
#include "Wen_interface.h"
#include <algorithm>
#include <type_traits>

struct WEN_API AppContext {
  FORCE_INLINE AppContext() = default;
  FORCE_INLINE ~AppContext() = default; 

  template<typename T>
  FORCE_INLINE T* get_layer() {
    STATIC_ASSERT(std::is_base_of<AppInterface, T>::value);
    auto itr = std::find_if(layers.begin(), layers.end(), [this](AppInterface* layer) {
      return (layer->layer_id == WEN_TYPEID<T>());
    });
    if (itr != layers.end()) {
      return static_cast<T*>(*itr);
    }

    return nullptr;
  }

  template<typename T, typename... Args>
  FORCE_INLINE T* attach_layer(Args&&... args) {
    STATIC_ASSERT(std::is_base_of<AppInterface, T>::value);
    if (get_layer<T>() != nullptr) {
      return nullptr;
    }
    auto layer = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = layer.get();
    layers.push_back(std::move(layer));
    layer->layer_id = WEN_TYPEID<T>();
    layer->on_init();

    return *ptr;
  }

  AppContext(const AppContext&) = delete;
  AppContext& operator=(const AppContext&) = delete;

  std::vector<std::unique_ptr<AppInterface>> layers;
};