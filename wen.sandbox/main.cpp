#include <Wen/Wen.h>

int main() {
    auto app = Wen_Engine();
    auto app_id = Wen_TypeId<Wen_Engine>();
    WEN_INFO("Sandbox {}", app_id);
    return 0;
}
