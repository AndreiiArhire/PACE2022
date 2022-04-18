#include <bits/stdc++.h>
#include <chrono>

using namespace std;
const int SECONDS = 600;

std::chrono::time_point<std::chrono::high_resolution_clock> begin_;
int n, m, fitnessType, sccIndex, sccCounter, testNo, currentErased, firstTime;
string output, s;
vector<pair<pair<int, int>, int >> toBeErasedBypass;
vector<pair<pair<int, set<int>::iterator>, int> > stackTarjan;
vector<pair<int, int>> toBeErasedDOME, toBeErasedSCC;
set<pair<int, int>> edges, edgesReduced;
vector<set<int>> inDegreeReducedSimple, inDegreeDouble, outDegreeDouble, inDegreeSimple, outDegreeSimple, inDegreeReducedDouble, outDegreeReducedDouble, outDegreeReducedSimple;
vector<bool> availableNode, inStack, availableNodeReduced, visitedCORE;
vector<int> inNodes, outNodes, localDFVS, local, selfLoopNodes, zeroDegreeNodes, oneDegreeNodes, lowLevel, sccStack, currLevel, whichSCC, candidatesSorted;
set<int> candidatesNodesReduced, localSet, cliqueCORE, candidatesNodes, feedbackVertexSet, bestFeedbackVertexSet, lastFeedbackVertexSet, feedbackVertexSetReduced;
vector<long long> lastFitness;
auto cmp = [](pair<long long, int> a, pair<long long, int> b) {
    return a.first > b.first || (a.first == b.first && a.second > b.second);
};
set<pair<long long, int>, decltype(cmp)> availableNodes(cmp);

void doBasicReductions();

void loop();

void findDFVS();

void readData();

void createInitialDFVS();

void improveFeedbackVertexSet();

void doLocalSearch();

void solveTestcase();

void checkTime();

void eraseEdge(int x, int y, int z);

void addEdge(int x, int y);

void initializeSets();

void clearSets();

void eraseNode(int node);

void bypassNode(int node);

void runTarjan(int node);

bool checkNodeCanBeReduced(int node);

bool checkTarjan(int node);

bool reduceCORE();

bool reduceDOME();

bool reduceSCC();

int countSCC(int node);

long long getFitness(int node);

double getElapsed();

signed main() {
    for (testNo = 133; testNo <= 133; testNo += 2) {
        cout << (testNo + 1) / 2 << '\n';
        solveTestcase();
    }
    return 0;
}

bool checkNodeCanBeReduced(int node) {
    checkTime();
    if (!availableNode[node]) {
        return false;
    }
    if ((inDegreeSimple[node].empty() && inDegreeDouble[node].empty()) ||
        (outDegreeSimple[node].empty() && outDegreeDouble[node].empty())) {
        zeroDegreeNodes.emplace_back(node);
        return true;
    }
    if (edges.count(make_pair(node, node))) {
        selfLoopNodes.emplace_back(node);
        return true;
    }

    if (inDegreeSimple[node].size() + inDegreeDouble[node].size() == 1 ||
        outDegreeSimple[node].size() + outDegreeDouble[node].size() == 1) {
        oneDegreeNodes.emplace_back(node);
        return true;
    }
    return false;
}

void eraseEdge(int x, int y, int z) {
    checkTime();
    edges.erase(make_pair(x, y));
    outDegreeSimple[x].erase(y);
    outDegreeDouble[x].erase(y);
    inDegreeSimple[y].erase(x);
    inDegreeDouble[y].erase(x);
    if (edges.count(make_pair(y, x))) {
        edges.erase(make_pair(y, x));
        inDegreeDouble[x].erase(y);
        inDegreeSimple[x].erase(y);
        outDegreeDouble[y].erase(x);
        outDegreeSimple[y].erase(x);
    }
    if ((z == 1 || z == 3) && !checkNodeCanBeReduced(x)) {
        if (lastFitness[x] != -1) {
            availableNodes.erase(make_pair(lastFitness[x], x));
        }
        availableNodes.insert(make_pair((lastFitness[x] = getFitness(x)), x));
    }
    if ((z == 2 || z == 3) && !checkNodeCanBeReduced(y)) {
        if (lastFitness[y] != -1) {
            availableNodes.erase(make_pair(lastFitness[y], y));
        }
        availableNodes.insert(make_pair((lastFitness[y] = getFitness(y)), y));
    }
}

