#include <bits/stdc++.h>

using namespace std;

vector<int> sol;
vector<int> ctc[1000001], st;
int used[1000001], nrctc, n, m;
vector<int> bad;
vector<int> lowLevel, sccStack, currLevel, whichSCC;
vector<pair<pair<int, set<int>::iterator>, int> > stackTarjan;
vector<bool> inStack;
vector<vector<int>> scc;
vector<set<int>> outDegree;
vector<set<int>> inDegree;
int sccIndex, sccCounter;


void runTarjan(int node) {
    stackTarjan.emplace_back(make_pair(node, outDegree[node].begin()), 0);
    for (; !stackTarjan.empty();) {
        if (stackTarjan.back().second) {
            lowLevel[stackTarjan.back().first.first] = min(lowLevel[stackTarjan.back().first.first],
                                                           lowLevel[*stackTarjan.back().first.second]);
            stackTarjan.back().first.second++;
        }
        if (stackTarjan.back().first.second == outDegree[stackTarjan.back().first.first].begin() &&
            !stackTarjan.back().second) {
            lowLevel[stackTarjan.back().first.first] = ++sccIndex;
            currLevel[stackTarjan.back().first.first] = sccIndex;
            inStack[stackTarjan.back().first.first] = true;
            sccStack.emplace_back(stackTarjan.back().first.first);
        }
        if (stackTarjan.back().first.second != outDegree[stackTarjan.back().first.first].end()) {
            if (!currLevel[*stackTarjan.back().first.second]) {
                stackTarjan.back().second = 1;
                stackTarjan.emplace_back(make_pair(*stackTarjan.back().first.second,
                                                   outDegree[*stackTarjan.back().first.second].begin()), 0);
                continue;
            } else {
                if (inStack[*stackTarjan.back().first.second]) {
                    lowLevel[stackTarjan.back().first.first] = min(lowLevel[stackTarjan.back().first.first],
                                                                   lowLevel[*stackTarjan.back().first.second]);
                }
                stackTarjan.back().first.second++;
                stackTarjan.back().second = 0;
                continue;
            }
        } else {
            if (lowLevel[stackTarjan.back().first.first] == currLevel[stackTarjan.back().first.first]) {
                ++sccCounter;
                int currNode = -1;
                while (currNode != stackTarjan.back().first.first) {
                    currNode = sccStack.back();
                    sccStack.pop_back();
                    inStack[currNode] = false;
                    whichSCC[currNode] = sccCounter;
                    scc[sccCounter].emplace_back(currNode);
                }
            }
            stackTarjan.pop_back();
        }
    }
}


void reduceSCC() {
    sccIndex = 0;
    sccCounter = 0;
    scc.resize(n + 1, vector<int>());
    whichSCC.resize(n + 1, 0);
    lowLevel.resize(n + 1, 0);
    currLevel.resize(n + 1, 0);
    inStack.resize(n + 1, false);
    for (int i = 1; i <= n; ++i) {
        if (!currLevel[i]) {
            runTarjan(i);
        }
    }
    if (n != sccCounter) {
        cout << "-1\n";
        exit(0);
    }
    sccIndex = 0;
    sccCounter = 0;
    lowLevel.clear();
    currLevel.clear();
    inStack.clear();
    sccStack.clear();
}

signed main() {

    map<int, int> mp;
    inDegree.resize(1000000, set<int>());
    outDegree.resize(1000000, set<int>());
    for (int t = 129; t <= 129; t += 2) {
        bad.clear();
        cout << "test " << t << " started\n";
        string path_in =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\public-testcases\grader_test)" +
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
        cout << "..." << n << '\n';
        for (int i = 1; i <= n; ++i) {
            in >> x;
            bad.emplace_back(x);
        }
        in.close();
        in2 >> n >> m;
        for (int i = 1; i <= m; ++i) {
            in2 >> x >> y;
            if (used[x] || used[y]) continue;
            outDegree[x].insert(y);
            inDegree[y].insert(x);
        }
        for (auto it : bad) {
            for (auto it2 : inDegree[it]) {
                outDegree[it2].erase(it);
            }
            outDegree[it].clear();
        }

        reduceSCC();
        cout << "test " << (t + 1) / 2 << " finished  ";
        cout << ":)\n";
        sol.clear();
        for (int i = 0; i <= n; ++i) {
            inDegree[i].clear();
            outDegree[i].clear();
            ctc[i].clear();
            used[i] = false;
        }
        nrctc = 0;
        st.clear();
    }


    return 0;
}