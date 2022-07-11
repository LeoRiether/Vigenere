// Includes {{{ 
#include "pollard_rho.cpp"
#include "scoring.cpp"
#include "types.hpp"
#include "util.cpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>
// }}}

using std::vector;
using std::string;
using std::unordered_map;
using std::map;

// Arg parser {{{
struct Args {
    char* input; // -i --input
    char* output; // -o --output
    int lengths; // -l --lengths
};

#define IS(x, y) strcmp(x, y) == 0
Args parse_args(int argc, char* argv[]) {
    Args a{ 0 };
    a.lengths = 10;
    for (int i = 1; i < argc; i++) {
        if (IS(argv[i], "-i") || IS(argv[i], "--input")) {
            assert(i+1 < argc && "--input should be followed by a filename ");
            a.input = argv[++i];
        }
        else if (IS(argv[i], "-o") || IS(argv[i], "--output")) {
            assert(i+1 < argc && "--output should be followed by a filename ");
            a.output = argv[++i];
        }
        else if (IS(argv[i], "-l") || IS(argv[i], "--lengths")) {
            assert(i+1 < argc && "--lengths should be followed by a number");
            a.lengths = atoi(argv[++i]);
        }
        else if (IS(argv[i], "-min")) {
            assert(i+1 < argc && "-min should be followed by a number");
            a.min = atoi(argv[++i]);
        }
        else if (IS(argv[i], "-max")) {
            assert(i+1 < argc && "-max should be followed by a number");
            a.max = atoi(argv[++i]);
        }
    }
    return a;
}
#undef IS
/// }}}

// KeyFinder {{{
struct KeyFinder {
    const vector<byte_t>& cipher;
    const score_table_t& scoring;

    KeyFinder(const vector<byte_t>& _cipher, const score_table_t& _scoring)
        : cipher(_cipher), scoring(_scoring) {}

    // Finds the k most likely key lengths {{{
    vector<int> most_likely_lengths(int k) {
        using byte3 = std::array<byte_t, 3>;
        map<byte3, int> last_occurence;
        unordered_map<int, int> deltas;
        unordered_map<int, vector<long long>> divisor_cache;
        int n = cipher.size();
        byte3 s;
        for (int i = 0; i+2 < n; i++) {
            s = { cipher[i], cipher[i+1], cipher[i+2] };

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

        // Sort entries to get the k most frequent deltas
        vector<std::pair<int, int>> vdelta(deltas.size());
        {
            int i = 0;
            for (const auto& [delta, cnt] : deltas)
                vdelta[i++] = { cnt, delta };
            std::sort(vdelta.rbegin(), vdelta.rend());
        }

        // Put the k most frequent deltas into the `likely` vector
        vector<int> likely;
        for (const auto& entry : vdelta) {
            if ((int)likely.size() >= k) break;
            likely.push_back(entry.second);
        }
        return likely;
    }
    // }}}

    struct Result {
        long long score;
        vector<byte_t> key;

        bool operator>(const Result& rhs) const {
            if (score != rhs.score)
                return score > rhs.score;
            return key.size() < rhs.key.size();
        }
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

                    auto it = scoring.find(b);
                    if (it != scoring.end())
                        score[candidate] += it->second;
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

    Args args = parse_args(argc, argv);
    assert(args.input && "--input flag must be present");

    vector<byte_t> cipher = read_all_bytes(args.input);
    KeyFinder findkey(cipher, english_score);

    // Find most likely key lengths {{{
    std::cerr << "Most likely lengths: ";
    auto likely_lengths = findkey.most_likely_lengths(args.lengths);
    for (auto x : likely_lengths)
        std::cerr << x << ' ';
    std::cerr << std::endl;
    // }}}

    // Find most likely key for each length {{{
    vector<KeyFinder::Result> results;
    std::mutex mutex; // too lazy to research how to properly use
                      // mutexes in C++, this'll do

    auto locked = [&](auto&& f) {
        mutex.lock();
        f();
        mutex.unlock();
    };
    
    // Find the most likely key for some particular length `len` {{{
    auto process_len = [&](int len) {
        locked([&]() {
            std::cerr << "Finding key with length " << len
                      << "..." << std::endl;
        });

        // Potentially takes a lot of time
        auto result = findkey.with_length(len);

        locked([&]() {
            std::cerr << "len " << len <<  " scored "
                      << log(result.score) / log(1.15)
                      << std::endl;

            if (is_readable(result.key)) {
                for (auto b : result.key)
                    std::cerr << b;
                std::cerr << std::endl;
            } else {
                std::cerr << "<unreadable>" << std::endl;
            }

            results.emplace_back(std::move(result));
        });
    };
    /// }}}

    // Process each length in a different thread
    vector<std::thread> handles;
    for (auto len : likely_lengths)
        handles.push_back(std::thread(process_len, len));
    for (auto& handle : handles)
        handle.join();
    // }}}

    // Output the result that scored better {{{
    FILE* fout;
    if (args.output)
        fout = fopen(args.output, "wb");
    else {
        SET_BINARY_MODE(stdout);
        fout = stdout;
    }

    auto best = results[0];
    for (const auto& r : results) {
        if (r > best)
            best = r;
    }
    fwrite(best.key.data(), sizeof(byte_t), best.key.size(), fout);
    // }}}

    fclose(fout);

    return 0;
}

