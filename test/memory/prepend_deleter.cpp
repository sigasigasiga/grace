#include <memory>

import grace.memory;

int g_invoke_counter = 0;

void test() {
    using namespace grace::memory;

    auto p = std::make_unique<int>();

    auto p2 = prepend_deleter(std::move(p), [](int *) { ++g_invoke_counter; });
}

int main() {
    test();

    if (g_invoke_counter != 1) {
        throw std::runtime_error("prepend_deleter did not invoke deleter correctly");
    }
}
