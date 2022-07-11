#pragma once
#include <vector>
#include <algorithm>

using ll = long long;
using ld = long double;

// {{{ Pollard-Rho implementation
// Ok I just copied this from
// https://github.com/Tiagosf00/Competitive-Programming/blob/master/C%2B%2B/Math/pollard_rho.cpp
// But Tiagosf00 is a member of my ICPC team so it's ok!

namespace pollard_rho {
    ll my_gcd(ll a, ll b) {
        if (a == 0)
            return b;
        return my_gcd(b % a, a);
    }

    ll mul(ll a, ll b, ll m) {
        ll ret = a*b - (ll)((ld)1/m*a*b+0.5)*m;
        return ret < 0 ? ret+m : ret;
    }

    ll pow(ll a, ll b, ll m) {
        ll ans = 1;
        for (; b > 0; b /= 2ll, a = mul(a, a, m)) {
            if (b % 2ll == 1)
                ans = mul(ans, a, m);
        }
        return ans;
    }

    bool prime(ll n) {
        if (n < 2) return 0;
        if (n <= 3) return 1;
        if (n % 2 == 0) return 0;

        ll r = __builtin_ctzll(n - 1), d = n >> r;
        std::vector<int> some_primes {2, 325, 9375, 28178, 450775, 9780504, 795265022};
        for (int a : some_primes) {
            ll x = pow(a, d, n);
            if (x == 1 or x == n - 1 or a % n == 0) continue;
            
            for (int j = 0; j < r - 1; j++) {
                x = mul(x, x, n);
                if (x == n - 1) break;
            }
            if (x != n - 1) return 0;
        }
        return 1;
    }

    ll rho(ll n) {
        if (n == 1 or prime(n)) return n;
        auto f = [n](ll x) {return mul(x, x, n) + 1;};

        ll x = 0, y = 0, t = 30, prd = 2, x0 = 1, q;
        while (t % 40 != 0 or my_gcd(prd, n) == 1) {
            if (x==y) x = ++x0, y = f(x);
            q = mul(prd, llabs(x-y), n);
            if (q != 0) prd = q;
            x = f(x), y = f(f(y)), t++;
        }
        return my_gcd(prd, n);
    }

    std::vector<ll> fact(ll n) {
        if (n == 1) return {};
        if (prime(n)) return {n};
        ll d = rho(n);
        std::vector<ll> l = fact(d), r = fact(n / d);
        l.insert(l.end(), r.begin(), r.end());
        return l;
    }
}
// }}}

std::vector<ll> divisors_from_factors(const std::vector<ll> factors, size_t i = 0) {
    if (i >= factors.size())
        return { 1 };

    // factors[i] ^ exponent divides the number
    int exponent = 1;
    while (i+1 < factors.size() && factors[i+1] == factors[i])
        i++;

    auto divs = divisors_from_factors(factors, i+1);
    auto ans = divs;
    ll p = factors[i]; // We'll iterate from p^1 to p^exponent
    for (int j = 1; j <= exponent; j++) {
        for (auto x : divs)
            ans.push_back(x * p);
        p *= factors[i];
    }
    return ans;
}

std::vector<ll> divisors(ll x) {
    auto f = pollard_rho::fact(x);
    auto ans = divisors_from_factors(f);
    std::sort(ans.begin(), ans.end());
    return ans;
}