void addEdge(int x, int y) {
    checkTime();
    if (!edges.count(make_pair(y, x)) || x == y) {
        edges.insert(make_pair(x, y));
        inDegreeSimple[y].insert(x);
        outDegreeSimple[x].insert(y);
    } else {
        edges.insert(make_pair(x, y));
        inDegreeSimple[x].erase(y);
        outDegreeSimple[y].erase(x);
        inDegreeDouble[x].insert(y);
        inDegreeDouble[y].insert(x);
        outDegreeDouble[x].insert(y);
        outDegreeDouble[y].insert(x);
    }
}

void initializeSets() {
    checkTime();
    lastFitness.resize(n + 1, -1);
    visitedCORE.resize(n + 1, false);
    lowLevel.resize(n + 1, 0);
    currLevel.resize(n + 1, 0);
    inStack.resize(n + 1, false);
    whichSCC.resize(n + 1, 0);
    availableNode.resize(n + 1, true);
    inDegreeSimple.resize(n + 1, set<int>());
    inDegreeDouble.resize(n + 1, set<int>());
    outDegreeSimple.resize(n + 1, set<int>());
    outDegreeDouble.resize(n + 1, set<int>());
}

void clearSets() {
    checkTime();
    lastFitness.clear();
    visitedCORE.clear();
    whichSCC.clear();
    feedbackVertexSet.clear();
    availableNode.clear();
    selfLoopNodes.clear();
    zeroDegreeNodes.clear();
    oneDegreeNodes.clear();
    availableNodes.clear();
    candidatesNodes.clear();
    edges.clear();
    inDegreeSimple.clear();
    inDegreeDouble.clear();
    outDegreeSimple.clear();
    outDegreeDouble.clear();
}

void eraseNode(int node) {
    checkTime();
    if (!availableNode[node]) {
        return;
    }
    availableNode[node] = false;
    candidatesNodes.erase(node);
    availableNodes.erase(make_pair(lastFitness[node], node));
    while (!inDegreeSimple[node].empty()) {
        checkTime();
        int x = *inDegreeSimple[node].begin();
        inDegreeSimple[node].erase(inDegreeSimple[node].begin());
        outDegreeSimple[x].erase(node);
        edges.erase(make_pair(x, node));
        if (node != x && !checkNodeCanBeReduced(x)) {
            if (lastFitness[x] != -1) {
                availableNodes.erase(make_pair(lastFitness[x], x));
            }
            availableNodes.insert(make_pair((lastFitness[x] = getFitness(x)), x));
        }
    }
    while (!inDegreeDouble[node].empty()) {
        checkTime();
        int x = *inDegreeDouble[node].begin();
        inDegreeDouble[node].erase(inDegreeDouble[node].begin());
        outDegreeDouble[x].erase(node);
        edges.erase(make_pair(x, node));
        if (node != x && !checkNodeCanBeReduced(x)) {
            if (lastFitness[x] != -1) {
                availableNodes.erase(make_pair(lastFitness[x], x));
            }
            availableNodes.insert(make_pair((lastFitness[x] = getFitness(x)), x));
        }
    }
    while (!outDegreeSimple[node].empty()) {
        checkTime();
        int x = *outDegreeSimple[node].begin();
        outDegreeSimple[node].erase(outDegreeSimple[node].begin());
        inDegreeSimple[x].erase(node);
        edges.erase(make_pair(node, x));
        if (node != x && !checkNodeCanBeReduced(x)) {
            if (lastFitness[x] != -1) {
                availableNodes.erase(make_pair(lastFitness[x], x));
            }
            availableNodes.insert(make_pair((lastFitness[x] = getFitness(x)), x));
        }
    }
    while (!outDegreeDouble[node].empty()) {
        checkTime();
        int x = *outDegreeDouble[node].begin();
        outDegreeDouble[node].erase(outDegreeDouble[node].begin());
        inDegreeDouble[x].erase(node);
        edges.erase(make_pair(node, x));
        if (node != x && !checkNodeCanBeReduced(x)) {
            if (lastFitness[x] != -1) {
                availableNodes.erase(make_pair(lastFitness[x], x));
            }
            availableNodes.insert(make_pair((lastFitness[x] = getFitness(x)), x));
        }
    }
}

