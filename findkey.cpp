#include "pollard_rho.cpp"
#include "types.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_map>
#include <vector>

using std::vector;
using std::string;
using std::unordered_map;

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

// {{{ KeyFinder
struct KeyFinder {
    const vector<byte_t>& cipher;
    KeyFinder(const vector<byte_t>& _cipher)
        : cipher(_cipher) {}

    // (I'm not actually using this procedure)
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

    // Finds the k most likely key lengths
    vector<int> most_likely_lengths(int k) {
        unordered_map<string, int> last_occurence;
        unordered_map<int, int> deltas;
        unordered_map<int, vector<long long>> divisor_cache;
        int n = cipher.size();
        string s(3, 0);
        for (int i = 0; i+2 < n; i++) {
            s[0] = cipher[i];
            s[1] = cipher[i+1];
            s[2] = cipher[i+2];

            if (last_occurence.count(s)) {
                int delta = i - last_occurence[s];
                if (!divisor_cache.count(delta))
                    divisor_cache[delta] = divisors(delta);

                for (auto d : divisor_cache[delta]) {
                    if (d != 1)
                        deltas[d]++;
                }
            }

            last_occurence[s] = i;
        }

        vector<std::pair<int, int>> vdelta(deltas.size());
        {
            int i = 0;
            for (const auto& [delta, cnt] : deltas)
                vdelta[i++] = { cnt, delta };
            std::sort(vdelta.rbegin(), vdelta.rend());
        }

        vector<int> likely;
        for (const auto& entry : vdelta) {
            if ((int)likely.size() >= k) break;
            likely.push_back(entry.second);
        }
        return likely;
    }

    struct Result {
        long long score;
        vector<byte_t> key;
    };

    // Finds the most likely key with length k
    // based on some scoring function
    Result with_length(int k) {
        vector<byte_t> guess(k);
        long long total_score = 0;
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
            total_score += score[best];
            guess[i] = best;
        }
        return { total_score, guess };
    }
};
// }}}

int main(int argc, char* argv[]) {

    vector<byte_t> cipher;

    std::cerr << "Reading cipher file..." << std::endl;

    // TODO: extract into a function
    FILE* fcipher = fopen("examples/shakespeare.cipher", "rb");
    fseek(fcipher, 0, SEEK_END); // thx stackoverflow
    long long length = ftell(fcipher);
    fseek(fcipher, 0, SEEK_SET);
    cipher.resize(length);
    fread(cipher.data(), sizeof(*cipher.data()), length, fcipher);
    fclose(fcipher);


    KeyFinder findkey(cipher);
    std::cerr << "Most likely lengths: ";
    auto likely_lengths = findkey.most_likely_lengths(6); 
    for (auto x : likely_lengths)
        std::cerr << x << ' ';
    std::cerr << std::endl;

    for (auto len : likely_lengths) {
        std::cerr << "Finding key with length " << len
                  << "..." << std::endl;

        auto result = findkey.with_length(len);
        for (auto b : result.key)
            std::cout << char(b);
        std::cerr << std::endl << "(score = " << result.score << ")" << std::endl;
    }

    return 0;
}

