#pragma once

#include "Wen_Core.h"
#include "Wen_Context.h"
#include "Wen_Interface.h"

struct WEN_API Wen_Engine {
    WEN_INLINE Wen_Engine() {
        m_layer_id = Wen_TypeId<Wen_Engine>();
        m_context = std::make_unique<Wen_AppContext>();
    };

    WEN_INLINE ~Wen_Engine() = default;

    WEN_INLINE void Run(float deltaTime) {
        for (auto& layer: m_context->layers) {
            layer->OnUpdate(deltaTime);
        }
    }

    Wen_Logger logger;

    private:
    uint32_t m_layer_id;
    std::unique_ptr<Wen_AppContext> m_context;
};
