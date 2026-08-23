#include <concepts>
#include <limits>

import grace.algorithm;

using grace::algorithm::clamp_to;

template<typename From, typename To>
concept clampable = requires(From f) {
    { clamp_to<To>(f) } -> std::same_as<To>;
};

consteval void test() {
    if (clamp_to<int>(0) != 0) {
        throw "base case failed";
    }

    if (clamp_to<int>(1ll << 50) != std::numeric_limits<int>::max()) {
        throw "upper bound failed";
    }

    if (clamp_to<int>(-(1ll << 50)) != std::numeric_limits<int>::min()) {
        throw "lower bound failed";
    }

    if (clamp_to<int>(std::numeric_limits<int>::max()) != std::numeric_limits<int>::max()) {
        throw "upper bound no-op failed";
    }

    if (clamp_to<int>(std::numeric_limits<int>::min()) != std::numeric_limits<int>::min()) {
        throw "lower bound no-op failed";
    }

    if (clamp_to<unsigned char>(3000u) != std::numeric_limits<unsigned char>::max()) {
        throw "unsigned upper bound failed";
    }

    if (clampable<int, long int>) {
        throw "must not clamp to larger type";
    }

    if (clampable<int, unsigned int>) {
        throw "must not clamp to different signedness 1";
    }

    if (clampable<unsigned int, int>) {
        throw "must not clamp to different signedness 2";
    }
}

int main() {
    test();
}
