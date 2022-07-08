#pragma once

#include "types.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::vector;

namespace base64 {
    byte_t encoding[64];
    byte_t inv_encoding[256];
    bool built = false;
    void ensure_encoding_built() {
        if (built) return;
        built = true;

        int p = 0;
        for (byte_t c = 'A'; c <= 'Z'; c++) // 26
            encoding[p++] = c;
        for (byte_t c = 'a'; c <= 'z'; c++) // 26
            encoding[p++] = c;
        for (byte_t c = '0'; c <= '9'; c++) // 10
            encoding[p++] = c;
        encoding[p++] = '+'; // 1
        encoding[p++] = '/'; // 1

        for (int i = 0; i < 64; i++)
            inv_encoding[(int)encoding[i]] = i;
    }

    void encode(const vector<byte_t>& input, vector<byte_t>& output) {
        ensure_encoding_built();

        long n = input.size();
        long pad = (3 - n%3)%3;
        long out_sz = (4*n + 2) / 3 + pad; // == ceil(4n / 3 + pad)
        output.resize(out_sz);

        long op = 0;
        for (int i = 0; i < n; i += 3) {
            //  buf[0]   buf[1]   buf[2]
            // 76543210 76543210 76543210
            //          becomes
            // 765432 107654 321076 543210
            //  e[0]   e[1]   e[2]   e[3]
            auto at = [&](int j) { return j >= n ? 0 : input[j]; };
            auto push = [&](int x) { output[op++] = encoding[x]; };
            push(at(i) >> 2);
            push(((at(i) & 3) << 4) | (at(i+1) >> 4));

            if (i+1 >= n) output[op++] = '=';
            else push(((at(i+1) & 15) << 2) | (at(i+2) >> 6));

            if (i+2 >= n) output[op++] = '=';
            else push(at(i+2) & 63);
        }
    }

    // Decode `s` and put the output into `out`
    void decode(const vector<byte_t> input, vector<byte_t>& output) {
        ensure_encoding_built();
        long n = input.size();
        output.resize(0);
        long p = 0;
        for (int i = 0; i < n; i += 4) {
            // Inverse of the `encode` mapping
            auto at = [&](int j) { return inv_encoding[(int)input[j]]; };
            output[p++] = (at(i) << 2) | (at(i+1) >> 4);

            if (input[i+2] != '=')
                output[p++] = ((at(i+1) & 15) << 4) | (at(i+2) >> 2);

            if (input[i+3] != '=')
                output[p++] = ((at(i+2) & 3) << 6) | at(i+3);
        }
    }
}
