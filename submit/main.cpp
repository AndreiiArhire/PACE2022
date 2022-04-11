#include <bits/stdc++.h>
#include <chrono>

using namespace std;
const int SECONDS = 600;

std::chrono::time_point<std::chrono::high_resolution_clock> begin_;
int n, m, fitnessType, sccIndex, sccCounter;
int testNo;
vector<pair<pair<int, int>, int >> toBeErasedBypass;
vector<pair<pair<int, int>, int >> toBeErasedMerge;

vector<set<int>> inDegreeDouble, outDegreeDouble;
vector<set<int>> inDegreeSimple, outDegreeSimple;
vector<set<int>> inDegreeReducedDouble, outDegreeReducedDouble;
vector<set<int>> inDegreeReducedSimple, outDegreeReducedSimple;
set<pair<int, int>> edges;
set<pair<int, int>> edgesReduced;
set<int> candidatesNodes, candidatesNodesReduced, feedbackVertexSet, bestFeedbackVertexSet, lastFeedbackVertexSet;
set<int> feedbackVertexSetReduced;
vector<int> selfLoopNodes, zeroDegreeNodes, oneDegreeNodes;
vector<bool> availableNode, inStack;
vector<bool> availableNodeReduced;
vector<int> feedbackVertexSetList;
vector<int> lowLevel, sccStack, currLevel, whichSCC;
vector<pair<pair<int, set<int>::iterator>, int> > stackTarjan;
vector<bool> partOfSet;
set<int> cliqueCORE;
vector<bool> visitedCORE;
vector<int> candidatesSorted;
string output;
vector<int> inNodes, outNodes;
int currentErased, toBeAvoided, alpha;
set<int> notInDFVS;

struct cmpR {
    bool inline operator()(const pair<int, long long> &i, const pair<int, long long> &j) {
        return i.second < j.second || (i.second == j.second && i.first < j.first);
    }
};

/*
struct cmpM {
    bool inline operator()(const pair<int, long long> &i, const pair<int, long long> &j) {
        if (i.second < 1e11 && j.second < 1e11) {
            return i.second < j.second || (i.second == j.second && i.first < j.first);
        }
        if (i.second < 1e11) {
            return true;
        }
        if (j.second < 1e11) {
            return false;
        }
        return i.second > j.second || (i.second == j.second && i.first < j.first);
    }
};
*/

struct cmpM {
    bool inline operator()(const pair<int, long long> &i, const pair<int, long long> &j) {
        return i.second > j.second || (i.second == j.second && i.first < j.first);
    }
};

priority_queue<pair<int, long long>, vector<pair<int, long long>>, cmpR> availableNodesR;
priority_queue<pair<int, long long>, vector<pair<int, long long>>, cmpM> availableNodesM;


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
    if (sec >= SECONDS - 60) {
        string path_output =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)" + to_string(testNo) + ".out";
        ofstream out(path_output);
        //cout << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
        //out << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
        for (auto it: feedbackVertexSetReduced) {
            output += to_string(it) + '\n';
            // bestFeedbackVertexSet.insert(it);
        }
        //cout << bestFeedbackVertexSet.size() << '\n';
        for (auto node : bestFeedbackVertexSet) {
            output += to_string(node) + '\n';
        }
        //ios_base::sync_with_stdio(false);
        //cout.tie(nullptr);
        cout << output;
        //out << output;
        //cout << "time elapsed in seconds: " << double(clock() - begin_) / CLOCKS_PER_SEC << '\n';
        // out.close();
        // output.clear();
        exit(0);
    }
}

