#pragma once

#include "types.hpp"
#include <iostream>
#include <string>
#include <cassert>
using std::string;

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

    string encode(const byte_t* buf, const int n) {
        ensure_encoding_built();
        string e;
        int pad = (3 - n%3)%3;
        e.reserve((4*n + 2) / 3 + pad); // == ceil(4n / 3 + pad)
        for (int i = 0; i < n; i += 3) {
            //  buf[0]   buf[1]   buf[2]
            // 76543210 76543210 76543210
            //          becomes
            // 765432 107654 321076 543210
            //  e[0]   e[1]   e[2]   e[3]
            auto at = [&](int j) { return j >= n ? 0 : buf[j]; };
            auto push = [&](int x) { e.push_back(encoding[x]); };
            push(at(i) >> 2);
            push(((at(i) & 3) << 4) | (at(i+1) >> 4));

            if (i+1 >= n) e.push_back('=');
            else push(((at(i+1) & 15) << 2) | (at(i+2) >> 6));

            if (i+2 >= n) e.push_back('=');
            else push(at(i+2) & 63);
        }
        return e;
    }

    // Decode `s` and put the output into `out`
    int decode(const string& s, byte_t* out) {
        ensure_encoding_built();
        int p = 0;
        int n = s.size();
        for (int i = 0; i < n; i += 4) {
            // Inverse of the `encode` mapping
            auto at = [&](int j) { return inv_encoding[(int)s[j]]; };
            out[p++] = (at(i) << 2) | (at(i+1) >> 4);

            if (s[i+2] != '=')
                out[p++] = ((at(i+1) & 15) << 4) | (at(i+2) >> 2);

            if (s[i+3] != '=')
                out[p++] = ((at(i+2) & 3) << 6) | at(i+3);
        }
        out[p] = 0;
        return p;
    }
}
