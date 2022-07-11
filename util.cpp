#pragma once

#include "types.hpp"
#include <vector>
#include <cstdio>

// Sets a file handle to binary mode  
// https://stackoverflow.com/a/1613677
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define SET_BINARY_MODE(handle) setmode(handle, O_BINARY)
#else
# define SET_BINARY_MODE(handle) ((void)0)
#endif

// thx stackoverflow
std::vector<byte_t> read_all_bytes(const char* filename) {
    FILE* file = fopen(filename, "rb");

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::vector<byte_t> buf(length);
    fread(buf.data(), sizeof(*buf.data()), length, file);

    fclose(file);
    return buf; 
}