void bypassNode(int node) {
    checkTime();
    if (!availableNode[node]) {
        return;
    }
    if (edges.count(make_pair(node, node))) {
        selfLoopNodes.emplace_back(node);
        return;
    }
    if ((inDegreeSimple[node].empty() && inDegreeDouble[node].empty()) ||
        (outDegreeSimple[node].empty() && outDegreeDouble[node].empty())) {
        zeroDegreeNodes.emplace_back(node);
        return;
    }
    if (inDegreeSimple[node].size() + inDegreeDouble[node].size() != 1 &&
        outDegreeSimple[node].size() + outDegreeDouble[node].size() != 1) {
        return;
    }
    availableNode[node] = false;
    candidatesNodes.erase(node);
    for (auto it :inDegreeSimple[node]) {
        checkTime();
        toBeErasedBypass.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :inDegreeDouble[node]) {
        checkTime();
        toBeErasedBypass.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :outDegreeSimple[node]) {
        checkTime();
        toBeErasedBypass.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it :outDegreeDouble[node]) {
        checkTime();
        toBeErasedBypass.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it : toBeErasedBypass) {
        checkTime();
        eraseEdge(it.first.first, it.first.second, it.second);
    }
    toBeErasedBypass.clear();
    for (auto it1 : inNodes) {
        checkTime();
        for (auto it2 : outNodes) {
            checkTime();
            addEdge(it1, it2);
            edges.insert(make_pair(it1, it2));
            if (it1 == it2) {
                selfLoopNodes.emplace_back(it1);
            }
            checkNodeCanBeReduced(it2);
            if (lastFitness[it2] != -1) {
                availableNodes.erase(make_pair(lastFitness[it2], it2));
            }
            availableNodes.insert(make_pair((lastFitness[it2] = getFitness(it2)), it2));
        }
        checkNodeCanBeReduced(it1);
        if (lastFitness[it1] != -1) {
            availableNodes.erase(make_pair(lastFitness[it1], it1));
        }
        availableNodes.insert(make_pair((lastFitness[it1] = getFitness(it1)), it1));
    }
    inNodes.clear();
    outNodes.clear();
}

