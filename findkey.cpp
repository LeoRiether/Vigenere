#include "types.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include <vector>

using std::vector;

// From
// https://www3.nd.edu/~busiforc/handouts/cryptography/letterfrequencies.html
std::unordered_map<byte_t, int> score_table = {
    { 'A', 4331 }, { 'B', 1056 }, { 'C', 2313 },
    { 'D', 1725 }, { 'E', 5688 }, { 'F', 924 },
    { 'G', 1259 }, { 'H', 1531 }, { 'I', 3845 },
    { 'J', 100 }, { 'K', 561 }, { 'L', 2798 },
    { 'M', 1536 }, { 'N', 3392 }, { 'O', 3651 },
    { 'P', 1614 }, { 'Q', 100 }, { 'R', 3864 },
    { 'S', 2923 }, { 'T', 3543 }, { 'U', 1851 },
    { 'V', 513 }, { 'W', 657 }, { 'X', 148 },
    { 'Y', 906 }, { 'Z', 139 }, { ' ', 50 }
};

struct KeyFinder {
    const vector<byte_t>& cipher;
    KeyFinder(const vector<byte_t>& _cipher)
        : cipher(_cipher) {}

    // Builds a histogram/frequency table for every
    // k-th byte of the cipher, starting at index `start`
    vector<int> build_histogram(int start, int k) {
        int n = cipher.size();
        vector<int> hist(256);
        for (int i = start; i < n; i += k) {
            int byte = cipher[i];
            hist[byte]++;
        }
        return hist;
    }

    // Finds the most likely key with length k
    vector<byte_t> with_length(int k) {
        vector<byte_t> guess(k);
        vector<long long> score;
        for (int i = 0; i < k; i++) {
            score.assign(256, 0);
            int n = cipher.size();
            for (int j = i; j < n; j += k) {
                for (int candidate = 0; candidate < 256; candidate++) {
                    byte_t b = cipher[j] ^ candidate;
                    if (b >= 'a' && b <= 'z')
                        b = b - 'a' + 'A';

                    if (score_table.count(b))
                        score[candidate] += score_table[b];
                }
            }

            byte_t best = std::max_element(score.begin(), score.end()) - score.begin();
            guess[i] = best;
        }
        return guess;
    }
};


int main(int argc, char* argv[]) {

    return 0;
}

