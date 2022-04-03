#include <bits/stdc++.h>

using namespace std;
const int SECONDS = 300;

struct cmp {
    bool inline operator()(const pair<int, long long> &i, const pair<int, long long> &j) {
        return i.second < j.second || (i.second == j.second && i.first < j.first);
    }
};

priority_queue<pair<int, long long>, vector<pair<int, long long>>, cmp> availableNodes;
clock_t begin_;
int n, m, fitnessType, sccIndex, sccCounter;
int testNo;

vector<set<int>> inDegree, outDegree;
vector<set<int>> inDegreeReduced, outDegreeReduced;
set<pair<int, int> > edges;
set<pair<int, int> > edgesReduced;
set<int> candidatesNodes, feedbackVertexSet, bestFeedbackVertexSet;
set<int> feedbackVertexSetReduced;
vector<int> selfLoopNodes, zeroDegreeNodes, oneDegreeNodes;
vector<bool> availableNode, inStack;
vector<bool> availableNodeReduced;
set<int> feedbackVertexSetLocalSearch;
vector<int> feedbackVertexSetList;
vector<int> lowLevel, sccStack, currLevel, whichSCC;
vector<pair<pair<int, set<int>::iterator>, int> > stackTarjan;
set<int> cliqueCORE;
vector<bool> visited;
vector<int> candidatesSorted;
vector<int> posInCandidatesNodesCORE;

void checkTime() {
    clock_t end_ = clock();
    double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
    if (elapsed_secs >= SECONDS - 10) {
        for (auto it: feedbackVertexSetReduced) {
            bestFeedbackVertexSet.insert(it);
        }
        string path_output =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)" + to_string(testNo) + ".out";
        ofstream out(path_output);
        //cout << bestFeedbackVertexSet.size() << '\n';
        //out << bestFeedbackVertexSet.size() << '\n';
        for (auto node : bestFeedbackVertexSet) {
            cout << node << '\n';
        }
        out.close();
        //cout << "time elapsed in seconds: " << double(clock() - begin_) / CLOCKS_PER_SEC << '\n';
        exit(0);
    }
}

long long getFitness(int node) {
    if (fitnessType == 1) {
        return 1LL * inDegree[node].size() * outDegree[node].size();
    }
    if (fitnessType == 2) {
        return inDegree[node].size() + outDegree[node].size() -
               3 * abs(((int) inDegree[node].size()) - ((int) outDegree[node].size())) / 10;
    }
    if (fitnessType == 3) {
        return inDegree[node].size() + outDegree[node].size() -
               0 * abs(((int) inDegree[node].size()) - ((int) outDegree[node].size())) / 10;
    }
    if (fitnessType == 4) {
        return inDegree[node].size() + outDegree[node].size() -
               4 * abs(((int) inDegree[node].size()) - ((int) outDegree[node].size())) / 10;
    }
    if (fitnessType == 5) {
        return inDegree[node].size() + outDegree[node].size() -
               1 * abs(((int) inDegree[node].size()) - ((int) outDegree[node].size())) / 10;
    }
    if (fitnessType == 6) {
        return inDegree[node].size() + outDegree[node].size() -
               2 * abs(((int) inDegree[node].size()) - ((int) outDegree[node].size())) / 10;
    }
    return 0;
}

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
    if ((z == 1 || z == 3) && !checkNodeCanBeReduced(x)) {
        availableNodes.push(make_pair(x, getFitness(x)));
    }
    if ((z == 2 || z == 3) && !checkNodeCanBeReduced(y)) {
        availableNodes.push(make_pair(y, getFitness(y)));
    }
}

void addEdge(int x, int y) {
    edges.insert(make_pair(x, y));
    inDegree[y].insert(x);
    outDegree[x].insert(y);
}


void initializeSets() {
    whichSCC.resize(n + 1, 0);
    availableNode.resize(n + 1, true);
    inDegree.resize(n + 1, set<int>());
    outDegree.resize(n + 1, set<int>());
}