long long getFitness(int node) {
    if (fitnessType == 1) {
        return 1LL * (inDegreeSimple[node].size() + 2 * outDegreeDouble[node].size()) *
               (outDegreeSimple[node].size() + 2 * outDegreeDouble[node].size());
    }
    if (fitnessType == 2) {
        return 1LL * outDegreeDouble[node].size() * 1e11 +
               1LL * (inDegreeSimple[node].size()) * (outDegreeSimple[node].size());
    }
    if (fitnessType == 3) {
        return 1LL * (inDegreeSimple[node].size() + 1 * outDegreeDouble[node].size()) *
               (outDegreeSimple[node].size() + 1 * outDegreeDouble[node].size());
    }
    if (fitnessType == 4) {
        return 1LL * (inDegreeSimple[node].size() + 3 * outDegreeDouble[node].size()) *
               (outDegreeSimple[node].size() + 3 * outDegreeDouble[node].size());
    }
    if (fitnessType == 5) {
        return 1LL * (inDegreeSimple[node].size() + 5 * outDegreeDouble[node].size()) *
               (outDegreeSimple[node].size() + 5 * outDegreeDouble[node].size());
    }
    if (fitnessType == 6) {
        return 1LL * inDegreeDouble[node].size() * outDegreeDouble[node].size() + 1LL * (inDegreeSimple[node].size()) *
                                                                                  (outDegreeSimple[node].size());
    }
    if (fitnessType == 7) {
        return 1LL * inDegreeDouble[node].size() * outDegreeDouble[node].size() + inDegreeSimple[node].size() +
               outDegreeSimple[node].size() -
               3 * abs(((int) inDegreeSimple[node].size()) - ((int) outDegreeSimple[node].size())) / 10;
    }
    if (fitnessType == 8) {
        return 1LL * (inDegreeSimple[node].size() + alpha * outDegreeDouble[node].size() / 100) *
               (outDegreeSimple[node].size() + alpha * outDegreeDouble[node].size() / 100);
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

void eraseEdgeR(int x, int y, int z) {
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
        availableNodesR.push(make_pair(x, getFitness(x)));
    }
    if ((z == 2 || z == 3) && !checkNodeCanBeReduced(y)) {
        availableNodesR.push(make_pair(y, getFitness(y)));
    }
}

void eraseEdgeM(int x, int y, int z) {
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
        availableNodesM.push(make_pair(x, getFitness(x)));
    }
    if ((z == 2 || z == 3) && !checkNodeCanBeReduced(y)) {
        availableNodesM.push(make_pair(y, getFitness(y)));
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
    partOfSet.resize(n + 1, true);
    inDegreeSimple.resize(n + 1, set<int>());
    inDegreeDouble.resize(n + 1, set<int>());
    outDegreeSimple.resize(n + 1, set<int>());
    outDegreeDouble.resize(n + 1, set<int>());
}

void clearSets() {
    checkTime();
    notInDFVS.clear();
    visitedCORE.clear();
    whichSCC.clear();
    feedbackVertexSet.clear();
    availableNode.clear();
    partOfSet.clear();
    selfLoopNodes.clear();
    zeroDegreeNodes.clear();
    oneDegreeNodes.clear();
    while (!availableNodesR.empty()) {
        checkTime();
        availableNodesR.pop();
    }
    while (!availableNodesM.empty()) {
        checkTime();
        availableNodesM.pop();
    }
    candidatesNodes.clear();
    edges.clear();
    inDegreeSimple.clear();
    inDegreeDouble.clear();
    outDegreeSimple.clear();
    outDegreeDouble.clear();
}

vector<pair<pair<int, int>, int >> toBeErasedNode;

void eraseNodeR(int node) {
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
    }
    for (auto it : outDegreeSimple[node]) {
        checkTime();
        toBeErasedNode.emplace_back(make_pair(node, it), 2);
    }
    for (auto it : outDegreeDouble[node]) {
        checkTime();
        toBeErasedNode.emplace_back(make_pair(node, it), 2);
    }
    for (auto it : toBeErasedNode) {
        checkTime();
        eraseEdgeR(it.first.first, it.first.second, it.second);
    }
    toBeErasedNode.clear();
}

set<int> nodesToBeErased;
set<int> tempSet;

void mergeNodeM(int node) {
    checkTime();
    if (!availableNode[node]) {
        return;
    }
    availableNode[node] = false;
    candidatesNodes.erase(node);
    for (auto it :inDegreeSimple[node]) {
        checkTime();
        toBeErasedMerge.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :inDegreeDouble[node]) {
        checkTime();
        toBeErasedMerge.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :outDegreeSimple[node]) {
        checkTime();
        toBeErasedMerge.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it :outDegreeDouble[node]) {
        checkTime();
        toBeErasedMerge.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it : toBeErasedMerge) {
        checkTime();
        eraseEdgeM(it.first.first, it.first.second, it.second);
    }
    toBeErasedMerge.clear();
    for (auto it : inNodes) {
        checkTime();
        tempSet.insert(it);
    }
    for (auto it : outNodes) {
        checkTime();
        if (tempSet.count(it)) {
            nodesToBeErased.insert(it);
            edges.insert(make_pair(it, it));
            checkNodeCanBeReduced(it);
        }
    }
    for (auto it1 : inNodes) {
        checkTime();
        if (nodesToBeErased.count(it1)) {
            continue;
        }
        for (auto it2 : outNodes) {
            checkTime();
            if (nodesToBeErased.count(it2)) {
                continue;
            }
            addEdge(it1, it2);
            edges.insert(make_pair(it1, it2));
            availableNodesM.push(make_pair(it2, getFitness(it2)));
            checkNodeCanBeReduced(it2);
        }
        checkNodeCanBeReduced(it1);
        availableNodesM.push(make_pair(it1, getFitness(it1)));
    }
    inNodes.clear();
    outNodes.clear();
}


