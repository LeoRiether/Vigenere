#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdio>
#include "vigenere.hpp"
#include "types.hpp"

///
/// Most of the cipher logic is in vigenere.cpp!
///

struct Args {
    char* key; // -k --key
    char* key_file; // -kf --key-file
    char* input; // -i --input
    char* output; // -o --output
    bool b64_input, b64_output; // -i64, -o64
};

Args parse_args(int argc, char* argv[]) {
    Args a{ 0 };
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0 ||
                strcmp(argv[i], "--key") == 0) {
            assert(i+1 < argc && "--key should be followed by a key parameter");
            a.key = argv[++i];
        }
        else if (strcmp(argv[i], "-kf") == 0 ||
                strcmp(argv[i], "--key-file") == 0) {
            assert(i+1 < argc && "--key-file should be followed by a keyfile parameter");
            a.key_file = argv[++i];
        }
        else if (strcmp(argv[i], "-i") == 0 ||
                strcmp(argv[i], "--input") == 0) {
            assert(i+1 < argc && "--input should be followed by a filename ");
            a.input = argv[++i];
        }
        else if (strcmp(argv[i], "-o") == 0 ||
                strcmp(argv[i], "--out") == 0) {
            assert(i+1 < argc && "--out should be followed by a filename");
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

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);

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
    byte_t* key;
    FILE* fkey = nullptr;
    if (args.key) {
        key = reinterpret_cast<byte_t*>(args.key);
    } else if (args.key_file) {
        fkey = fopen(args.key_file, "rb");
        fseek(fkey, 0, SEEK_END); // thx stackoverflow
        long long length = ftell(fkey);
        fseek(fkey, 0, SEEK_SET);
        key = new byte_t[length+1];
        fread(key, sizeof(*key), length, fkey);
    } else {
        assert(false && "At least one of --key or --key-file must be present");
    }

    vigenere::cipher_file(
        key, fmessage, fcipher,
        args.b64_input, args.b64_output
    );

    // Close files
    fclose(fmessage);
    fclose(fcipher);
    if (fkey) {
        fclose(fkey);
        delete[] key;
    }

    return 0;
}
