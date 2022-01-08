#include <bits/stdc++.h>

using namespace std;

vector<int> sol;
vector<int> v_t[1000001], ctc[1000001], st, v[1000001];
int used[1000001], nrctc, n, m;
vector<bool> bad(1000000, true);

void dfs(int nod) {
    used[nod] = 1;
    for (int vecin : v[nod]) {
        if (!used[vecin] && !bad[vecin]) dfs(vecin);
    }
    st.emplace_back(nod);
}

void dfs_t(int nod) {
    used[nod] = 2;
    for (int vecin : v_t[nod]) {
        if (used[vecin] != 2 && !bad[vecin]) dfs_t(vecin);
    }
    ctc[nrctc].emplace_back(nod);
}

void solve() {
    int cnt = 0;
    for (int i = 1; i <= n; ++i) {
        if (!used[i] && !bad[i]) {
            ++cnt;
            dfs(i);
        }
    }
    while (!st.empty()) {
        int nod = st.back();
        if (used[nod] != 2 && !bad[nod]) dfs_t(nod), ++nrctc;
        st.pop_back();
    }
    for (int i = 0; i < nrctc; ++i) {
        if (ctc[i].size() != 1) {
            cout << "!!!\n";
            exit(0);
        }
    }
}

signed main() {

    for (int t = 1; t <= 32; ++t) {
        bad.clear();
        bad.resize(1000005, false);
        cout << "test " << t << " started\n";
        string path_in =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\correct-testcases\grader_test)" +
                to_string(t) +
                ".in";
        string path_out =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)" +
                to_string(t) +
                ".out";
        ifstream in2(path_in);
        ifstream in(path_out);
        int x, y;

        in >> n;
        for (int i = 1; i <= n; ++i) {
            in >> x;
            bad[x] = true;
        }
        in.close();
        in2 >> n >> m;
        for (int i = 1; i <= m; ++i) {
            in2 >> x >> y;
            if (used[x] || used[y]) continue;
            v[x].push_back(y);
            v_t[y].push_back(x);
        }

        solve();
        cout << "test " << t << " finished  ";
        cout << ":)\n";

        sol.clear();
        for (int i = 0; i <= n; ++i) {
            v[i].clear();
            v_t[i].clear();
            ctc[i].clear();
            used[i] = false;
        }
        nrctc = 0;
        st.clear();
    }


    return 0;
}