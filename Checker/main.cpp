#include <bits/stdc++.h>

using namespace std;

vector<int> sol;
vector<int> v_t[1000001], ctc[1000001], st, v[1000001];
int used[1000001], nrctc, n, m;
vector<bool> bad(1000000, true);
set<pair<int, int> > edges;
set<pair<int, int> > edges2;
vector<int> sortTop;
vector<int> candidates_nodes;
vector<int> posInCandidatesNodes;

void dfs(int nod) {
    used[nod] = 1;
    for (int vecin : v[nod]) {
        if (!used[vecin] && !bad[vecin]) dfs(vecin);
    }
    st.emplace_back(nod);
    sortTop.emplace_back(nod);
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
            //  exit(0);
        }
    }
}

struct cmp {
    bool inline operator()(const pair<int, int> i, const pair<int, int> j) {
        return i.second < j.second;
    }
};

priority_queue<pair<int, int>, vector<pair<int, int>>, cmp> pq;
set<int> cSet;

int getMaleFitness(vector<int> offspring) {
    /*return minimum number of arcs to be deleted to obtain DAG*/
    /*creates the subgraph with nodes in subset*/
    vector<set<int>> in_degree_local(n + 1);
    vector<set<int>> out_degree_local(n + 1);
    vector<int> sources, sinks;
    for (auto it : edges2) {
        if (cSet.count(it.first) && cSet.count(it.second)) {
            out_degree_local[it.first].insert(it.second);
            in_degree_local[it.second].insert(it.first);
        }
    }
    /* greedy heuristic to find Feedback Arc Set */
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        if (offspring[i]) {
            pq.push(make_pair(candidates_nodes[i], ((int) out_degree_local[candidates_nodes[i]].size()) -
                                                   ((int) in_degree_local[candidates_nodes[i]].size())));
            if (out_degree_local[candidates_nodes[i]].empty()) {
                sinks.emplace_back(candidates_nodes[i]);
            }
            if (in_degree_local[candidates_nodes[i]].empty()) {
                sources.emplace_back(candidates_nodes[i]);
            }
        }
    }
    vector<bool> alreadyChecked(n + 1, false);
    vector<int> posInTopSort(n + 1, 0), topSort;
    deque<int> leftPart, rightPart;
    int cnt2 = 0;
    while (!pq.empty()) {
        /* remove all sinks */
        while (!sinks.empty()) {
            auto node = sinks.back();
            sinks.pop_back();
            if (alreadyChecked[node]) {
                continue;
            }
            ++cnt2;
            alreadyChecked[node] = true;
            rightPart.push_front(node);
            for (auto it : in_degree_local[node]) {
                out_degree_local[it].erase(node);
                if (out_degree_local[it].empty()) {
                    sinks.emplace_back(it);
                }
                pq.push(make_pair(it, ((int) out_degree_local[it].size()) - ((int) in_degree_local[it].size())));
            }
        }

        /* remove all sources *
        while (!sources.empty()) {
            auto node = sources.back();
            sources.pop_back();
            if (alreadyChecked[node]) {
                continue;
            }
            ++cnt2;
            alreadyChecked[node] = true;
            leftPart.push_back(node);
            for (auto it : out_degree_local[node]) {
                in_degree_local[it].erase(node);
                if (in_degree_local[it].empty()) {
                    sources.emplace_back(it);
                }
                pq.push(make_pair(it, ((int) out_degree_local[it].size()) - ((int) in_degree_local[it].size())));
            }
        }
        */
        auto node = pq.top();
        pq.pop();
        /*
        if (alreadyChecked[node.first] ||
            node.second !=
            ((int) out_degree_local[node.first].size()) - ((int) in_degree_local[node.first].size())) {
            continue;
        }
        alreadyChecked[node.first] = true;
        leftPart.emplace_back(node.first);
        for (auto it : out_degree_local[node.first]) {
            in_degree_local[it].erase(node.first);
            if (in_degree_local[it].empty()) {
                sources.emplace_back(it);
            }
            pq.push(make_pair(it, ((int) out_degree_local[it].size()) - ((int) in_degree_local[it].size())));
        }
        for (auto it : in_degree_local[node.first]) {
            out_degree_local[it].erase(node.first);
            if (out_degree_local[it].empty()) {
                sinks.emplace_back(it);
            }
            pq.push(make_pair(it, ((int) out_degree_local[it].size()) - ((int) in_degree_local[it].size())));
        }
         */
    }
   // cout << cnt2 << '\n';
    while (!leftPart.empty()) {
        topSort.emplace_back(leftPart.front());
        posInTopSort[leftPart.front()] = topSort.size() - 1;
        leftPart.pop_front();
    }
    while (!rightPart.empty()) {
        topSort.emplace_back(rightPart.front());
        posInTopSort[rightPart.front()] = topSort.size() - 1;
        rightPart.pop_front();
    }
    int missMatch = 0;
    for (auto it : edges2) {
        if (cSet.count(it.first) && cSet.count(it.second) &&
            posInTopSort[it.first] >= posInTopSort[it.second]) {
            ++missMatch;
        }
    }
    return missMatch;
}

signed main() {

    for (int t = 49; t >= 49; --t) {
        bad.clear();
        bad.resize(1000005, false);
        cout << "test " << t << " started\n";
        string path_in =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\public-testcases\grader_test)" +
                to_string(t) +
                ".in";
        string path_out =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\GA-results2\grader_test)" +
                to_string(t) +
                ".out";
        ifstream in2(path_in);
        ifstream in(path_out);
        int x, y;

        in >> n;
      //  cout << n << '\n';
        for (int i = 1; i <= n; ++i) {
            in >> x;
            candidates_nodes.emplace_back(x);
            cSet.insert(x);
            bad[x] = true;
        }
        in.close();
        in2 >> n >> m;

        for (int i = 1; i <= m; ++i) {
            in2 >> x >> y;
            if (bad[x] && bad[y]) {
                edges2.insert(make_pair(x, y));
            }
            if (bad[x] || bad[y]) continue;
            edges.insert(make_pair(x, y));
            v[x].push_back(y);
            v_t[y].push_back(x);
        }
        posInCandidatesNodes.resize(n + 1, 0);
        for (int i = 0; i < candidates_nodes.size(); ++i) {
            posInCandidatesNodes[candidates_nodes[i]] = i;
        }
        /*
        cout << candidates_nodes.size() << '\n';
        vector<int> offspring(candidates_nodes.size() + 1, 1);
        cout << "ok\n";
        cout << getMaleFitness(offspring) << '\n';
       */
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
        vector<int> pos(n + 1, 0);
        for (int i = (int) sortTop.size() - 1; i >= 0; --i) {
            pos[sortTop[i]] = sortTop.size() - 1 - i;
        }
        int cnt = 0;
        for (auto it : edges) {
            if (pos[it.first] >= pos[it.second]) {
                ++cnt;
            }
        }
       // cout << cnt << '\n';
        st.clear();
        edges.clear();
    }

    return 0;
}