void mergeNodeR(int node) {
    checkTime();
    if (!availableNode[node]) {
        return;
    }
    availableNode[node] = false;
    candidatesNodes.erase(node);
    for (auto it :inDegreeSimple[node]) {
        checkTime();
        toBeErasedMerge.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :inDegreeDouble[node]) {
        checkTime();
        toBeErasedMerge.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :outDegreeSimple[node]) {
        checkTime();
        toBeErasedMerge.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it :outDegreeDouble[node]) {
        checkTime();
        toBeErasedMerge.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it : toBeErasedMerge) {
        checkTime();
        eraseEdgeR(it.first.first, it.first.second, it.second);
    }
    toBeErasedMerge.clear();
    for (auto it : inNodes) {
        checkTime();
        tempSet.insert(it);
    }
    for (auto it : outNodes) {
        checkTime();
        if (tempSet.count(it)) {
            nodesToBeErased.insert(it);
            edges.insert(make_pair(it, it));
            checkNodeCanBeReduced(it);
        }
    }
    for (auto it1 : inNodes) {
        checkTime();
        if (nodesToBeErased.count(it1)) {
            continue;
        }
        for (auto it2 : outNodes) {
            checkTime();
            if (nodesToBeErased.count(it2)) {
                continue;
            }
            addEdge(it1, it2);
            edges.insert(make_pair(it1, it2));
            availableNodesR.push(make_pair(it2, getFitness(it2)));
            checkNodeCanBeReduced(it2);
        }
        checkNodeCanBeReduced(it1);
        availableNodesR.push(make_pair(it1, getFitness(it1)));
    }
    inNodes.clear();
    outNodes.clear();
}

void eraseNodeM(int node) {
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
    }
    for (auto it : outDegreeSimple[node]) {
        checkTime();
        toBeErasedNode.emplace_back(make_pair(node, it), 2);
    }
    for (auto it : outDegreeDouble[node]) {
        checkTime();
        toBeErasedNode.emplace_back(make_pair(node, it), 2);
    }
    for (auto it : toBeErasedNode) {
        checkTime();
        eraseEdgeM(it.first.first, it.first.second, it.second);
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

void bypassNodeR(int node) {
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
        eraseEdgeR(it.first.first, it.first.second, it.second);
    }
    toBeErasedBypass.clear();
    for (auto it1 : inNodes) {
        checkTime();
        for (auto it2 : outNodes) {
            checkTime();
            if (edges.count(make_pair(it1, it2))) {
                checkNodeCanBeReduced(it1);
                availableNodesR.push(make_pair(it1, getFitness(it1)));
                checkNodeCanBeReduced(it2);
                availableNodesR.push(make_pair(it2, getFitness(it2)));
            }
            if (it1 == it2) {
                edges.insert(make_pair(it1, it2));
                selfLoopNodes.emplace_back(it1);
            }
            addEdge(it1, it2);
        }
    }

    if (inNodes.size() == 1) {
        availableNodesR.push(
                make_pair(inNodes.back(), getFitness(inNodes.back())));
    }
    if (outNodes.size() == 1) {
        availableNodesR.push(
                make_pair(outNodes.back(), getFitness(outNodes.back())));
    }
    inNodes.clear();
    outNodes.clear();
}

