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
        vector<byte_t> input(n);
        for (int i = 0; i < n; i++)
            input[i] = gen(rnd);

        // Encode it in base64
        vector<byte_t> encoded;
        base64::encode(input, encoded);

        // Decode it
        vector<byte_t> output;
        base64::decode(encoded, output);

        TS_ASSERT_EQUALS(input.size(), output.size());
        for (size_t i = 0; i < input.size(); i++)
            TS_ASSERT_EQUALS(input[i], output[i]);
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