void clearSets() {
    posInCandidatesNodesCORE.clear();
    whichSCC.clear();
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

vector<pair<pair<int, int>, int >> toBeErasedNode;

void eraseNode(int node) {
    if (!availableNode[node]) {
        return;
    }
    availableNode[node] = false;
    candidatesNodes.erase(node);
    for (auto it : inDegree[node]) {
        toBeErasedNode.emplace_back(make_pair(it, node), 1);
    }
    for (auto it : outDegree[node]) {
        toBeErasedNode.emplace_back(make_pair(node, it), 2);
    }
    for (auto it : toBeErasedNode) {
        eraseEdge(it.first.first, it.first.second, it.second);
    }
    toBeErasedNode.clear();
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
vector<pair<pair<int, int>, int >> toBeErasedBypass;
vector<int> inNodes, outNodes;

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
    for (auto it :inDegree[node]) {
        toBeErasedBypass.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :outDegree[node]) {
        toBeErasedBypass.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it : toBeErasedBypass) {
        eraseEdge(it.first.first, it.first.second, it.second);
    }
    toBeErasedBypass.clear();
    for (auto it1 : inNodes) {
        for (auto it2 : outNodes) {
            if (edges.count(make_pair(it1, it2))) {
                checkNodeCanBeReduced(it1);
                availableNodes.push(make_pair(it1, getFitness(it1)));
                checkNodeCanBeReduced(it2);
                availableNodes.push(make_pair(it2, getFitness(it2)));
            }
            if (it1 == it2) {
                edges.insert(make_pair(it1, it2));
                selfLoopNodes.emplace_back(it1);
            }
            addEdge(it1, it2);
        }
    }

    if (inNodes.size() == 1) {
        availableNodes.push(
                make_pair(inNodes.back(), getFitness(inNodes.back())));
    }
    if (outNodes.size() == 1) {
        availableNodes.push(
                make_pair(outNodes.back(), getFitness(outNodes.back())));
    }
    inNodes.clear();
    outNodes.clear();
}

void runTarjan(int node) {
    stackTarjan.emplace_back(make_pair(node, outDegree[node].begin()), 0);
    for (; !stackTarjan.empty();) {
        if (stackTarjan.back().first.second == outDegree[stackTarjan.back().first.first].begin() &&
            !stackTarjan.back().second) {
            lowLevel[stackTarjan.back().first.first] = ++sccIndex;
            currLevel[stackTarjan.back().first.first] = sccIndex;
            inStack[stackTarjan.back().first.first] = true;
            sccStack.emplace_back(stackTarjan.back().first.first);
        }

        if (stackTarjan.back().first.second != outDegree[stackTarjan.back().first.first].end() &&
            edges.count(make_pair(*stackTarjan.back().first.second, stackTarjan.back().first.first))) {
            stackTarjan.back().first.second++;
            continue;
        }

        if (stackTarjan.back().second &&
            stackTarjan.back().first.second != outDegree[stackTarjan.back().first.first].end()) {
            lowLevel[stackTarjan.back().first.first] = min(lowLevel[stackTarjan.back().first.first],
                                                           lowLevel[*stackTarjan.back().first.second]);
            stackTarjan.back().first.second++;
            stackTarjan.back().second = 0;
            continue;
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
                }
            }
            stackTarjan.pop_back();
        }
    }
}


void reduceCORE() {
    if (posInCandidatesNodesCORE.empty()) {
        posInCandidatesNodesCORE.resize(n + 1, 0);
    }
    visited.resize(candidatesNodes.size(), false);
    int i = 0;
    for (auto it : candidatesNodes) {
        posInCandidatesNodesCORE[it] = i;
        ++i;
    }
    for (auto it : edges) {
        if (!edges.count(make_pair(it.second, it.first))) {
            visited[posInCandidatesNodesCORE[it.first]] = true;
            visited[posInCandidatesNodesCORE[it.second]] = true;
        }
    }
    candidatesSorted.reserve(candidatesNodes.size());
    for (auto node : candidatesNodes) {
        candidatesSorted.emplace_back(node);
    }
    sort(candidatesSorted.begin(), candidatesSorted.end(), [](const int i, const int j) {
        return inDegree[i].size() + outDegree[i].size() < inDegree[j].size() + outDegree[j].size();
    });
    for (auto node : candidatesSorted) {
        if (visited[posInCandidatesNodesCORE[node]]) continue;
        cliqueCORE.insert(node);
        for (auto it : outDegree[node]) {
            cliqueCORE.insert(it);
        }
        bool isCore = true;
        for (auto it : cliqueCORE) {
            int sz = 1;
            if (isCore) {
                for (auto it3 : outDegree[it]) {
                    if (it != it3 && cliqueCORE.count(it3)) {
                        ++sz;
                    }
                }
                if (sz != cliqueCORE.size()) {
                    isCore = false;
                }
            }
            visited[posInCandidatesNodesCORE[it]] = false;
        }
        if (isCore) {
            for (auto it : cliqueCORE) {
                eraseNode(it);
                if (it != node) {
                    feedbackVertexSet.insert(it);
                }
            }
        }
        cliqueCORE.clear();
    }
    visited.clear();
    candidatesSorted.clear();
}

