#pragma once

#include "base64.cpp"
#include "types.hpp"
#include <cstdio>
#include <vector>
#include <algorithm>

using std::vector;

namespace vigenere {

    // Thing that gives me the next byte of the key
    // and loops it back every time it needs to
    struct RollingKey {
        const vector<byte_t>& key;
        int i;
        RollingKey(const vector<byte_t>& _key)
            : key(_key), i(0) {}

        byte_t next() {
            if (i >= (int)key.size())
                i = 0; // loop key back to start
            return key[i++]; // return current key byte and advance pointer
        }
    };

    // It's a multiple of 3 and 4 so we can produce
    // chunks of base64
    // ~1.57MB
    constexpr int CHUNK_SIZE = 3 * (1 << 19);

    // Cipher a chunk of a buffer
    void cipher_chunk(RollingKey& key, vector<byte_t>& buf) {
        for (auto& byte : buf)
            byte ^= key.next();
    }

    // Cipher the message in chunks
    // The input and output files are encoded/decoded in base64
    // when appropriate (TODO:)
    void cipher_file(
        const vector<byte_t>& key, FILE* fmessage, FILE* fcipher,
        bool b64_input, bool b64_output
    ) {

        RollingKey rolling_key(key);
        vector<byte_t> buffer(CHUNK_SIZE+1);
        size_t bytes_read;

        vector<byte_t> b64_buf = b64_input ? vector<byte_t>(CHUNK_SIZE+1) : vector<byte_t>();

        while ((bytes_read = fread(buffer.data(), sizeof(*buffer.data()), CHUNK_SIZE, fmessage)) > 0) {
            buffer.resize(bytes_read);
            cipher_chunk(rolling_key, buffer);

            fwrite(buffer.data(), sizeof(*buffer.data()), buffer.size(), fcipher);
        }
    }
}
