#include <bits/stdc++.h>

using namespace std;
const int NR = 1e6 + 16;
int n, k;
int a[NR];
map<int, int> mp;

signed main() {
    cin >> n >> k;
    for (int i = 1; i <= n; ++i) {
        cin >> a[i];
        mp[a[i]] = 1 + mp[a[i]];
    }
    int best = 0;
    for (auto it : mp) {
        cout << it.first << '\n';
        best = max(best, it.second + mp[it.first + k]);
    }
    cout << best << '\n';
    return 0;
}