void bypassNodeM(int node) {
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
        eraseEdgeM(it.first.first, it.first.second, it.second);
    }
    toBeErasedBypass.clear();
    for (auto it1 : inNodes) {
        checkTime();
        for (auto it2 : outNodes) {
            checkTime();
            if (edges.count(make_pair(it1, it2))) {
                checkNodeCanBeReduced(it1);
                availableNodesM.push(make_pair(it1, getFitness(it1)));
                checkNodeCanBeReduced(it2);
                availableNodesM.push(make_pair(it2, getFitness(it2)));
            }
            if (it1 == it2) {
                edges.insert(make_pair(it1, it2));
                selfLoopNodes.emplace_back(it1);
            }
            addEdge(it1, it2);
        }
    }

    if (inNodes.size() == 1) {
        availableNodesM.push(
                make_pair(inNodes.back(), getFitness(inNodes.back())));
    }
    if (outNodes.size() == 1) {
        availableNodesM.push(
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

void reducePseudoCORE() {
    checkTime();
    for (auto it : candidatesNodes) {
        visitedCORE[it] = false;
    }
    checkTime();
    candidatesSorted.reserve(candidatesNodes.size());
    for (auto node : candidatesNodes) {
        checkTime();
        candidatesSorted.emplace_back(node);
    }
    checkTime();
    /*
    sort(candidatesSorted.begin(), candidatesSorted.end(), [](const int i, const int j) {
        return inDegreeSimple[i].size() + outDegreeSimple[i].size() + inDegreeDouble[i].size() <
               inDegreeSimple[j].size() + outDegreeSimple[j].size() + inDegreeDouble[j].size();
    });
    */
    sort(candidatesSorted.begin(), candidatesSorted.end(), [](const int i, const int j) {
        return (inDegreeDouble[i].size() < inDegreeDouble[j].size()) ||
               (inDegreeDouble[i].size() == inDegreeDouble[j].size() &&
                inDegreeSimple[i].size() + outDegreeSimple[i].size() <
                inDegreeSimple[j].size() + outDegreeSimple[j].size());
    });
    checkTime();
    //cout << "**\n";
    for (auto node : candidatesSorted) {
        checkTime();
        if (cliqueCORE.count(node)) {
            continue;
        }
        for (auto it : outDegreeDouble[node]) {
            checkTime();
            cliqueCORE.insert(it);
        }
    }
    for (auto it : cliqueCORE) {
        checkTime();
        eraseNodeR(it);
        feedbackVertexSet.insert(it);
    }
    cliqueCORE.clear();
    candidatesSorted.clear();
}

bool reduceCORER() {
    checkTime();
    for (auto it : candidatesNodes) {
        checkTime();
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
    bool ret = false;
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
                eraseNodeR(it);
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

void reduceCOREM() {
    checkTime();
    for (auto it : candidatesNodes) {
        checkTime();
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
                eraseNodeM(it);
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

bool reduceDOMER() {
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
        eraseEdgeR(it.first, it.second, 3);
    }
    toBeErasedDOME.clear();
    checkTime();
    return ret;
}

bool reduceDOMEM() {
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
    bool ret = !toBeErasedDOME.empty();
    for (auto it : toBeErasedDOME) {
        checkTime();
        eraseEdgeM(it.first, it.second, 3);
    }
    toBeErasedDOME.clear();
    checkTime();
    return ret;
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


bool reduceSCCR() {
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
        eraseEdgeR(it.first, it.second, 3);
    }
    bool ret = !toBeErasedSCC.empty();
    toBeErasedSCC.clear();
    sccIndex = 0;
    sccCounter = 0;
    sccStack.clear();
    return ret;
}

void reduceSCCM() {
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
        eraseEdgeM(it.first, it.second, 3);
    }
    toBeErasedSCC.clear();
    sccIndex = 0;
    sccCounter = 0;
    sccStack.clear();
}


void doBasicReductionsR() {
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
            eraseNodeR(node);
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
            eraseNodeR(node);
        }
        change |= !oneDegreeNodes.empty();
        while (!oneDegreeNodes.empty()) {
            checkTime();
            int node = oneDegreeNodes.back();
            oneDegreeNodes.pop_back();
            bypassNodeR(node);
        }
    }
}

void doBasicReductionsM() {
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
            eraseNodeM(node);
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
            eraseNodeM(node);
        }
        change |= !oneDegreeNodes.empty();
        while (!oneDegreeNodes.empty()) {
            checkTime();
            int node = oneDegreeNodes.back();
            oneDegreeNodes.pop_back();
            bypassNodeM(node);
        }
    }
}

