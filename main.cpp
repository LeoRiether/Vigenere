#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdio>
#include "vigenere.cpp"
#include "types.hpp"

// Sets a file handle to binary mode  
// https://stackoverflow.com/a/1613677
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define SET_BINARY_MODE(handle) setmode(handle, O_BINARY)
#else
# define SET_BINARY_MODE(handle) ((void)0)
#endif

///
/// Most of the cipher logic is in vigenere.cpp!
///

// {{{ Arg parser
struct Args {
    char* key; // -k --key
    char* key_file; // -kf --key-file
    char* input; // -i --input
    char* output; // -o --output
    bool b64_input, b64_output; // -i64, -o64
};

#define IS(x, y) strcmp(x, y) == 0
Args parse_args(int argc, char* argv[]) {
    Args a{ 0 };
    for (int i = 1; i < argc; i++) {
        if (IS(argv[i], "-k") || IS(argv[i], "--key")) {
            assert(i+1 < argc && "--key should be followed by a key parameter");
            a.key = argv[++i];
        }
        else if (IS(argv[i], "-kf") || IS(argv[i], "--key-file")) {
            assert(i+1 < argc && "--key-file should be followed by a keyfile parameter");
            a.key_file = argv[++i];
        }
        else if (IS(argv[i], "-i") || IS(argv[i], "--input")) {
            assert(i+1 < argc && "--input should be followed by a filename ");
            a.input = argv[++i];
        }
        else if (IS(argv[i], "-o") || IS(argv[i], "--out")) {
            assert(i+1 < argc && "--out should be followed by a filename");
            a.output = argv[++i];
        }
        else if (IS(argv[i], "-i64")) {
            a.b64_input = true;
        }
        else if (IS(argv[i], "-o64")) {
            a.b64_output = true;
        }
    }
    return a;
}
#undef IS
/// }}}

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);

    // {{{ Read input, cipher and key
    FILE* fmessage;
    if (args.input)
        fmessage = fopen(args.input, "rb");
    else {
        SET_BINARY_MODE(stdin);
        fmessage = stdin;
    }

    FILE* fcipher;
    if (args.output)
        fcipher = fopen(args.output, "wb");
    else {
        SET_BINARY_MODE(stdout);
        fcipher = stdout;
    }

    // Get a key
    vector<byte_t> key;
    if (args.key) {
        // Copy args.key to vector
        char* p = args.key;
        while (*p)
            key.push_back(*(p++));
    } else if (args.key_file) {
        // Read key from file
        FILE* fkey = fopen(args.key_file, "rb");
        fseek(fkey, 0, SEEK_END); // thx stackoverflow
        long long length = ftell(fkey);
        fseek(fkey, 0, SEEK_SET);
        key.resize(length);
        fread(key.data(), sizeof(*key.data()), length, fkey);
        fclose(fkey);
    } else {
        assert(false && "At least one of --key or --key-file must be present");
    }
    /// }}}

    vigenere::cipher_file(
        key, fmessage, fcipher,
        args.b64_input, args.b64_output
    );

    // {{{ Close files
    fclose(fmessage);
    fclose(fcipher);
    // }}}

    return 0;
}
