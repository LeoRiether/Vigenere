#include <iostream>
#include <cstring>
#include <cassert>
#include <istream>
#include <ostream>
#include <sstream> 
#include <fstream>
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
    }
    return a;
}

ostream& vigenere(ostream& cipher, istream& message, const byte_t* key) {
    byte_t m;
    const byte_t* keyptr = key;
    while (message.read(reinterpret_cast<char*>(&m), sizeof(m))) {
        if (*keyptr == 0)
            keyptr = key;

        byte_t c = m ^ *keyptr;
        cipher.write(reinterpret_cast<char*>(&c), sizeof(c));
        keyptr++;
    }
    return cipher;
}

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);

    using std::ios;

    // Open `message` stream (either std::cin or a file,
    // depending on args.input)
    std::ifstream message_;
    if (args.input)
        message_.open(args.input, ios::in | ios::binary);
    auto& message = args.input ? message_ : std::cin;

    // Open `cipher` stream (either std::cout or a file,
    // depending on args.output)
    std::ofstream cipher_;
    if (args.output)
        cipher_.open(args.output, ios::out | ios::binary);
    auto& cipher = args.output ? cipher_ : std::cout;

    byte_t key[] = "Hello World!";
    vigenere(cipher, message, key);

    return 0;
}
