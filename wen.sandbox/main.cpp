#include <Wen/Wen.h>

int main() {
    auto app = Wen_Engine();
    WEN_TRACE("{} Sandbox", "Trace");
    WEN_DEBUG("{} Sandbox", "Debug");
    WEN_INFO("{} Sandbox", "Info");
    WEN_WARN("{} Sandbox", "Warn");
    WEN_ERROR("{} Sandbox", "Error");
    WEN_FATAL("{} Sandbox", "Fatal");
    return 0;
}
