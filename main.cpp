#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdio>
#include "types.h"

using std::ifstream;
using std::ofstream;
using std::istream;
using std::ostream;

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

void vigenere(byte_t* message, const byte_t* key) {
    const byte_t* keyptr = key;
    while (*message) {
        // loop key back to beginning
        if (*keyptr == 0)
            keyptr = key;

        *(message++) ^= *(keyptr++); // C is so concise sometimes
    }
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

    // Get file size
    fseek(fmessage, 0, SEEK_END);
    int length = ftell(fmessage);
    fseek(fmessage, 0, SEEK_SET);

    // Read fmessage into the message buffer
    byte_t* message = (byte_t*)malloc(sizeof(*message) * (length+1)); 
    fread(message, sizeof(*message), length, fmessage);
    message[length] = 0;

    // Make cipher in-place
    byte_t key[] = "Hello World!";
    vigenere(message, key);

    // Write cipher to cipher file
    fwrite(message, sizeof(*message), length, fcipher);

    // Free everything
    free(message);
    fclose(fmessage);
    fclose(fcipher);

    return 0;
}
