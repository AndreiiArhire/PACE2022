#include <bits/stdc++.h>

using namespace std;

struct cmp {
    bool inline operator()(const pair<int, long long> &i, const pair<int, long long> &j) {
        return i.second < j.second;
    }
};

priority_queue<pair<int, long long>, vector<pair<int, long long>>, cmp> availableNodes;

clock_t begin_;
int n, m;
vector<set<int>> inDegree, outDegree;
set<pair<int, int> > edges;
set<int> candidatesNodes, feedbackVertexSet;
vector<int> selfLoopNodes, zeroDegreeNodes, oneDegreeNodes;
vector<bool> availableNode;


bool checkNodeCanBeReduced(int node) {
    if (!availableNode[node]) {
        return false;
    }
    if (inDegree[node].empty() || outDegree[node].empty()) {
        zeroDegreeNodes.emplace_back(node);
        return true;
    }
    if (edges.count(make_pair(node, node))) {
        selfLoopNodes.emplace_back(node);
        return true;
    }

    if (inDegree[node].size() == 1 || outDegree[node].size() == 1) {
        oneDegreeNodes.emplace_back(node);
        return true;
    }
    return false;
}

void eraseEdge(int x, int y, int z) {
    edges.erase(make_pair(x, y));
    outDegree[x].erase(y);
    inDegree[y].erase(x);
    // checkNodeCanBeReduced(x);
    // checkNodeCanBeReduced(y);
    if ((z == 1 || z == 3) && !checkNodeCanBeReduced(x)) {
        availableNodes.push(make_pair(x, 1LL * inDegree[x].size() * outDegree[x].size()));
    }
    if ((z == 2 || z == 3) && !checkNodeCanBeReduced(y)) {
        availableNodes.push(make_pair(y, 1LL * inDegree[y].size() * outDegree[y].size()));
    }
}

void addEdge(int x, int y) {
    edges.insert(make_pair(x, y));
    inDegree[y].insert(x);
    outDegree[x].insert(y);
}


void initializeSets() {
    availableNode.resize(n + 1, true);
    inDegree.resize(n + 1, set<int>());
    outDegree.resize(n + 1, set<int>());
}

void clearSets() {
    feedbackVertexSet.clear();
    availableNode.clear();
    selfLoopNodes.clear();
    zeroDegreeNodes.clear();
    oneDegreeNodes.clear();
    while (!availableNodes.empty()) {
        availableNodes.pop();
    }
    candidatesNodes.clear();
    edges.clear();
    inDegree.clear();
    outDegree.clear();
}

void eraseNode(int node) {
    if (!availableNode[node]) {
        return;
    }
    availableNode[node] = false;
    candidatesNodes.erase(node);
    vector<pair<pair<int, int>, int >> toBeErased;
    for (auto it : inDegree[node]) {
        toBeErased.emplace_back(make_pair(it, node), 1);
    }
    for (auto it : outDegree[node]) {
        toBeErased.emplace_back(make_pair(node, it), 2);
    }
    for (auto it : toBeErased) {
        eraseEdge(it.first.first, it.first.second, it.second);
    }
}

/*
 * [] - [] - []
 *      \  - []
 */

/*
 *  [1] -  \
 *   |     |
 *  [2] - [3]
 */
void bypassNode(int node) {
    if (!availableNode[node]) {
        return;
    }
    if (edges.count(make_pair(node, node))) {
        selfLoopNodes.emplace_back(node);
        return;
    }
    if (inDegree[node].empty() || outDegree[node].empty()) {
        zeroDegreeNodes.emplace_back(node);
        return;
    }
    if (inDegree[node].size() != 1 && outDegree[node].size() != 1) {
        return;
    }
    availableNode[node] = false;
    candidatesNodes.erase(node);
    vector<int> inNodes, outNodes;
    vector<pair<pair<int, int>, int >> toBeErased;
    for (auto it :inDegree[node]) {
        toBeErased.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :outDegree[node]) {
        toBeErased.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it : toBeErased) {
        eraseEdge(it.first.first, it.first.second, it.second);
    }
    for (auto it1 : inNodes) {
        for (auto it2 : outNodes) {
            addEdge(it1, it2);
        }
    }
    for (auto it : inNodes) {
        checkNodeCanBeReduced(it);
    }
    for (auto it : outNodes) {
        checkNodeCanBeReduced(it);
    }
}

void findDFVS() {
    bool sw = true;
    while (sw) {
        sw = false;
        sw |= !selfLoopNodes.empty();
        while (!selfLoopNodes.empty()) {
            int node = selfLoopNodes.back();
            selfLoopNodes.pop_back();
            if (!edges.count(make_pair(node, node))) {
                continue;
            }
            eraseNode(node);
            feedbackVertexSet.insert(node);
        }
        sw |= !zeroDegreeNodes.empty();
        while (!zeroDegreeNodes.empty()) {
            int node = zeroDegreeNodes.back();
            zeroDegreeNodes.pop_back();
            if (!inDegree[node].empty() && !outDegree[node].empty()) {
                continue;
            }
            eraseNode(node);
        }
        sw |= !oneDegreeNodes.empty();
        while (!oneDegreeNodes.empty()) {
            int node = oneDegreeNodes.back();
            oneDegreeNodes.pop_back();
            bypassNode(node);
        }
    }
    for (auto it : candidatesNodes) {
        if (inDegree[it].size() == 1 || outDegree[it].size() == 1) {
            cout << "????\n";
            exit(0);
        }
    }
    cout << candidatesNodes.size() << ' ' << edges.size() << '\n';
}

void solveTestcase(const string &pathInput, const string &pathOutput) {
    ifstream in(pathInput);
    ofstream out(pathOutput);
    int x, y;
    in >> n >> m;
    cout << n << ' ' << m << '\n';
    if (n == 0) {
        // empty testcase
        return;
    }
    initializeSets();
    for (int i = 1; i <= m; ++i) {
        in >> x >> y;
        addEdge(x, y);
    }
    for (int i = 1; i <= n; ++i) {
        candidatesNodes.insert(i);
        if (!checkNodeCanBeReduced(i)) {
            availableNodes.push(make_pair(i, 1LL * inDegree[i].size() * outDegree[i].size()));
        }
    }
    findDFVS();
    clearSets();
    in.close();
    out.close();
}


signed main() {
    srand(0);
    vector<int> tests;
    tests.emplace_back(1);
    string pathInput = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\public-testcases\grader_test)";
    string pathOutput = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)";
    for (auto i : tests) {
        int currTestcase = i;
        begin_ = clock();
        cout << "test " << currTestcase << " began\n";
        solveTestcase(pathInput + to_string(currTestcase) + ".in", pathOutput + to_string(currTestcase) + ".out");
        cout << "test " << currTestcase << " finished\n";
        clock_t end_ = clock();
        double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
        cout << "time elapsed in seconds: " << elapsed_secs << '\n';
    }
    return 0;
}
