#include <bits/stdc++.h>
#include <chrono>

using namespace std;
const int SECONDS = 600;

struct cmp {
    bool inline operator()(const pair<int, long long> &i, const pair<int, long long> &j) {
        return i.second < j.second || (i.second == j.second && i.first < j.first);
    }
};

priority_queue<pair<int, long long>, vector<pair<int, long long>>, cmp> availableNodes;
std::chrono::time_point<std::chrono::high_resolution_clock> begin_;
int n, m, fitnessType, sccIndex, sccCounter;
int testNo, erasedDoubleEdges = 0;

vector<set<int>> inDegreeDouble, outDegreeDouble;
vector<set<int>> inDegreeSimple, outDegreeSimple;
vector<set<int>> inDegreeReducedDouble, outDegreeReducedDouble;
vector<set<int>> inDegreeReducedSimple, outDegreeReducedSimple;
set<pair<int, int>> edges;
set<pair<int, int>> edgesReduced;
set<int> candidatesNodes, candidatesNodesReduced, feedbackVertexSet, bestFeedbackVertexSet;
set<int> feedbackVertexSetReduced;
vector<int> selfLoopNodes, zeroDegreeNodes, oneDegreeNodes;
vector<bool> availableNode, inStack;
vector<bool> availableNodeReduced;
set<int> feedbackVertexSetLocalSearch;
vector<int> feedbackVertexSetList;
vector<int> lowLevel, sccStack, currLevel, whichSCC;
vector<pair<pair<int, set<int>::iterator>, int> > stackTarjan;
set<int> cliqueCORE;
vector<bool> visitedCORE;
vector<int> candidatesSorted;
string output;


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
        string path_output =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)" + to_string(testNo) + ".out";
        ofstream out(path_output);
        cout << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
        out << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
        for (auto it: feedbackVertexSetReduced) {
            output += to_string(it) + '\n';
            // bestFeedbackVertexSet.insert(it);
        }
        //cout << bestFeedbackVertexSet.size() << '\n';
        for (auto node : bestFeedbackVertexSet) {
            output += to_string(node) + '\n';
        }
        ios_base::sync_with_stdio(false);
        cout.tie(nullptr);
        //cout << output;
        out << output;
        //cout << "time elapsed in seconds: " << double(clock() - begin_) / CLOCKS_PER_SEC << '\n';
        //out.close();
        //output.clear();
        exit(0);
    }
}


long long getFitness(int node) {
    if (fitnessType == 0) {
        return 1LL * (inDegreeSimple[node].size() + outDegreeDouble[node].size()) *
               (outDegreeSimple[node].size() + outDegreeDouble[node].size());
    }
    if (fitnessType == 1) {
        return 1LL * outDegreeDouble[node].size() * 1e11 +
               1LL * (inDegreeSimple[node].size()) * (outDegreeSimple[node].size());
    }
    if (fitnessType == 2) {
        return 1LL * (inDegreeSimple[node].size() + inDegreeDouble[node].size() * 2) *
               (outDegreeSimple[node].size() + outDegreeDouble[node].size() * 2);
    }
    if (fitnessType == 3) {
        return inDegreeSimple[node].size() + outDegreeSimple[node].size() + 2 * inDegreeDouble[node].size() -
               3 * abs(((int) inDegreeSimple[node].size()) - ((int) outDegreeDouble[node].size())) / 10;
    }

    if (fitnessType == 4) {
        return 1LL * (1LL * inDegreeDouble[node].size() * outDegreeDouble[node].size() + outDegreeSimple[node].size() +
                      inDegreeSimple[node].size());
    }

    if (fitnessType == 5) {
        return 1LL * (inDegreeSimple[node].size() + 1LL * inDegreeDouble[node].size() * 120) *
               (outDegreeSimple[node].size() + 1LL * outDegreeDouble[node].size() * 120);
    }
    /*
    if (fitnessType == 6) {
        return inDegree[node].size() + outDegree[node].size() -
               2 * abs(((int) inDegree[node].size()) - ((int) outDegree[node].size())) / 10;
    }
    */
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
        outDegreeDouble[y].erase(x);
    }
    if ((z == 1 || z == 3) && !checkNodeCanBeReduced(x)) {
        availableNodes.push(make_pair(x, getFitness(x)));
    }
    if ((z == 2 || z == 3) && !checkNodeCanBeReduced(y)) {
        availableNodes.push(make_pair(y, getFitness(y)));
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
    visitedCORE.clear();
    whichSCC.clear();
    feedbackVertexSet.clear();
    availableNode.clear();
    selfLoopNodes.clear();
    zeroDegreeNodes.clear();
    oneDegreeNodes.clear();
    while (!availableNodes.empty()) {
        checkTime();
        availableNodes.pop();
    }
    candidatesNodes.clear();
    edges.clear();
    inDegreeSimple.clear();
    inDegreeDouble.clear();
    outDegreeSimple.clear();
    outDegreeDouble.clear();
}