vector<pair<int, int>> toBeErasedSCC;

void reduceSCC() {
    sccIndex = 0;
    sccCounter = 0;
    lowLevel.resize(n + 1, 0);
    currLevel.resize(n + 1, 0);
    inStack.resize(n + 1, false);
    for (auto it : candidatesNodes) {
        if (!currLevel[it]) {
            runTarjan(it);
        }
    }
    for (auto it : edges) {
        if (edges.count(make_pair(it.second, it.first))) continue;
        if (whichSCC[it.first] != whichSCC[it.second]) {
            toBeErasedSCC.emplace_back(it);
        }
    }
    for (auto it : toBeErasedSCC) {
        eraseEdge(it.first, it.second, 3);
    }
    toBeErasedSCC.clear();
    sccIndex = 0;
    sccCounter = 0;
    lowLevel.clear();
    currLevel.clear();
    inStack.clear();
    sccStack.clear();
}

void doBasicReductions() {
    checkTime();
    bool change = true;
    while (change) {
        change = false;
        change |= !selfLoopNodes.empty();
        while (!selfLoopNodes.empty()) {
            int node = selfLoopNodes.back();
            selfLoopNodes.pop_back();
            if (!edges.count(make_pair(node, node))) {
                continue;
            }
            eraseNode(node);
            feedbackVertexSet.insert(node);
        }
        change |= !zeroDegreeNodes.empty();
        while (!zeroDegreeNodes.empty()) {
            int node = zeroDegreeNodes.back();
            zeroDegreeNodes.pop_back();
            if (!inDegree[node].empty() && !outDegree[node].empty()) {
                continue;
            }
            eraseNode(node);
        }
        change |= !oneDegreeNodes.empty();
        while (!oneDegreeNodes.empty()) {
            int node = oneDegreeNodes.back();
            oneDegreeNodes.pop_back();
            bypassNode(node);
        }
    }
}


void findDFVS() {
    doBasicReductions();
    int edgesCount = edges.size();
    while (!availableNodes.empty()) {
        pair<int, long long> topNode = availableNodes.top();
        availableNodes.pop();
        if (!availableNode[topNode.first] ||
            topNode.second != getFitness(topNode.first)) {
            continue;
        }
        eraseNode(topNode.first);
        doBasicReductions();
        if (bestFeedbackVertexSet.size() == n && edges.size() < edgesCount * 3 / 4) {
            edgesCount = edges.size();
            reduceSCC();
            doBasicReductions();
            reduceCORE();
            doBasicReductions();
        }
        feedbackVertexSet.insert(topNode.first);
    }
    if (bestFeedbackVertexSet.size() > feedbackVertexSet.size()) {
        bestFeedbackVertexSet = feedbackVertexSet;
    }
}

string s;

void readData() {
    string path_input = R"(C:\Users\andre\OneDrive\Desktop\pace\heuristic_public\h_)";
    for (int l = 1; l <= 3 - to_string(testNo).size(); ++l) {
        path_input.push_back('0');
    }
    path_input += to_string(testNo);
    ifstream in(path_input);
    int t;
    while (getline(cin, s)) {
        deque<char> input;
        for (auto it : s) {
            input.push_back(it);
        }
        while (!input.empty() && input.front() == ' ') {
            input.pop_front();
        }
        s.clear();
        for (char i : input) {
            s.push_back(i);
        }
        if (s.empty() || s[0] == '%') continue;
        istringstream is(s);
        is >> n >> m >> t;
        break;
    }
    initializeSets();
    for (int j = 1; j <= n; ++j) {
        getline(cin, s);
        deque<char> input;
        for (auto it : s) {
            input.push_back(it);
        }
        while (!input.empty() && input.front() == ' ') {
            input.pop_front();
        }
        s.clear();
        for (char i : input) {
            s.push_back(i);
        }
        if (!s.empty() && s[0] == '%') {
            j--;
            continue;
        }
        istringstream is(s);
        while (is >> t) {
            addEdge(j, t);
        }
    }
    for (int i = 1; i <= n; ++i) {
        bestFeedbackVertexSet.insert(i);
    }
    in.close();
    if (n == 0) {
        // empty testcase
        exit(0);
    }
}


