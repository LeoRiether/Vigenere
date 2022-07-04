#pragma once

#include "types.hpp"
#include <cstdio>

namespace vigenere {

    // Thing that gives me the next byte of the key
    // and loops it back every time it needs to
    struct RollingKey {
        const byte_t* base, *cur;
        RollingKey(const byte_t* _base)
            : base(_base), cur(_base) {}

        byte_t next() {
            if (*cur == 0) // loop back the key
                cur = base;
            return *(cur++); // return *cur and advance pointer
        }
    };

    // It's a multiple of 3 and 4 so we can produce
    // chunks of base64
    // ~1.57MB
    constexpr int CHUNK_SIZE = 3 * (1 << 19);

    // Cipher a chunk of a buffer
    void cipher_chunk(RollingKey& key, byte_t* buf, int bufsz) {
        for (int i = 0; i < bufsz; i++)
            buf[i] ^= key.next();
    }

    // Cipher the message in chunks
    // The input and output files are encoded/decoded in base64
    // when appropriate (TODO:)
    void cipher_file(
        byte_t* key, FILE* fmessage, FILE* fcipher,
        bool b64_input, bool b64_output
    ) {

        RollingKey rolling_key(key);
        byte_t* buffer = new byte_t[CHUNK_SIZE+1];
        size_t bytes_read;

        while ((bytes_read = fread(buffer, sizeof(*buffer), CHUNK_SIZE, fmessage)) > 0) {
            cipher_chunk(rolling_key, buffer, bytes_read);
            fwrite(buffer, sizeof(*buffer), bytes_read, fcipher);
        }

        delete[] buffer;
    }
}
