#pragma once
#include "../base64.cpp"
#include "../types.hpp"
#include <cxxtest/TestSuite.h>
#include <random>
#include <chrono>

class Base64TestSuite : public CxxTest::TestSuite {
private:
    std::mt19937 rnd;

    void onceInversibility() {
        std::uniform_int_distribution gen(0, 255);

        // Initialize random string
        int n = gen(rnd);
        byte_t buf[n+1];
        for (int i = 0; i < n; i++)
            buf[i] = gen(rnd);
        buf[n] = 0;

        // Encode it in base64
        auto encoded = base64::encode(buf, n);

        // Decode it
        byte_t out[n+1];
        int m = base64::decode(encoded, out);

        TS_ASSERT_EQUALS(n, m);
        for (int i = 0; i < n; i++)
            TS_ASSERT_EQUALS(buf[i], out[i]);
    }
public:
    Base64TestSuite() {
        auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
        rnd = std::mt19937(seed);
    }

    void testInversibility() {
        for (int i = 0; i < 1000; i++)
            onceInversibility();
    }
};