vector<pair<pair<int, int>, int >> toBeErasedNode;

void eraseNode(int node) {
    checkTime();
    if (!availableNode[node]) {
        return;
    }
    availableNode[node] = false;
    candidatesNodes.erase(node);
    for (auto it : inDegreeSimple[node]) {
        checkTime();
        toBeErasedNode.emplace_back(make_pair(it, node), 1);
    }
    for (auto it : inDegreeDouble[node]) {
        checkTime();
        toBeErasedNode.emplace_back(make_pair(it, node), 1);
        ++erasedDoubleEdges;
    }
    for (auto it : outDegreeSimple[node]) {
        checkTime();
        toBeErasedNode.emplace_back(make_pair(node, it), 2);
    }
    for (auto it : outDegreeDouble[node]) {
        checkTime();
        toBeErasedNode.emplace_back(make_pair(node, it), 2);
        ++erasedDoubleEdges;
    }
    for (auto it : toBeErasedNode) {
        checkTime();
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
            if (edges.count(make_pair(it1, it2))) {
                checkNodeCanBeReduced(it1);
                availableNodes.push(make_pair(it1, getFitness(it1)));
                checkNodeCanBeReduced(it2);
                availableNodes.push(make_pair(it2, getFitness(it2)));
            }
            if (it1 == it2) {
                edges.insert(make_pair(it1, it2));
                selfLoopNodes.emplace_back(it1);
                erasedDoubleEdges += 2;
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

vector<int> TarjanNodes;

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
            TarjanNodes.emplace_back(stackTarjan.back().first.first);
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


void reduceCORE() {
    checkTime();
    for (auto it : candidatesNodes) {
        visitedCORE[it] = false;
    }
    for (auto it : edges) {
        checkTime();
        if (!edges.count(make_pair(it.second, it.first))) {
            visitedCORE[it.first] = true;
            visitedCORE[it.second] = true;
        }
    }
    checkTime();
    candidatesSorted.reserve(candidatesNodes.size());
    for (auto node : candidatesNodes) {
        checkTime();
        candidatesSorted.emplace_back(node);
    }
    checkTime();
    sort(candidatesSorted.begin(), candidatesSorted.end(), [](const int i, const int j) {
        return inDegreeSimple[i].size() + inDegreeDouble[i].size() + outDegreeSimple[i].size() +
               outDegreeDouble[i].size() <
               inDegreeSimple[j].size() + inDegreeDouble[j].size() + outDegreeSimple[j].size() +
               outDegreeDouble[j].size();
    });
    checkTime();
    for (auto node : candidatesSorted) {
        checkTime();
        if (visitedCORE[node]) continue;
        cliqueCORE.insert(node);
        if (!outDegreeSimple[node].empty()) {
            visitedCORE[node] = true;
            continue;
        }
        for (auto it : outDegreeDouble[node]) {
            checkTime();
            cliqueCORE.insert(it);
        }
        bool isCore = true;
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
            visitedCORE[it] = false;
        }
        if (isCore) {
            for (auto it : cliqueCORE) {
                checkTime();
                eraseNode(it);
                if (it != node) {
                    feedbackVertexSet.insert(it);
                }
            }
        }
        cliqueCORE.clear();
    }
    candidatesSorted.clear();
}

vector<pair<int, int>> toBeErasedDOME;

void reduceDOME() {
    checkTime();
    for (auto node : candidatesNodes) {
        checkTime();
        for (auto it : outDegreeSimple[node]) {
            checkTime();
            if (inDegreeSimple[it].size() >= inDegreeSimple[node].size() && inDegreeSimple[node].size() <= 10) {
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
            if (outDegreeSimple[node].size() >= outDegreeSimple[it].size() && outDegreeSimple[it].size() <= 10) {
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
    for (auto it : toBeErasedDOME) {
        checkTime();
        eraseEdge(it.first, it.second, 3);
    }
    toBeErasedDOME.clear();
    checkTime();
}

vector<pair<int, int>> toBeErasedSCC;

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


void reduceSCC() {
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
    toBeErasedSCC.clear();
    sccIndex = 0;
    sccCounter = 0;
    sccStack.clear();
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


void findDFVS() {
    checkTime();
    doBasicReductions();
    erasedDoubleEdges = 0;
    int simpleEdges = 0;
    int edgesCount = edges.size();
    for (auto node  : candidatesNodes) {
        simpleEdges += inDegreeSimple[node].size();
    }
    while (!availableNodes.empty()) {
        checkTime();
        pair<int, long long> topNode = availableNodes.top();
        availableNodes.pop();
        if (!availableNode[topNode.first] ||
            topNode.second != getFitness(topNode.first)) {
            continue;
        }
        eraseNode(topNode.first);
        doBasicReductions();

        if (getElapsed() < SECONDS - 30 - 60 * 5) {
            if (edges.size() < edgesCount * 4 / 5) {
                edgesCount = edges.size();
                reduceSCC();
                doBasicReductions();
                reduceCORE();
                doBasicReductions();
                reduceDOME();
                doBasicReductions();
            }
        } else {
            if (getElapsed() < SECONDS - 30 - 60 * 3) {
                if (edges.size() < edgesCount * 3 / 4) {
                    edgesCount = edges.size();
                    reduceSCC();
                    doBasicReductions();
                    reduceCORE();
                    doBasicReductions();
                    reduceDOME();
                    doBasicReductions();
                }
            } else {
                if (getElapsed() < SECONDS - 30 - 60 * 1) {
                    if (edges.size() < edgesCount * 2 / 3) {
                        edgesCount = edges.size();
                        reduceSCC();
                        doBasicReductions();
                        reduceCORE();
                        doBasicReductions();
                        reduceDOME();
                        doBasicReductions();
                    }
                } else {
                    if (edges.size() < edgesCount * 1 / 2) {
                        edgesCount = edges.size();
                        reduceSCC();
                        doBasicReductions();
                        reduceCORE();
                        doBasicReductions();
                        reduceDOME();
                        doBasicReductions();
                    }
                }
            }
        }
        /*
        if ((edgesCount - ((int) edges.size() + erasedDoubleEdges)) > simpleEdges / 15 && getElapsed() < SECONDS - 90) {
            reduceSCC();
            if (getElapsed() < SECONDS - 120) {
                doBasicReductions();
                reduceCORE();
            }
            if (getElapsed() < SECONDS - 150) {
                doBasicReductions();
                reduceDOME();
            }
            doBasicReductions();
            edgesCount = edges.size();
            simpleEdges -= (edgesCount - ((int) edges.size() + erasedDoubleEdges));
            doubleEdges = ((int) edges.size()) - simpleEdges;
            erasedDoubleEdges = 0;
        }
        */
        /*
        if (erasedDoubleEdges > doubleEdges / 7 && getElapsed() < SECONDS - 120) {
            doBasicReductions();
            reduceCORE();
            doBasicReductions();
            edgesCount = edges.size();
            simpleEdges -= (edgesCount - ((int) edges.size() + erasedDoubleEdges));
            doubleEdges = ((int) edges.size()) - simpleEdges;
            erasedDoubleEdges = 0;
        }
         */
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
        // empty testcase
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
    for (auto i : candidatesNodes) {
        checkTime();
        if (!checkNodeCanBeReduced(i)) {
            availableNodes.push(make_pair(i, getFitness(i)));
        }
    }
    findDFVS();
    feedbackVertexSetLocalSearch = feedbackVertexSet;
    clearSets();
    initializeSets();
}

void doLocalSearch() {
    checkTime();
    candidatesNodes = candidatesNodesReduced;
    inDegreeSimple = inDegreeReducedSimple;
    inDegreeDouble = inDegreeReducedDouble;
    outDegreeSimple = outDegreeReducedSimple;
    outDegreeDouble = outDegreeReducedDouble;
    edges = edgesReduced;
    availableNode = availableNodeReduced;
    feedbackVertexSetList.clear();
    for (auto node : feedbackVertexSetLocalSearch) {
        checkTime();
        feedbackVertexSetList.emplace_back(node);
    }
    unsigned seed = rand();
    shuffle(feedbackVertexSetList.begin(), feedbackVertexSetList.end(), default_random_engine(seed));
    int size = feedbackVertexSetList.size();
    int toBeErasedCounter = size / 2 + rand() % (size / 2 - size / 3 + 1);
    while (toBeErasedCounter--) {
        checkTime();
        int node = feedbackVertexSetList.back();
        feedbackVertexSetList.pop_back();
        eraseNode(node);
        doBasicReductions();
        feedbackVertexSet.insert(node);
    }
    for (auto i : candidatesNodes) {
        if (!checkNodeCanBeReduced(i)) {
            availableNodes.push(make_pair(i, getFitness(i)));
        }
    }
    findDFVS();
    if (feedbackVertexSet.size() < feedbackVertexSetLocalSearch.size()) {
        feedbackVertexSetLocalSearch = feedbackVertexSet;
    }
    clearSets();
    initializeSets();
}

void improveFeedbackVertexSet() {
    checkTime();
    candidatesNodes = candidatesNodesReduced;
    inDegreeSimple = inDegreeReducedSimple;
    inDegreeDouble = inDegreeReducedDouble;
    outDegreeSimple = outDegreeReducedSimple;
    outDegreeDouble = outDegreeReducedDouble;
    edges = edgesReduced;
    availableNode = availableNodeReduced;
    feedbackVertexSetList.clear();
    for (auto i : candidatesNodes) {
        checkTime();
        candidatesNodes.insert(i);
    }
    for (auto node : bestFeedbackVertexSet) {
        checkTime();
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
        for (auto it : inDegreeDouble[node]) {
            checkTime();
            if (it != node) {
                outDegreeDouble[it].erase(node);
                edges.erase(make_pair(it, node));
            }
        }
        inDegreeDouble[node].clear();
        for (auto it : outDegreeDouble[node]) {
            checkTime();
            if (it != node) {
                inDegreeDouble[it].erase(node);
                edges.erase(make_pair(node, it));
            }
        }
        outDegreeDouble[node].clear();
        edges.erase(make_pair(node, node));
        candidatesNodes.erase(node);
        availableNode[node] = false;
    }

    vector<int> localDFVS;
    localDFVS.reserve(bestFeedbackVertexSet.size());
    for (auto it : bestFeedbackVertexSet) {
        checkTime();
        localDFVS.emplace_back(it);
    }
    sort(localDFVS.begin(), localDFVS.end(), [](const int i, const int j) {
        return inDegreeReducedSimple[i].size() + outDegreeReducedSimple[i].size() >
               inDegreeReducedSimple[j].size() + outDegreeReducedSimple[j].size();
    });
    while (!localDFVS.empty()) {
        cout << "**" << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
        checkTime();
        int node = localDFVS.back();
        localDFVS.pop_back();
        bool dag = true;
        if (inDegreeReducedSimple[node].count(node) || inDegreeReducedDouble[node].count(node)) {
            continue;
        }
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
            bestFeedbackVertexSet.erase(node);
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
    clearSets();
    initializeSets();
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
    for (int i = 1; i <= 5; ++i) {
        checkTime();
        int last = edges.size();
        reduceSCC();
        doBasicReductions();
        if (last - edges.size() < 1000) {
            break;
        }
    }
    reduceCORE();
    doBasicReductions();
    reduceDOME();
    doBasicReductions();
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
    }
    cout << getElapsed() << ' ' << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    improveFeedbackVertexSet();
    fitnessType = 1;

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
        //bestFeedbackVertexSet.insert(it);
    }
    for (auto node : bestFeedbackVertexSet) {
        output += to_string(node) + '\n';
    }
    //ios_base::sync_with_stdio(false);
    //cout.tie(nullptr);
    //out.tie(nullptr);
    //cout << output;
    out << output;
    //out << bestFeedbackVertexSet.size() << '\n';
    //for (auto it: bestFeedbackVertexSet) {
    //    out << it << '\n';
    // }
    // cout << "time elapsed in seconds: " << double(clock() - begin_) / CLOCKS_PER_SEC << '\n';
    out.close();
    output.clear();
}


signed main() {
    /*
    set<int> test;
    cout.sync_with_stdio(false);
    cout.tie();
    for (int i = 1; output.size() < 20 * 1024 * 1024; ++i) {
        output += to_string(i) + ' ';
        test.insert(i);
    }
    cout << output;
    cout << "time elapsed in seconds: " << double(clock() - begin_) / CLOCKS_PER_SEC << '\n';
    return 0;
     */
    for (testNo = 199; testNo <= 199; testNo += 2) {
        //cout << testNo << ' ';
        solveTestcase();
        //cout << getElapsed() << '\n';
    }
    return 0;
}