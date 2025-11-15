#pragma once

#include "Wen_Core.h"
#include "Wen_Context.h"

struct Wen_AppInterface {
    WEN_INLINE Wen_AppInterface(Wen_AppContext* ctx) : m_context(ctx) {};
    WEN_INLINE virtual ~Wen_AppInterface() = default;

    template<typename Layer>
    WEN_INLINE Layer* get_layer() {
        WEN_STATIC_ASSERT(std::is_base_of<Wen_AppInterface, Layer>::value);
        auto itr = std::find_if(m_context->layers.begin(), m_context->layers.end(), [this](auto layer) {
            return (layer->m_layer_id == Wen_TypeId<Layer>());
        });
        if (itr != m_context->layers.end()) {
            return static_cast<Layer*>(itr->get());
        }

        return nullptr;
    }

    template<typename Layer, typename... Args>
    WEN_INLINE Layer& create_layer(Args&&... args) {
        WEN_STATIC_ASSERT(std::is_base_of<Wen_AppInterface, Layer>::value);
        if (auto exists = get_layer<Layer>()) {
            throw std::runtime_error("layer already exists");
        }

        auto layer = std::make_unique<Layer>(&m_context, std::forward<Args>(args)...);
        layer->m_layer_id = Wen_TypeId<Layer>();
        layer->m_context = m_context;
        Layer& ref = *layer;
        m_context->layers.push_back(std::move(layer));
        return ref;
    }

    WEN_INLINE virtual void OnUpdate(float deltaTime) {};
    WEN_INLINE virtual void OnInit() {};

    private:
    Wen_AppContext* m_context;
    uint32_t m_layer_id;
};
