#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdio>
#include "types.h"

// It's a multiple of 3 and 4 so we can produce                                         
// chunks of base64 
// ~1.57MB                                                                              
constexpr int CHUNK_SIZE = 3 * (1 << 19); 

struct Args {
    char* key; // -k --key
    char* key_file; // -kf --keyfile
    char* input; // -i --input
    char* output; // -o --output
    bool b64_input, b64_output; // -i64, -o64
};

Args parse_args(int argc, char* argv[]) {
    Args a{ 0 };
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0 ||
                strcmp(argv[i], "--key") == 0) {
            assert(i+1 < argc);
            a.key = argv[++i];
        }
        else if (strcmp(argv[i], "-kf") == 0 ||
                strcmp(argv[i], "--keyfile") == 0) {
            assert(i+1 < argc);
            a.key_file = argv[++i];
        }
        else if (strcmp(argv[i], "-i") == 0 ||
                strcmp(argv[i], "--input") == 0) {
            assert(i+1 < argc);
            a.input = argv[++i];
        }
        else if (strcmp(argv[i], "-o") == 0 ||
                strcmp(argv[i], "--out") == 0) {
            assert(i+1 < argc);
            a.output = argv[++i];
        }
        else if (strcmp(argv[i], "-i64") == 0) {
            a.b64_input = true;
        }
        else if (strcmp(argv[i], "-o64") == 0) {
            a.b64_output = true;
        }
    }
    return a;
}

struct RollingKey {
    const byte_t* base, *cur;
    RollingKey(const byte_t* _base)
        : base(_base), cur(_base) {}

    byte_t next() {
        if (*cur == 0)
            cur = base;
        return *(cur++);
    }
};

void vigenere(RollingKey& key, byte_t* buf, int bufsz) {
    for (int i = 0; i < bufsz; i++)
        buf[i] ^= key.next(); // C is so concise sometimes
}

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);

    // Open files
    FILE* fmessage;
    if (args.input)
        fmessage = fopen(args.input, "rb");
    else {
        freopen(NULL, "rb", stdin); // reopen stdin in binary mode
        fmessage = stdin;
    }

    FILE* fcipher;
    if (args.input)
        fcipher = fopen(args.output, "wb");
    else {
        freopen(NULL, "wb", stdout); // reopen stdout in binary mode
        fcipher = stdout;
    }

    // Get a key 
    byte_t key[] = "Hello World!";
    RollingKey rolling_key(key);

    // Cipher the message in chunks
    byte_t* buffer = new byte_t[CHUNK_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(*buffer), CHUNK_SIZE, fmessage)) > 0) {
        vigenere(rolling_key, buffer, bytes_read);
        fwrite(buffer, sizeof(*buffer), bytes_read, fcipher);
    }

    // Free resources
    delete[] buffer;
    free(fmessage);
    free(fcipher);

    return 0;
}