void findDfvsM() {
    checkTime();
    doBasicReductionsM();
    int edgesCount = edges.size();
    while (!availableNodesM.empty()) {
        checkTime();
        pair<int, long long> topNode = availableNodesM.top();
        availableNodesM.pop();
        if (!availableNode[topNode.first] ||
            topNode.second != getFitness(topNode.first)) {
            continue;
        }
        // if (!inDegreeDouble[topNode.first].empty()) {
        mergeNodeM(topNode.first);
        // } else {
        //   eraseNodeM(topNode.first);
        // }
        doBasicReductionsM();
        if (getElapsed() < SECONDS - 60 - 60 * 5) {
            if (edges.size() < edgesCount * 4 / 5) {
                edgesCount = edges.size();
                reduceSCCM();
                doBasicReductionsM();
                reduceCOREM();
                doBasicReductionsM();
                reduceDOMEM();
                doBasicReductionsM();
            }
        } else {
            if (getElapsed() < SECONDS - 60 - 60 * 3) {
                if (edges.size() < edgesCount * 3 / 4) {
                    edgesCount = edges.size();
                    reduceSCCM();
                    doBasicReductionsM();
                    reduceCOREM();
                    doBasicReductionsM();
                    //reduceDOMEM();
                    //doBasicReductionsM();
                }
            } else {
                if (getElapsed() < SECONDS - 60 - 60 * 1) {
                    if (edges.size() < edgesCount * 2 / 3) {
                        edgesCount = edges.size();
                        //reduceSCCM();
                        //doBasicReductionsM();
                        reduceCOREM();
                        doBasicReductionsM();
                        //reduceDOMEM();
                        //doBasicReductionsM();
                    }
                } else {
                    if (edges.size() < edgesCount * 1 / 2) {
                        edgesCount = edges.size();
                        //reduceSCCM();
                        //doBasicReductionsM();
                        reduceCOREM();
                        doBasicReductionsM();
                        //reduceDOMEM();
                        //doBasicReductionsM();
                    }
                }
            }
        }
    }
    lastFeedbackVertexSet = feedbackVertexSet;
    //cout << "0--" << feedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    if (bestFeedbackVertexSet.size() > feedbackVertexSet.size()) {
        bestFeedbackVertexSet = feedbackVertexSet;
    }
}

void loop() {
    bool change = true;
    while (change) {
        change = false;
        change |= reduceSCCR();
        doBasicReductionsR();
        change |= reduceCORER();
        doBasicReductionsR();
        change |= reduceDOMER();
        doBasicReductionsR();
    }
}

void findDfvsR() {
    checkTime();
    doBasicReductionsR();
    int edgesCount = edges.size();
    //   cout << "++" << candidatesNodes.size() << ' ' << fitnessType << '\n';
    currentErased = 0;
    int node = 0;
    loop();
    while (!availableNodesR.empty()) {
        checkTime();
        pair<int, long long> topNode = availableNodesR.top();
        availableNodesR.pop();
        if (!availableNode[topNode.first] ||
            topNode.second != getFitness(topNode.first)) {
            continue;
        }
        ++currentErased;
        if (currentErased == toBeAvoided || notInDFVS.count(topNode.first)) {
            // cout << topNode.first << ' ' << "**\n";
            mergeNodeR(topNode.first);
            if (currentErased == toBeAvoided) {
                node = topNode.first;
            }
        } else {
            eraseNodeR(topNode.first);
        }
        if (inDegreeDouble[topNode.first].empty()) {
            //   cout << "--" << candidatesNodes.size() << '\n';
        }
        doBasicReductionsR();
        if (getElapsed() < SECONDS - 60 - 60 * 6) {
            if (edges.size() < edgesCount * 4 / 5) {
                edgesCount = edges.size();
                loop();
            }
        } else {
            if (getElapsed() < SECONDS - 60 - 60 * 5) {
                if (edges.size() < edgesCount * 3 / 4) {
                    edgesCount = edges.size();
                    reduceSCCR();
                    // loop();
                }
            } else {
                if (getElapsed() < SECONDS - 60 - 60 * 3) {
                    if (edges.size() < edgesCount * 2 / 3) {
                        edgesCount = edges.size();
                        //   loop();
                    }
                } else {
                    if (getElapsed() < SECONDS - 60 - 60 * 1) {
                        if (edges.size() < edgesCount * 2 / 3) {
                            edgesCount = edges.size();
                            //     loop();
                        }
                    } else {
                        if (edges.size() < edgesCount * 1 / 2) {
                            edgesCount = edges.size();
                            //   loop();
                        }
                    }
                }
            }
        }
        feedbackVertexSet.insert(topNode.first);
    }
    lastFeedbackVertexSet = feedbackVertexSet;
    //cout << "______" << feedbackVertexSet.size() << '\n';
    if (bestFeedbackVertexSet.size() > feedbackVertexSet.size()) {
        if (toBeAvoided != -1) {
            notInDFVS.insert(node);
        }
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
    for (int i = 1; i < n; ++i) {
        bestFeedbackVertexSet.insert(i);
    }
    in.close();
    if (n == 0) {
        // empty testcase
        exit(0);
    }
}


void createInitialDfvsR() {
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
            availableNodesR.push(make_pair(i, getFitness(i)));
        }
    }
    //cout << candidatesNodes.size() << ' ';
    //reducePseudoCORE();
    //doBasicReductionsM();
    //cout << candidatesNodes.size() << '\n';
    findDfvsR();
    clearSets();
    initializeSets();
}