void runTarjan(int node) {
    checkTime();
    stackTarjan.emplace_back(make_pair(node, outDegreeSimple[node].begin()), 0);
    for (; !stackTarjan.empty();) {
        checkTime();
        if (stackTarjan.back().first.second == outDegreeSimple[stackTarjan.back().first.first].begin() &&
            !stackTarjan.back().second) {
            lowLevel[stackTarjan.back().first.first] = ++sccIndex;
            currLevel[stackTarjan.back().first.first] = sccIndex;
            inStack[stackTarjan.back().first.first] = true;
            sccStack.emplace_back(stackTarjan.back().first.first);
        }

        if (stackTarjan.back().first.second != outDegreeSimple[stackTarjan.back().first.first].end() &&
            edges.count(make_pair(*stackTarjan.back().first.second, stackTarjan.back().first.first))) {
            stackTarjan.back().first.second++;
            continue;
        }

        if (stackTarjan.back().second &&
            stackTarjan.back().first.second != outDegreeSimple[stackTarjan.back().first.first].end()) {
            lowLevel[stackTarjan.back().first.first] = min(
                    lowLevel[stackTarjan.back().first.first],
                    lowLevel[*stackTarjan.back().first.second]);
            stackTarjan.back().first.second++;
            stackTarjan.back().second = 0;
            continue;
        }

        if (stackTarjan.back().first.second != outDegreeSimple[stackTarjan.back().first.first].end()) {
            if (!currLevel[*stackTarjan.back().first.second]) {
                stackTarjan.back().second = 1;
                stackTarjan.emplace_back(make_pair(*stackTarjan.back().first.second,
                                                   outDegreeSimple[*stackTarjan.back().first.second].begin()), 0);
                continue;
            } else {
                if (inStack[*stackTarjan.back().first.second]) {
                    lowLevel[stackTarjan.back().first.first] = min(
                            lowLevel[stackTarjan.back().first.first],
                            lowLevel[*stackTarjan.back().first.second]);
                }
                stackTarjan.back().first.second++;
                stackTarjan.back().second = 0;
                continue;
            }
        } else {
            if (lowLevel[stackTarjan.back().first.first] ==
                currLevel[stackTarjan.back().first.first]) {
                ++sccCounter;
                int currNode = -1;
                while (currNode != stackTarjan.back().first.first) {
                    checkTime();
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

bool checkTarjan(int node) {
    checkTime();
    stackTarjan.emplace_back(make_pair(node, outDegreeSimple[node].begin()), 0);
    for (; !stackTarjan.empty();) {
        checkTime();
        if (stackTarjan.back().first.second == outDegreeSimple[stackTarjan.back().first.first].begin() &&
            !stackTarjan.back().second) {
            lowLevel[stackTarjan.back().first.first] = ++sccIndex;
            currLevel[stackTarjan.back().first.first] = sccIndex;
            inStack[stackTarjan.back().first.first] = true;
            sccStack.emplace_back(stackTarjan.back().first.first);
        }
        if (stackTarjan.back().first.second != outDegreeSimple[stackTarjan.back().first.first].end() &&
            edges.count(make_pair(*stackTarjan.back().first.second, stackTarjan.back().first.first))) {
            stackTarjan.back().first.second++;
            continue;
        }

        if (stackTarjan.back().second &&
            stackTarjan.back().first.second != outDegreeSimple[stackTarjan.back().first.first].end()) {
            lowLevel[stackTarjan.back().first.first] = min(
                    lowLevel[stackTarjan.back().first.first],
                    lowLevel[*stackTarjan.back().first.second]);
            if (lowLevel[stackTarjan.back().first.first] != currLevel[stackTarjan.back().first.first]) {
                sccStack.clear();
                stackTarjan.clear();
                return false;
            }
            stackTarjan.back().first.second++;
            stackTarjan.back().second = 0;
            continue;
        }

        if (stackTarjan.back().first.second != outDegreeSimple[stackTarjan.back().first.first].end()) {
            if (!currLevel[*stackTarjan.back().first.second]) {
                stackTarjan.back().second = 1;
                stackTarjan.emplace_back(make_pair(*stackTarjan.back().first.second,
                                                   outDegreeSimple[*stackTarjan.back().first.second].begin()), 0);
                continue;
            } else {
                if (inStack[*stackTarjan.back().first.second]) {
                    lowLevel[stackTarjan.back().first.first] = min(
                            lowLevel[stackTarjan.back().first.first],
                            lowLevel[*stackTarjan.back().first.second]);
                }
                if (lowLevel[stackTarjan.back().first.first] != currLevel[stackTarjan.back().first.first]) {
                    sccStack.clear();
                    stackTarjan.clear();
                    return false;
                }
                stackTarjan.back().first.second++;
                stackTarjan.back().second = 0;
                continue;
            }
        } else {
            if (lowLevel[stackTarjan.back().first.first] ==
                currLevel[stackTarjan.back().first.first]) {
                ++sccCounter;
                int currNode = -1;
                while (currNode != stackTarjan.back().first.first) {
                    checkTime();
                    currNode = sccStack.back();
                    sccStack.pop_back();
                    inStack[currNode] = false;
                    whichSCC[currNode] = sccCounter;
                }
            }
            stackTarjan.pop_back();
        }
    }
    return true;
}

bool reduceCORE() {
    checkTime();
    for (auto it : candidatesNodes) {
        checkTime();
        visitedCORE[it] = false;
    }
    checkTime();
    candidatesSorted.reserve(candidatesNodes.size());
    for (auto node : candidatesNodes) {
        checkTime();
        candidatesSorted.emplace_back(node);
    }
    checkTime();
    sort(candidatesSorted.begin(), candidatesSorted.end(), [](const int i, const int j) {
        return outDegreeDouble[i].size() < outDegreeDouble[j].size() ||
               (outDegreeDouble[i].size() == outDegreeDouble[j].size() &&
                1LL * inDegreeSimple[i].size() * outDegreeSimple[i].size() <
                1LL * inDegreeSimple[j].size() * outDegreeSimple[j].size());
    });
    checkTime();
    bool ret = false;
    for (auto node : candidatesSorted) {
        checkTime();
        if (visitedCORE[node]) continue;

        if (!outDegreeSimple[node].empty() && !inDegreeSimple[node].empty()) {
            visitedCORE[node] = true;
            continue;
        }
        for (auto it : outDegreeDouble[node]) {
            checkTime();
            cliqueCORE.insert(it);
        }
        bool isCore = true;
        if (cliqueCORE.empty()) {
            isCore = false;
        }
        for (auto it : cliqueCORE) {
            checkTime();
            int sz = 1;
            if (isCore) {
                for (auto it3 : outDegreeDouble[it]) {
                    checkTime();
                    if (it != it3 && cliqueCORE.count(it3)) {
                        ++sz;
                    }
                }
                if (sz != cliqueCORE.size()) {
                    isCore = false;
                }
            }
            visitedCORE[it] = true;
        }
        if (isCore) {
            for (auto it : cliqueCORE) {
                checkTime();
                eraseNode(it);
                if (it != node) {
                    feedbackVertexSet.insert(it);
                }
            }
            ret = true;
        }
        cliqueCORE.clear();
    }
    candidatesSorted.clear();
    return ret;
}

bool reduceDOME() {
    checkTime();
    for (auto node : candidatesNodes) {
        checkTime();
        for (auto it : outDegreeSimple[node]) {
            checkTime();
            if (inDegreeSimple[it].size() >= inDegreeSimple[node].size() && inDegreeSimple[node].size() <= 50) {
                bool status = true;
                for (auto it2 : inDegreeSimple[node]) {
                    checkTime();
                    if (!inDegreeSimple[it].count(it2)) {
                        status = false;
                        break;
                    }
                }
                if (status) {
                    toBeErasedDOME.emplace_back(node, it);
                    continue;
                }
            }
            if (outDegreeSimple[node].size() >= outDegreeSimple[it].size() && outDegreeSimple[it].size() <= 50) {
                bool status = true;
                for (auto it2 : outDegreeSimple[it]) {
                    checkTime();
                    if (!outDegreeSimple[node].count(it2)) {
                        status = false;
                        break;
                    }
                }
                if (status) {
                    toBeErasedDOME.emplace_back(node, it);
                    continue;
                }
            }
        }
    }
    bool ret = !toBeErasedDOME.empty();
    for (auto it : toBeErasedDOME) {
        checkTime();
        eraseEdge(it.first, it.second, 3);
    }
    toBeErasedDOME.clear();
    checkTime();
    return ret;
}

int countSCC(int node) {
    checkTime();
    sccIndex = 0;
    sccCounter = 0;
    for (auto it : candidatesNodes) {
        checkTime();
        lowLevel[it] = 0;
        currLevel[it] = 0;
        inStack[it] = false;
    }
    bool ret = checkTarjan(node);
    sccIndex = 0;
    sccCounter = 0;
    sccStack.clear();
    return ret;
}

bool reduceSCC() {
    checkTime();
    sccIndex = 0;
    sccCounter = 0;
    for (auto it : candidatesNodes) {
        checkTime();
        lowLevel[it] = 0;
        currLevel[it] = 0;
        inStack[it] = false;
    }
    for (auto it : candidatesNodes) {
        checkTime();
        if (!currLevel[it]) {
            checkTime();
            runTarjan(it);
        }
    }
    for (auto it : edges) {
        checkTime();
        if (edges.count(make_pair(it.second, it.first))) continue;
        if (whichSCC[it.first] != whichSCC[it.second]) {
            toBeErasedSCC.emplace_back(it);
        }
    }
    for (auto it : toBeErasedSCC) {
        checkTime();
        eraseEdge(it.first, it.second, 3);
    }
    bool ret = !toBeErasedSCC.empty();
    toBeErasedSCC.clear();
    sccIndex = 0;
    sccCounter = 0;
    sccStack.clear();
    return ret;
}

void doBasicReductions() {
    checkTime();
    bool change = true;
    while (change) {
        checkTime();
        change = false;
        change |= !selfLoopNodes.empty();
        while (!selfLoopNodes.empty()) {
            checkTime();
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
            checkTime();
            int node = zeroDegreeNodes.back();
            zeroDegreeNodes.pop_back();
            if ((!inDegreeDouble[node].empty() || !inDegreeSimple[node].empty()) &&
                (!outDegreeSimple[node].empty() || !outDegreeDouble[node].empty())) {
                continue;
            }
            eraseNode(node);
        }
        change |= !oneDegreeNodes.empty();
        while (!oneDegreeNodes.empty()) {
            checkTime();
            int node = oneDegreeNodes.back();
            oneDegreeNodes.pop_back();
            bypassNode(node);
        }
    }
}

void loop() {
    bool change = true;
    char count = 0;
    while (change && count++ < 3) {
        checkTime();
        change = false;
        change |= reduceSCC();
        doBasicReductions();
        change |= reduceCORE();
        doBasicReductions();
        //change |= reduceDOME();
        //doBasicReductions();
    }
}

void findDFVS() {
    checkTime();
    doBasicReductions();
    int edgesCount = edges.size();
    currentErased = 0;
    loop();
    while (!availableNodes.empty()) {
        checkTime();
        pair<int, long long> topNode = make_pair((*availableNodes.begin()).second, (*availableNodes.begin()).first);
        availableNodes.erase(availableNodes.begin());
        if (!availableNode[topNode.first]) {
            continue;
        }
        ++currentErased;
        eraseNode(topNode.first);
        doBasicReductions();
        if (getElapsed() < SECONDS - 60 &&
            (edges.size() * 4 < edgesCount * 3 || (edges.size() * 10 < edgesCount * 9 && !firstTime))) {
            edgesCount = edges.size();
            loop();
        }
        feedbackVertexSet.insert(topNode.first);
    }
    lastFeedbackVertexSet = feedbackVertexSet;
    if (bestFeedbackVertexSet.size() > feedbackVertexSet.size()) {
        bestFeedbackVertexSet = feedbackVertexSet;
    }
}

void readData() {
    string path_input = R"(C:\Users\andre\OneDrive\Desktop\pace\heuristic_public\h_)";
    for (int l = 1; l <= 3 - to_string(testNo).size(); ++l) {
        path_input.push_back('0');
    }
    path_input += to_string(testNo);
    ifstream in(path_input);
    int t;
    while (getline(in, s)) {
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
        getline(in, s);
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
    for (int i = 1; i < n; ++i) {
        bestFeedbackVertexSet.insert(i);
    }
    in.close();
    if (n == 0) {
        exit(0);
    }
}

void createInitialDFVS() {
    checkTime();
    candidatesNodes = candidatesNodesReduced;
    inDegreeSimple = inDegreeReducedSimple;
    inDegreeDouble = inDegreeReducedDouble;
    outDegreeSimple = outDegreeReducedSimple;
    outDegreeDouble = outDegreeReducedDouble;
    edges = edgesReduced;
    availableNode = availableNodeReduced;
    for (auto x : candidatesNodes) {
        checkTime();
        if (!checkNodeCanBeReduced(x)) {
            if (lastFitness[x] != -1) {
                availableNodes.erase(make_pair(lastFitness[x], x));
            }
            availableNodes.insert(make_pair(getFitness(x), x));
            lastFitness[x] = getFitness(x);
        }
    }
    cout << candidatesNodes.size() << '\n';
    findDFVS();
    clearSets();
    initializeSets();
}

void improveFeedbackVertexSet() {
    checkTime();
    availableNode = availableNodeReduced;
    for (auto it : candidatesNodesReduced) {
        checkTime();
        if (!lastFeedbackVertexSet.count(it)) {
            candidatesNodes.insert(it);
        } else {
            availableNode[it] = false;
        }
    }
    for (auto it : edgesReduced) {
        checkTime();
        bool node1 = false, node2 = false;
        if (candidatesNodes.count(it.first)) {
            node1 = true;
        }
        if (candidatesNodes.count(it.second)) {
            node2 = true;
        }
        if (!node1 || !node2) {
            continue;
        }
        edges.insert(it);
    }
    for (auto it : edges) {
        checkTime();
        if (edges.count(make_pair(it.second, it.first))) {
            outDegreeDouble[it.first].insert(it.second);
            inDegreeDouble[it.second].insert(it.first);
        } else {
            outDegreeSimple[it.first].insert(it.second);
            inDegreeSimple[it.second].insert(it.first);
        }
    }

    localDFVS.reserve(lastFeedbackVertexSet.size());
    for (auto it : lastFeedbackVertexSet) {
        checkTime();
        localDFVS.emplace_back(it);
    }
    sort(localDFVS.begin(), localDFVS.end(), [](const int i, const int j) {
        return 1LL * inDegreeReducedSimple[i].size() * outDegreeReducedSimple[i].size() >
               1LL * inDegreeReducedSimple[j].size() * outDegreeReducedSimple[j].size();
    });
    int size = localDFVS.size();
    while (localDFVS.size() /*> size / 3*/) {
        checkTime();
        int node = localDFVS.back();
        localDFVS.pop_back();
        bool dag = true;
        for (auto it : inDegreeReducedDouble[node]) {
            checkTime();
            if (candidatesNodes.count(it)) {
                dag = false;
                break;
            }
        }
        if (!dag) {
            continue;
        }
        for (auto it : inDegreeReducedSimple[node]) {
            checkTime();
            if (candidatesNodes.count(it)) {
                inDegreeSimple[node].insert(it);
                outDegreeSimple[it].insert(node);
                edges.insert(make_pair(it, node));
            }
        }
        for (auto it : outDegreeReducedSimple[node]) {
            checkTime();
            if (candidatesNodes.count(it)) {
                outDegreeSimple[node].insert(it);
                inDegreeSimple[it].insert(node);
                edges.insert(make_pair(node, it));
            }
        }
        candidatesNodes.insert(node);
        availableNode[node] = true;
        if (countSCC(node)) {
            lastFeedbackVertexSet.erase(node);
        } else {
            for (auto it : inDegreeSimple[node]) {
                checkTime();
                if (it != node) {
                    outDegreeSimple[it].erase(node);
                    edges.erase(make_pair(it, node));
                }
            }
            inDegreeSimple[node].clear();
            for (auto it : outDegreeSimple[node]) {
                checkTime();
                if (it != node) {
                    inDegreeSimple[it].erase(node);
                    edges.erase(make_pair(node, it));
                }
            }
            outDegreeSimple[node].clear();
            edges.erase(make_pair(node, node));
            candidatesNodes.erase(node);
            availableNode[node] = false;
        }
    }
    localDFVS.clear();
    clearSets();
    initializeSets();
    if (lastFeedbackVertexSet.size() < bestFeedbackVertexSet.size()) {
        bestFeedbackVertexSet = lastFeedbackVertexSet;
    }
}

void doLocalSearch() {
    checkTime();
    cout << "++" << getElapsed() << '\n';
    fitnessType = 2;
    checkTime();
    for (auto node : bestFeedbackVertexSet) {
        checkTime();
        local.emplace_back(node);
    }
    checkTime();
    unsigned seed = rand();
    shuffle(local.begin(), local.end(), default_random_engine(seed));
    int toBeErasedCounter = (int) local.size() * 3 / 4;
    while (toBeErasedCounter--) {
        checkTime();
        int node = local.back();
        local.pop_back();
        localSet.insert(node);
        feedbackVertexSet.insert(node);
    }
    local.clear();
    availableNode = availableNodeReduced;
    for (auto it : candidatesNodesReduced) {
        checkTime();
        if (!localSet.count(it)) {
            candidatesNodes.insert(it);
        } else {
            availableNode[it] = false;
        }
    }
    for (auto it : edgesReduced) {
        checkTime();
        bool node1 = false, node2 = false;
        if (candidatesNodes.count(it.first)) {
            node1 = true;
        }
        if (candidatesNodes.count(it.second)) {
            node2 = true;
        }
        if (!node1 || !node2) {
            continue;
        }
        edges.insert(it);
    }
    for (auto it : edges) {
        checkTime();
        if (edges.count(make_pair(it.second, it.first))) {
            outDegreeDouble[it.first].insert(it.second);
            inDegreeDouble[it.second].insert(it.first);
        } else {
            outDegreeSimple[it.first].insert(it.second);
            inDegreeSimple[it.second].insert(it.first);
        }
    }
    loop();
    for (auto x : candidatesNodes) {
        checkTime();
        if (!checkNodeCanBeReduced(x)) {
            if (lastFitness[x] != -1) {
                availableNodes.erase(make_pair(lastFitness[x], x));
            }
            availableNodes.insert(make_pair(getFitness(x), x));
            lastFitness[x] = getFitness(x);
        }
    }
    cout << "--" << getElapsed() << '\n';
    findDFVS();
    clearSets();
    initializeSets();
    improveFeedbackVertexSet();
    cout << "**" << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    clearSets();
    initializeSets();
    local.clear();
    localSet.clear();
    lastFeedbackVertexSet.clear();
}

double getElapsed() {
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin_);
    double sec = elapsed.count() * 1e-9;
    return sec;
}

void checkTime() {
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin_);
    double sec = elapsed.count() * 1e-9;
    if (sec >= SECONDS - 30) {
        //string path_output =R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)" + to_string(testNo) + ".out";
        //ofstream out(path_output);
        ios_base::sync_with_stdio(false);
        cout.tie();
        //out << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
        //out << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
        for (auto it: feedbackVertexSetReduced) {
            output += to_string(it) + '\n';
        }
        for (auto node : bestFeedbackVertexSet) {
            output += to_string(node) + '\n';
        }
        cout << output;
        //out.close();
        //output.clear();
        exit(0);
    }
}

long long getFitness(int node) {
    if (fitnessType == 1) {
        return 1LL * outDegreeDouble[node].size() * (long long) 1e11 +
               1LL * (inDegreeSimple[node].size()) * (outDegreeSimple[node].size());
    }
    if (fitnessType == 2) {
        return 1LL * (inDegreeSimple[node].size() + inDegreeDouble[node].size()) *
               (outDegreeSimple[node].size() + outDegreeDouble[node].size());
    }
    return 0;
}

void solveTestcase() {
    begin_ = std::chrono::high_resolution_clock::now();
    srand(0);
    readData();
    fitnessType = 1;
    for (int i = 1; i <= n; ++i) {
        checkNodeCanBeReduced(i);
    }
    for (int i = 1; i <= n; ++i) {
        candidatesNodes.insert(i);
    }
    loop();
    candidatesNodesReduced = candidatesNodes;
    inDegreeReducedSimple = inDegreeSimple;
    inDegreeReducedDouble = inDegreeDouble;
    outDegreeReducedSimple = outDegreeSimple;
    outDegreeReducedDouble = outDegreeDouble;
    feedbackVertexSetReduced = feedbackVertexSet;
    edgesReduced = edges;
    availableNodeReduced = availableNode;
    feedbackVertexSet.clear();
    for (fitnessType = 1; fitnessType <= 1; ++fitnessType) {
        checkTime();
        createInitialDFVS();
        clearSets();
        initializeSets();
        improveFeedbackVertexSet();
        cout << "!!" << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    }
    firstTime = 1;
    cout << "()" << getElapsed() << '\n';
    clearSets();
    initializeSets();
    for (;;) {
        checkTime();
        doLocalSearch();
    }
    string path_output =
            R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)" + to_string(testNo) + ".out";
    ofstream out(path_output);
    out << feedbackVertexSetReduced.size() + bestFeedbackVertexSet.size() << '\n';
    cout << feedbackVertexSetReduced.size() + bestFeedbackVertexSet.size() << '\n';
    for (auto it: feedbackVertexSetReduced) {
        output += to_string(it) + '\n';
    }
    for (auto node : bestFeedbackVertexSet) {
        output += to_string(node) + '\n';
    }
    out << output;
    out.close();
    output.clear();
}