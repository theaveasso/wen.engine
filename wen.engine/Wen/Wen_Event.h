#pragma once

#include "Wen_Core.h"

template<typename Event>
struct Wen_EventListener {
    using CallbackFn = std::function<void(const Event&)>;

    WEN_INLINE Wen_EventListener(CallbackFn&& callback, uint32_t listener_id):
        callback(std::move(callback)), id(listener_id) {}

    CallbackFn callback;
    uint32_t id;
};

template<typename Event>
struct Wen_EventRegistry {
    using Listener = std::unique_ptr<Wen_EventListener<Event>>;
    std::queue<std::unique_ptr<Event>> queue;
    std::vector<Listener> listeners;
};

struct Wen_EventDispatcher {

    WEN_INLINE void poll_events() {
        WEN_INFO("polling event");
        // for (auto& [_, ptr]: m_registry) {
        //     auto registry = cast_registry<char>(ptr);
        //     while(!registry->queue.empty()) {
        //         for (auto& listener: registry->listeners) {
        //             listener->callback(*registry->queue.front());
        //         }
        //     }
        // }
    }

    private:
    template<typename Event>
    WEN_INLINE Wen_EventRegistry<Event>* cast_registry(void* ptr) {
        return reinterpret_cast<Wen_EventRegistry<Event>*>(ptr);
    }

    private:
    std::unordered_map<uint32_t, void*> m_registry;
    std::queue<std::function<void()>> m_tasks;
};