void createInitialDfvsM() {
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
            availableNodesM.push(make_pair(i, getFitness(i)));
        }
    }
    findDfvsM();
    clearSets();
    initializeSets();
}

vector<int> localDFVS;

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
    for (auto node : lastFeedbackVertexSet) {
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

    localDFVS.reserve(lastFeedbackVertexSet.size());
    for (auto it : lastFeedbackVertexSet) {
        checkTime();
        localDFVS.emplace_back(it);
    }
    sort(localDFVS.begin(), localDFVS.end(), [](const int i, const int j) {
        return inDegreeReducedSimple[i].size() + outDegreeReducedSimple[i].size() >
               inDegreeReducedSimple[j].size() + outDegreeReducedSimple[j].size();
    });
    while (!localDFVS.empty()) {
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
    //cout << ">?" << lastFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
}

vector<int> local;

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
    fitnessType = 2;
    for (auto i : candidatesNodes) {
        if (!checkNodeCanBeReduced(i)) {
            availableNodesR.push(make_pair(i, getFitness(i)));
        }
    }
    for (auto node : bestFeedbackVertexSet) {
        checkTime();
        feedbackVertexSetList.emplace_back(node);
    }
    checkTime();
    int size = bestFeedbackVertexSet.size();
    sort(feedbackVertexSetList.begin(), feedbackVertexSetList.end(), [](const int i, const int j) {
        return getFitness(i) > getFitness(j);
    });
    for (int i = size / 3; i < size - size / 3; ++i) {
        local.emplace_back(feedbackVertexSetList[i]);
    }
    unsigned seed = rand();
    shuffle(local.begin(), local.end(), default_random_engine(seed));
    size = local.size();
    //cout << "--" << size << '\n';
    int toBeErasedCounter = rand() % (size * 1 / 2 + 1);

    while (toBeErasedCounter--) {
        checkTime();
        int node = local.back();
        local.pop_back();
        eraseNodeR(node);
        doBasicReductionsR();
        feedbackVertexSet.insert(node);
    }
    findDfvsR();
    improveFeedbackVertexSet();
    if (feedbackVertexSet.size() < bestFeedbackVertexSet.size()) {
        bestFeedbackVertexSet = feedbackVertexSet;
    }
    clearSets();
    initializeSets();
    local.clear();
    feedbackVertexSetList.clear();
    //cout << "**" << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
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
    //cout << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    toBeAvoided = -1;
    for (fitnessType = 1; fitnessType <= 7; ++fitnessType) {
        checkTime();
        createInitialDfvsR();
        improveFeedbackVertexSet();
        // cout << "!!" << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    }
    for (alpha = 205; alpha <= 295; alpha += 5) {
        checkTime();
        fitnessType = 8;
        checkTime();
        createInitialDfvsR();
        improveFeedbackVertexSet();
    }
    for (toBeAvoided = 1; toBeAvoided <= 100; ++toBeAvoided) {
        checkTime();
        fitnessType = 2;
        checkTime();
        createInitialDfvsR();
        improveFeedbackVertexSet();
        // cout << "()" << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    }
    /*
    for (fitnessType = 1; fitnessType <= 7; ++fitnessType) {
        cout << "**-" << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    }
    */
    //cout << "(" << bestFeedbackVertexSet.size() + feedbackVertexSetReduced.size() << '\n';
    fitnessType = 1;
    checkTime();

    for (;;) {
        checkTime();
        doLocalSearch();
    }

    string
            path_output =
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
    //out.close();
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
    for (testNo = 1; testNo <= 1; testNo += 2) {
        //cout << testNo << ' ';
        solveTestcase();
        //cout << getElapsed() << '\n';
    }
    return 0;
}