#include <bits/stdc++.h>

using namespace std;
const int NR = 1e5 + 1;
int p, n, x[NR], fr[10];
ifstream in("balba.in");
ofstream out("balba.out");

signed main() {
    ios_base::sync_with_stdio(false);
    in.tie(nullptr);
    in >> p >> n;
    int sol1 = 0, sol2 = 0;
    for (int i = 1; i <= n; ++i) {
        in >> x[i];
        ++fr[x[i]];
    }
    if (p == 1) {
        for (int i = 1, j; i <= n; i = j, sol1++) {
            for (j = i; j <= n && x[i] == x[j]; ++j);
            if (j > i + 1) {
                sol2++;
            }
        }
        out << sol1 << ' ' << sol2 << '\n';
        return 0;
    }

    for (int sw = 0, i = 9; i; --i) {
        for (int j = 1; j <= fr[i] / 2; ++j) {
            out << i;
        }
        if (fr[i] % 2 && !sw) {
            sw = 1;
            ++fr[i];
        }
    }
    for (int i = 0; i <= 9; ++i) {
        for (int j = 1; j <= fr[i] / 2; ++j) {
            out << i;
        }
    }
    return 0;
}