void createInitialDFVS() {
    inDegree = inDegreeReduced;
    outDegree = outDegreeReduced;
    edges = edgesReduced;
    availableNode = availableNodeReduced;
    for (int i = 1; i <= n; ++i) {
        if (availableNode[i]) {
            candidatesNodes.insert(i);
            if (!checkNodeCanBeReduced(i)) {
                availableNodes.push(make_pair(i, getFitness(i)));
            }
        }
    }
    findDFVS();
    feedbackVertexSetLocalSearch = feedbackVertexSet;
    clearSets();
    initializeSets();
}

void doLocalSearch() {
    inDegree = inDegreeReduced;
    outDegree = outDegreeReduced;
    edges = edgesReduced;
    availableNode = availableNodeReduced;
    feedbackVertexSetList.clear();
    for (auto node : feedbackVertexSetLocalSearch) {
        feedbackVertexSetList.emplace_back(node);
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(feedbackVertexSetList.begin(), feedbackVertexSetList.end(), std::default_random_engine(seed));
    int size = feedbackVertexSetList.size();
    int toBeErasedCounter = size / 2 + rand() % (size / 2 - size / 3 + 1);
    while (toBeErasedCounter--) {
        int node = feedbackVertexSetList.back();
        feedbackVertexSetList.pop_back();
        eraseNode(node);
        doBasicReductions();
        feedbackVertexSet.insert(node);
    }
    for (int i = 1; i <= n; ++i) {
        if (availableNode[i]) {
            candidatesNodes.insert(i);
            if (!checkNodeCanBeReduced(i)) {
                availableNodes.push(make_pair(i, getFitness(i)));
            }
        }
    }
    checkTime();
    findDFVS();
    if (feedbackVertexSet.size() < feedbackVertexSetLocalSearch.size()) {
        feedbackVertexSetLocalSearch = feedbackVertexSet;
    }
    clearSets();
    initializeSets();
}

void solveTestcase() {
    begin_ = clock();
    srand(0);
    readData();
    fitnessType = 1;
    for (int i = 1; i <= n; ++i) {
        checkNodeCanBeReduced(i);
    }
    for (int i = 1; i <= n; ++i) {
        candidatesNodes.insert(i);
    }
    for (int i = 1; i <= 5; ++i) {
        int last = edges.size();
        reduceSCC();
        doBasicReductions();
        if (last - edges.size() < 1000) {
            break;
        }
    }
    reduceCORE();
    candidatesNodes.clear();
    inDegreeReduced = inDegree;
    outDegreeReduced = outDegree;
    feedbackVertexSetReduced = feedbackVertexSet;
    edgesReduced = edges;
    availableNodeReduced = availableNode;
    feedbackVertexSet.clear();
    for (; fitnessType <= 1; ++fitnessType) {
        createInitialDFVS();
        int lastActive = 1;
        for (int i = 1;; ++i) {
            int bestSize = bestFeedbackVertexSet.size();
            doLocalSearch();
            int currSize = bestFeedbackVertexSet.size();
            if (bestSize != currSize) {
                lastActive = i;
            }
            checkTime();
        }
    }
    for (auto it: feedbackVertexSetReduced) {
        bestFeedbackVertexSet.insert(it);
    }
    string path_output =
            R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)" + to_string(testNo) + ".out";
    ofstream out(path_output);
    //cout << bestFeedbackVertexSet.size() << '\n';
    //out << bestFeedbackVertexSet.size() << '\n';
    for (auto it: bestFeedbackVertexSet) {
        cout << it << '\n';
    }
    out.close();
}


signed main() {
    for (testNo = 115; testNo <= 115; testNo += 2) {
        //cout << testNo << ' ';
        solveTestcase();
    }
    return 0;
}