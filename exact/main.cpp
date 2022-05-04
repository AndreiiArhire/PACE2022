#include <bits/stdc++.h>
#include <chrono>

using namespace std;

struct pair_hash {
    std::size_t operator()(const std::pair<int, int> &v) const {
        return 1LL * (1LL * v.first + 1LL * v.second * 666013) % 1000000007;
    }
};

std::chrono::time_point<std::chrono::high_resolution_clock> begin_;
int n, m, fitnessType, sccIndex, sccCounter, testNo, changed, firstTime;
string output, s;
vector<pair<pair<int, int>, int >> toBeErasedBypass;
vector<pair<pair<int, unordered_set<int>::iterator>, int> > stackTarjan;
vector<pair<int, int>> toBeErasedDOME, toBeErasedSCC;
unordered_set<pair<int, int>, pair_hash> currentEdges;
vector<unordered_set<int>> currentDegreeDouble, currentInDegreeSimple, currentOutDegreeSimple;
vector<bool> currentAvailableNode, inStack, visitedCORE;
vector<int> inNodes, outNodes, selfLoopNodes, zeroDegreeNodes, oneDegreeNodes, lowLevel, sccStack, currLevel, whichSCC, candidatesSorted;
unordered_set<int> candidatesSortedSet, cliqueCORE, currentCandidatesNodes, currentFeedbackVertexSet, bestFeedbackVertexSet, lastFeedbackVertexSet;
vector<int> toBeErasedSet;
unordered_set<int> cl2[2];
unordered_set<int> cl3[3];

auto cmp = [](pair<long long, int> a, pair<long long, int> b) {
    return a.first > b.first || (a.first == b.first && a.second > b.second);
};
set<pair<long long, int>, decltype(cmp)> availableNodes(cmp);


void doBasicReductions();

void loop();

void readData();

void solveTestcase();

void eraseEdge(int x, int y, int z);

void addEdge(int x, int y);

void initializeSets();

void eraseNode(int node);

void bypassNode(int node);

void runTarjan(int node);

bool checkNodeCanBeReduced(int node);

bool reduceCORE();

bool reduceDOME();

bool reduceSCC();

long long getFitness(int node);

double getElapsed();


void mergeNode(int node) {


    currentAvailableNode[node] = false;
    currentCandidatesNodes.erase(node);
    for (auto it :currentInDegreeSimple[node]) {

        toBeErasedBypass.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :currentDegreeDouble[node]) {

        toBeErasedBypass.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :currentOutDegreeSimple[node]) {

        toBeErasedBypass.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it :currentDegreeDouble[node]) {
        toBeErasedBypass.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it : toBeErasedBypass) {
        eraseEdge(it.first.first, it.first.second, it.second);
    }
    toBeErasedBypass.clear();
    for (auto it1 : inNodes) {
        for (auto it2 : outNodes) {
            addEdge(it1, it2);
            currentEdges.insert(make_pair(it1, it2));
            if (it1 == it2) {
                selfLoopNodes.emplace_back(it1);
            }
            checkNodeCanBeReduced(it2);
        }
        checkNodeCanBeReduced(it1);
    }
    inNodes.clear();
    outNodes.clear();
}

unordered_set<int> ll;

bool reduceDICLIQUE3() {
    bool ret;
    ll = currentCandidatesNodes;
    for (auto it : ll) {
        if (!currentAvailableNode[it]) {
            continue;
        }
        if (currentEdges.count(make_pair(it, it))) {
            continue;
        }
        if (!currentDegreeDouble[it].empty()) {
            continue;
        }
        for (auto it2 : currentInDegreeSimple[it]) {

            candidatesSortedSet.insert(it2);
        }
        for (auto it2 : currentOutDegreeSimple[it]) {

            candidatesSortedSet.insert(it2);
        }
        bool ok = true;
        for (auto &i : cl3) {

            if (candidatesSortedSet.empty()) {
                candidatesSortedSet.clear();
                for (auto &it2 : cl3) {

                    it2.clear();
                }
                ok = false;
                break;
            }
            int node = *candidatesSortedSet.begin();
            candidatesSortedSet.erase(node);
            i.insert(node);
            for (auto it2 : candidatesSortedSet) {

                if (currentEdges.count(make_pair(node, it2)) && currentEdges.count(make_pair(it2, node))) {
                    i.insert(it2);
                    toBeErasedSet.emplace_back(it2);
                }
            }
            for (auto it2 : toBeErasedSet) {

                candidatesSortedSet.erase(it2);
            }
            toBeErasedSet.clear();
        }
        if (!ok) {
            continue;
        }
        if (!candidatesSortedSet.empty()) {
            candidatesSortedSet.clear();
            for (auto &it2 : cl3) {

                it2.clear();
            }
            continue;
        }
        bool clOK[3] = {true, true, true}, okIsh = true;
        for (int i = 0; i < 3 && okIsh; ++i) {

            for (auto it3 : cl3[i]) {

                for (auto it4 : cl3[i]) {

                    if (it3 == it4) {
                        continue;
                    }
                    if (!currentEdges.count(make_pair(it3, it4)) || !currentEdges.count(make_pair(it4, it3))) {
                        clOK[i] = false;
                        break;
                    }
                }
                if (!clOK[i]) {
                    break;
                }
            }
            if (!clOK[i]) {
                okIsh = false;
                for (auto &it2 : cl3) {

                    it2.clear();
                }
                candidatesSortedSet.clear();
                break;
            }
        }
        if (!okIsh) {
            continue;
        }
        mergeNode(it);
        ret = true;
        for (auto &it2 : cl3) {

            it2.clear();
        }
        candidatesSortedSet.clear();
    }
    return ret;
}

unordered_set<int> set1;

bool reduceDICLIQUE22() {
    bool ret = false;
    ll = currentCandidatesNodes;
    for (auto it : ll) {

        if (!currentAvailableNode[it]) {
            continue;
        }
        if (currentEdges.count(make_pair(it, it))) {
            doBasicReductions();
            continue;
        }
        if (currentDegreeDouble[it].size() > 50 || currentInDegreeSimple[it].size() > 50 ||
            currentOutDegreeSimple[it].size() > 50) {
            continue;
        }
        bool doubleOK = true;
        for (auto it2 : currentDegreeDouble[it]) {
            for (auto it3 : currentDegreeDouble[it]) {
                if (it2 == it3) {
                    continue;
                }
                if (!currentEdges.count(make_pair(it2, it3)) || !currentEdges.count(make_pair(it3, it2))) {
                    doubleOK = false;
                    break;
                }
            }
            if (!doubleOK) {
                break;
            }
        }
        if (!doubleOK) {
            continue;
        }
        for (auto it2 : currentInDegreeSimple[it]) {
            bool isCliqueOne = true;
            for (auto it3 : currentDegreeDouble[it]) {
                if (!currentEdges.count(make_pair(it2, it3)) || !currentEdges.count(make_pair(it3, it2))) {
                    isCliqueOne = false;
                    break;
                }
            }
            if (!isCliqueOne) {
                cl2[1].insert(it2);
            } else {
                set1.insert(it2);
            }
        }
        for (auto it2 : currentOutDegreeSimple[it]) {
            bool isCliqueOne = true;
            for (auto it3 : currentDegreeDouble[it]) {
                if (!currentEdges.count(make_pair(it2, it3)) || !currentEdges.count(make_pair(it3, it2))) {
                    isCliqueOne = false;
                    break;
                }
            }
            if (!isCliqueOne) {
                cl2[1].insert(it2);
            } else {
                set1.insert(it2);
            }
        }
        bool simpleOK = true;
        for (auto it2 : cl2[1]) {
            for (auto it3 : cl2[1]) {
                if (!currentEdges.count(make_pair(it2, it3)) || !currentEdges.count(make_pair(it3, it2))) {
                    simpleOK = false;
                    continue;
                }
            }
            if (!simpleOK) {
                break;
            }
        }
        if (!simpleOK) {
            set1.clear();
            cl2[1].clear();
            continue;
        }
        bool change = true, diClique = true;
        while (change) {
            change = false;
            vector<int> toBeErased;
            for (auto it2 : set1) {
                bool ok[2] = {true, true};
                for (int i = 0; i < 2; ++i) {
                    for (auto it3 : cl2[i]) {
                        if (!currentEdges.count(make_pair(it2, it3)) || !currentEdges.count(make_pair(it3, it2))) {
                            ok[i] = false;
                            break;
                        }
                    }
                    if (!ok[i]) {
                        break;
                    }
                }
                if (ok[0] && ok[1]) {
                    continue;
                }
                if (!ok[0] && !ok[1]) {
                    diClique = false;
                    break;
                }
                change = true;
                ok[0] ? cl2[0].insert(it2) : cl2[1].insert(it2);
                toBeErased.emplace_back(it2);
            }
            while (!toBeErased.empty()) {
                set1.erase(toBeErased.back());
                toBeErased.pop_back();
            }
        }
        cl2[0].clear();
        cl2[1].clear();
        if (!diClique) {
            set1.clear();
            continue;
        }
        if (set1.empty()) {
            mergeNode(it);
            ret = true;
            continue;
        } else {
            continue;
        }
        vector<pair<int, int>> nodes;
        for (auto it2 : set1) {
            int counter = 0;
            for (auto it3 : set1) {
                if (currentEdges.count(make_pair(it2, it3)) && currentEdges.count(make_pair(it3, it2))) {
                    ++counter;
                }
            }
            nodes.emplace_back(it2, counter);
        }
        sort(nodes.begin(), nodes.end(),
             [](const pair<int, int> &i, const pair<int, int> &j) { return i.second < j.second; });
        for (auto it2 : set1) {
            if (it2 == nodes[0].first ||
                (currentEdges.count(make_pair(nodes[0].first, it2)) &&
                 currentEdges.count(make_pair(it2, nodes[0].first)))) {
                cl2[0].insert(it2);
            } else {
                cl2[1].insert(it2);
            }
        }
        set1.clear();
        for (int i = 0; i < 2 && diClique; ++i) {
            for (auto it2 : cl2[i]) {
                for (auto it3 : cl2[i]) {
                    if (!currentEdges.count(make_pair(it2, it3)) || !currentEdges.count(make_pair(it3, it2))) {
                        diClique = false;
                    }
                }
                if (!diClique) {
                    break;
                }
            }
        }
        cl2[0].clear();
        cl2[1].clear();
        nodes.clear();
        if (diClique) {
            mergeNode(it);
        }
    }

    return ret;
}

bool reduceDICLIQUE2() {
    bool ret = false;
    ll = currentCandidatesNodes;
    for (auto it : ll) {

        if (!currentAvailableNode[it]) {
            continue;
        }
        if (currentEdges.count(make_pair(it, it))) {
            continue;
        }
        for (auto it2 : currentInDegreeSimple[it]) {

            candidatesSortedSet.insert(it2);
        }
        for (auto it2 : currentOutDegreeSimple[it]) {

            candidatesSortedSet.insert(it2);
        }
        for (auto it2 : currentDegreeDouble[it]) {

            candidatesSortedSet.insert(it2);
        }
        bool ok = true;
        for (auto &i : cl2) {

            if (candidatesSortedSet.empty()) {
                candidatesSortedSet.clear();
                for (auto &it2 : cl2) {

                    it2.clear();
                }
                ok = false;
                break;
            }
            int node = *candidatesSortedSet.begin();
            candidatesSortedSet.erase(node);
            i.insert(node);
            for (auto it2 : candidatesSortedSet) {

                if (currentEdges.count(make_pair(node, it2)) && currentEdges.count(make_pair(it2, node))) {
                    i.insert(it2);
                    toBeErasedSet.emplace_back(it2);
                }
            }
            for (auto it2 : toBeErasedSet) {

                candidatesSortedSet.erase(it2);
            }
            toBeErasedSet.clear();
        }
        if (!ok) {
            continue;
        }
        if (!candidatesSortedSet.empty()) {
            candidatesSortedSet.clear();
            for (auto &it2 : cl2) {

                it2.clear();
            }
            continue;
        }
        bool cl1OK = true, cl2OK = true;
        for (auto it3 : cl2[0]) {

            for (auto it4 : cl2[0]) {

                if (it3 == it4) {
                    continue;
                }
                if (!currentEdges.count(make_pair(it3, it4)) || !currentEdges.count(make_pair(it4, it3))) {
                    cl1OK = false;
                    break;
                }
            }
            if (!cl1OK) {
                break;
            }
        }
        if (!cl1OK) {
            for (auto &it2 : cl2) {

                it2.clear();
            }
            candidatesSortedSet.clear();
            continue;
        }
        for (auto it3 : cl2[1]) {

            for (auto it4 : cl2[1]) {

                if (it3 == it4) {
                    continue;
                }
                if (!currentEdges.count(make_pair(it3, it4)) || !currentEdges.count(make_pair(it4, it3))) {
                    cl2OK = false;
                    break;
                }
            }
            if (!cl2OK) {
                break;
            }
        }
        if (!cl2OK) {
            for (auto &it2 : cl2) {

                it2.clear();
            }
            candidatesSortedSet.clear();
            continue;
        }
        bool sw0 = false, sw1 = false;
        for (auto it2 : currentDegreeDouble[it]) {

            if (cl2[0].count(it2)) {
                sw0 = true;
            }
            if (cl2[1].count(it2)) {
                sw1 = true;
            }
        }
        if (sw0 && sw1) {
            for (auto &it2 : cl2) {

                it2.clear();
            }
            candidatesSortedSet.clear();
            continue;
        }
        mergeNode(it);
        ret = true;
        for (auto &it2 : cl2) {

            it2.clear();
        }
        candidatesSortedSet.clear();
    }
    return ret;
}

bool reduceDICLIQUE1() {
    bool ret = false;
    for (auto it : currentCandidatesNodes) {

        candidatesSorted.emplace_back(it);
    }
    sort(candidatesSorted.begin(), candidatesSorted.end(), [](const int i, const int j) {
        return currentDegreeDouble[i].size() + min(currentInDegreeSimple[i].size(), currentOutDegreeSimple[i].size()) <
               currentDegreeDouble[j].size() + min(currentInDegreeSimple[j].size(), currentOutDegreeSimple[j].size());
    });
    for (auto it : candidatesSorted) {

        if (!currentAvailableNode[it]) {
            continue;
        }
        if (currentEdges.count(make_pair(it, it))) {
            doBasicReductions();
            continue;
        }
        bool doubleOK = true;
        for (auto it1 : currentDegreeDouble[it]) {

            for (auto it2 : currentDegreeDouble[it]) {

                if (it1 == it2) {
                    continue;
                }
                if (!currentEdges.count(make_pair(it1, it2)) || !currentEdges.count(make_pair(it2, it1))) {
                    doubleOK = false;
                    break;
                }
            }
            if (!doubleOK) {
                break;
            }
        }
        if (!doubleOK) {
            continue;
        }
        if (currentInDegreeSimple[it].empty() || currentOutDegreeSimple[it].empty()) {
            mergeNode(it);
            ret = true;
            continue;
        }
        continue;
        bool inOK = true;
        for (auto it1 : currentInDegreeSimple[it]) {

            for (auto it2 : currentInDegreeSimple[it]) {

                if (!currentEdges.count(make_pair(it1, it2)) || !currentEdges.count(make_pair(it2, it1))) {
                    inOK = false;
                    break;
                }
            }
            if (!inOK) {
                break;
            }
        }
        if (inOK) {
            for (auto it1 : currentInDegreeSimple[it]) {

                for (auto it2 : currentDegreeDouble[it]) {

                    if (!currentEdges.count(make_pair(it1, it2)) || !currentEdges.count(make_pair(it2, it1))) {
                        inOK = false;
                        break;
                    }
                }
                if (!inOK) {
                    break;
                }
            }
        }
        if (inOK) {
            mergeNode(it);
            ret = true;
            continue;
        }
        bool outOK = false;
        outOK = true;
        for (auto it1 : currentOutDegreeSimple[it]) {

            for (auto it2 : currentOutDegreeSimple[it]) {

                if (!currentEdges.count(make_pair(it1, it2)) || !currentEdges.count(make_pair(it2, it1))) {
                    outOK = false;
                    break;
                }
            }
            if (!outOK) {
                break;
            }
        }
        if (outOK) {
            for (auto it1 : currentOutDegreeSimple[it]) {

                for (auto it2 : currentDegreeDouble[it]) {

                    if (!currentEdges.count(make_pair(it1, it2)) || !currentEdges.count(make_pair(it2, it1))) {
                        outOK = false;
                        break;
                    }
                }
                if (!outOK) {
                    break;
                }
            }
        }
        if (outOK) {
            mergeNode(it);
            ret = true;
            continue;
        }
    }
    candidatesSorted.clear();
    return ret;
}


signed main() {
    //std::ios::sync_with_stdio(false);
    //cin.tie();
    //cout.tie();
    for (testNo = 19; testNo <= 19; testNo += 2) {
        solveTestcase();
    }
    return 0;
}

bool checkNodeCanBeReduced(int node) {

    if (!currentAvailableNode[node]) {
        return false;
    }
    if ((currentInDegreeSimple[node].empty() && currentDegreeDouble[node].empty()) ||
        (currentOutDegreeSimple[node].empty() && currentDegreeDouble[node].empty())) {
        zeroDegreeNodes.emplace_back(node);
        return true;
    }
    if (currentEdges.count(make_pair(node, node))) {
        selfLoopNodes.emplace_back(node);
        return true;
    }
    if (currentInDegreeSimple[node].size() + currentDegreeDouble[node].size() == 1 ||
        currentOutDegreeSimple[node].size() + currentDegreeDouble[node].size() == 1) {
        oneDegreeNodes.emplace_back(node);
        return true;
    }
    return false;
}

void eraseEdge(int x, int y, int z) {

    currentEdges.erase(make_pair(x, y));
    currentOutDegreeSimple[x].erase(y);
    currentDegreeDouble[x].erase(y);
    currentInDegreeSimple[y].erase(x);
    currentDegreeDouble[y].erase(x);
    if (currentEdges.count(make_pair(y, x))) {
        currentEdges.erase(make_pair(y, x));
        currentDegreeDouble[x].erase(y);
        currentInDegreeSimple[x].erase(y);
        currentDegreeDouble[y].erase(x);
        currentOutDegreeSimple[y].erase(x);
    }
    checkNodeCanBeReduced(x);
    checkNodeCanBeReduced(y);

}

void addEdge(int x, int y) {

    if (!currentEdges.count(make_pair(y, x)) || x == y) {
        currentEdges.insert(make_pair(x, y));
        currentInDegreeSimple[y].insert(x);
        currentOutDegreeSimple[x].insert(y);
    } else {
        currentEdges.insert(make_pair(x, y));
        currentInDegreeSimple[x].erase(y);
        currentOutDegreeSimple[y].erase(x);
        currentDegreeDouble[x].insert(y);
        currentDegreeDouble[y].insert(x);
    }
}

void initializeSets() {

    visitedCORE.resize(n + 1, false);
    lowLevel.resize(n + 1, 0);
    currLevel.resize(n + 1, 0);
    inStack.resize(n + 1, false);
    whichSCC.resize(n + 1, 0);
    currentAvailableNode.resize(n + 1, true);
    currentInDegreeSimple.resize(n + 1, unordered_set<int>());
    currentDegreeDouble.resize(n + 1, unordered_set<int>());
    currentOutDegreeSimple.resize(n + 1, unordered_set<int>());
}

void eraseNode(int node) {

    if (!currentAvailableNode[node]) {
        return;
    }
    currentAvailableNode[node] = false;
    currentCandidatesNodes.erase(node);
    while (!currentInDegreeSimple[node].empty()) {

        int x = *currentInDegreeSimple[node].begin();
        currentInDegreeSimple[node].erase(currentInDegreeSimple[node].begin());
        currentOutDegreeSimple[x].erase(node);
        currentEdges.erase(make_pair(x, node));
        checkNodeCanBeReduced(x);
    }
    while (!currentDegreeDouble[node].empty()) {

        int x = *currentDegreeDouble[node].begin();
        currentDegreeDouble[node].erase(currentDegreeDouble[node].begin());
        currentDegreeDouble[x].erase(node);
        currentEdges.erase(make_pair(x, node));
        currentEdges.erase(make_pair(node, x));
        checkNodeCanBeReduced(x);
    }
    while (!currentOutDegreeSimple[node].empty()) {

        int x = *currentOutDegreeSimple[node].begin();
        currentOutDegreeSimple[node].erase(currentOutDegreeSimple[node].begin());
        currentInDegreeSimple[x].erase(node);
        currentEdges.erase(make_pair(node, x));
        checkNodeCanBeReduced(x);
    }
}

void bypassNode(int node) {

    if (!currentAvailableNode[node]) {
        return;
    }
    if (currentEdges.count(make_pair(node, node))) {
        selfLoopNodes.emplace_back(node);
        return;
    }
    if ((currentInDegreeSimple[node].empty() && currentDegreeDouble[node].empty()) ||
        (currentOutDegreeSimple[node].empty() && currentDegreeDouble[node].empty())) {
        zeroDegreeNodes.emplace_back(node);
        return;
    }
    if (currentInDegreeSimple[node].size() + currentDegreeDouble[node].size() != 1 &&
        currentOutDegreeSimple[node].size() + currentDegreeDouble[node].size() != 1) {
        return;
    }
    currentAvailableNode[node] = false;
    currentCandidatesNodes.erase(node);
    for (auto it :currentInDegreeSimple[node]) {

        toBeErasedBypass.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :currentDegreeDouble[node]) {

        toBeErasedBypass.emplace_back(make_pair(it, node), 0);
        inNodes.emplace_back(it);
    }
    for (auto it :currentOutDegreeSimple[node]) {

        toBeErasedBypass.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it :currentDegreeDouble[node]) {

        toBeErasedBypass.emplace_back(make_pair(node, it), 0);
        outNodes.emplace_back(it);
    }
    for (auto it : toBeErasedBypass) {

        eraseEdge(it.first.first, it.first.second, it.second);
    }
    toBeErasedBypass.clear();
    for (auto it1 : inNodes) {

        for (auto it2 : outNodes) {

            addEdge(it1, it2);
            currentEdges.insert(make_pair(it1, it2));
            if (it1 == it2) {
                selfLoopNodes.emplace_back(it1);
            }
            checkNodeCanBeReduced(it2);
        }
        checkNodeCanBeReduced(it1);
    }
    inNodes.clear();
    outNodes.clear();
}

void runTarjan(int node) {

    stackTarjan.emplace_back(make_pair(node, currentOutDegreeSimple[node].begin()), 0);
    for (; !stackTarjan.empty();) {

        if (stackTarjan.back().first.second == currentOutDegreeSimple[stackTarjan.back().first.first].begin() &&
            !stackTarjan.back().second) {
            lowLevel[stackTarjan.back().first.first] = ++sccIndex;
            currLevel[stackTarjan.back().first.first] = sccIndex;
            inStack[stackTarjan.back().first.first] = true;
            sccStack.emplace_back(stackTarjan.back().first.first);
        }

        if (stackTarjan.back().first.second != currentOutDegreeSimple[stackTarjan.back().first.first].end() &&
            currentEdges.count(make_pair(*stackTarjan.back().first.second, stackTarjan.back().first.first))) {
            stackTarjan.back().first.second++;
            continue;
        }

        if (stackTarjan.back().second &&
            stackTarjan.back().first.second != currentOutDegreeSimple[stackTarjan.back().first.first].end()) {
            lowLevel[stackTarjan.back().first.first] = min(
                    lowLevel[stackTarjan.back().first.first],
                    lowLevel[*stackTarjan.back().first.second]);
            stackTarjan.back().first.second++;
            stackTarjan.back().second = 0;
            continue;
        }

        if (stackTarjan.back().first.second != currentOutDegreeSimple[stackTarjan.back().first.first].end()) {
            if (!currLevel[*stackTarjan.back().first.second]) {
                stackTarjan.back().second = 1;
                stackTarjan.emplace_back(make_pair(*stackTarjan.back().first.second,
                                                   currentOutDegreeSimple[*stackTarjan.back().first.second].begin()),
                                         0);
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

bool reduceCORE() {
    for (auto it : currentCandidatesNodes) {
        visitedCORE[it] = false;
    }
    candidatesSorted.reserve(currentCandidatesNodes.size());
    for (auto node : currentCandidatesNodes) {

        candidatesSorted.emplace_back(node);
    }

    sort(candidatesSorted.begin(), candidatesSorted.end(), [](const int i, const int j) {
        return currentDegreeDouble[i].size() < currentDegreeDouble[j].size() ||
               (currentDegreeDouble[i].size() == currentDegreeDouble[j].size() &&
                min(currentInDegreeSimple[i].size(), currentOutDegreeSimple[i].size()) <
                min(currentInDegreeSimple[j].size(), currentOutDegreeSimple[j].size()));
    });

    bool ret = false;
    for (auto node : candidatesSorted) {
        if (visitedCORE[node]) continue;
        if (!currentOutDegreeSimple[node].empty() && !currentInDegreeSimple[node].empty()) {
            continue;
        }
        for (auto it : currentDegreeDouble[node]) {
            cliqueCORE.insert(it);
        }
        bool isCore = true;
        if (cliqueCORE.empty()) {
            isCore = false;
        }
        cliqueCORE.insert(node);
        for (auto it : cliqueCORE) {
            int sz = 1;
            if (isCore) {
                for (auto it3 : currentDegreeDouble[it]) {
                    if (cliqueCORE.count(it3)) {
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
                eraseNode(it);
                if (it != node) {
                    currentFeedbackVertexSet.insert(it);
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

    for (auto node : currentCandidatesNodes) {

        for (auto it : currentOutDegreeSimple[node]) {

            if (currentInDegreeSimple[it].size() >= currentInDegreeSimple[node].size() &&
                currentInDegreeSimple[node].size() <= 50) {
                bool status = true;
                for (auto it2 : currentInDegreeSimple[node]) {

                    if (!currentInDegreeSimple[it].count(it2)) {
                        status = false;
                        break;
                    }
                }
                if (status) {
                    toBeErasedDOME.emplace_back(node, it);
                    continue;
                }
            }
            if (currentOutDegreeSimple[node].size() >= currentOutDegreeSimple[it].size() &&
                currentOutDegreeSimple[it].size() <= 50) {
                bool status = true;
                for (auto it2 : currentOutDegreeSimple[it]) {

                    if (!currentOutDegreeSimple[node].count(it2)) {
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

        eraseEdge(it.first, it.second, 3);
    }
    toBeErasedDOME.clear();

    return ret;
}

bool reduceSCC() {

    sccIndex = 0;
    sccCounter = 0;
    for (auto it : currentCandidatesNodes) {

        lowLevel[it] = 0;
        currLevel[it] = 0;
        inStack[it] = false;
    }
    for (auto it : currentCandidatesNodes) {

        if (!currLevel[it]) {

            runTarjan(it);
        }
    }
    for (auto it : currentEdges) {

        if (currentEdges.count(make_pair(it.second, it.first))) continue;
        if (whichSCC[it.first] != whichSCC[it.second]) {
            toBeErasedSCC.emplace_back(it);
        }
    }
    for (auto it : toBeErasedSCC) {

        eraseEdge(it.first, it.second, 3);
    }
    bool ret = !toBeErasedSCC.empty();
    toBeErasedSCC.clear();
    sccIndex = 0;
    sccCounter = 0;
    sccStack.clear();
    return ret;
}


void findDFVS() {

    doBasicReductions();
    int edgesCount = currentEdges.size();
    loop();
    int cnt = 0;
    while (!availableNodes.empty()) {

        pair<int, long long> topNode = make_pair((*availableNodes.begin()).second, (*availableNodes.begin()).first);
        availableNodes.erase(availableNodes.begin());
        if (!currentCandidatesNodes.count(topNode.first)) {
            continue;
        }
        ++cnt;
        eraseNode(topNode.first);
        doBasicReductions();
        loop();
        currentFeedbackVertexSet.insert(topNode.first);
    }
    lastFeedbackVertexSet = currentFeedbackVertexSet;
    if (bestFeedbackVertexSet.size() > currentFeedbackVertexSet.size()) {
        changed = (int) bestFeedbackVertexSet.size() - (int) currentFeedbackVertexSet.size();
        bestFeedbackVertexSet = currentFeedbackVertexSet;
    } else {
        changed = 0;
    }
}

void doBasicReductions() {

    bool change = true;
    while (change) {

        change = false;
        change |= !selfLoopNodes.empty();
        while (!selfLoopNodes.empty()) {

            int node = selfLoopNodes.back();
            selfLoopNodes.pop_back();
            if (!currentEdges.count(make_pair(node, node))) {
                continue;
            }
            eraseNode(node);
            currentFeedbackVertexSet.insert(node);
        }
        change |= !zeroDegreeNodes.empty();
        while (!zeroDegreeNodes.empty()) {

            int node = zeroDegreeNodes.back();
            zeroDegreeNodes.pop_back();
            if ((!currentDegreeDouble[node].empty() || !currentInDegreeSimple[node].empty()) &&
                (!currentOutDegreeSimple[node].empty() || !currentDegreeDouble[node].empty())) {
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

void loop() {
    bool change = true;
    int count = 0;
    while (change) {
        //cout << "()" << currentEdges.size() << '\n';
        change = false;
        doBasicReductions();
        change |= reduceSCC();
        doBasicReductions();
        change |= reduceCORE();
        doBasicReductions();
        change |= reduceDOME();
        doBasicReductions();
    }
}


void readData() {
    string path_input = R"(C:\Users\andre\OneDrive\Desktop\pace\exact_public\e_)";
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

double getElapsed() {
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin_);
    double sec = elapsed.count() * 1e-9;
    return sec;
}

long long getFitness(int node) {
    if (fitnessType == 1) {
        return 1LL * currentDegreeDouble[node].size() * (long long) 1e11 +
               1LL * (currentInDegreeSimple[node].size()) * (currentOutDegreeSimple[node].size());
    }
    if (fitnessType == 2) {
        return 1LL * (currentInDegreeSimple[node].size() + currentDegreeDouble[node].size()) *
               (currentOutDegreeSimple[node].size() + currentDegreeDouble[node].size());
    }
    if (fitnessType == 3) {
        return 1LL * (currentInDegreeSimple[node].size() + 2 * currentDegreeDouble[node].size()) *
               (currentOutDegreeSimple[node].size() + 2 * currentDegreeDouble[node].size());
    }
    return 0;
}

double getRating(int node) {
    return 1.e11 * currentDegreeDouble[node].size() +
           1. * currentInDegreeSimple[node].size() *
           currentOutDegreeSimple[node].size();
}

int getBound() {
    int current = currentFeedbackVertexSet.size();
    while (!currentCandidatesNodes.empty()) {
        int nextNode = -1;
        double value = -1e18;
        for (auto node : currentCandidatesNodes) {
            if (!currentDegreeDouble[node].empty() && getRating(node) > value) {
                value = getRating(node);
                nextNode = node;
            }
        }
        if (nextNode == -1) {
            return (int) currentFeedbackVertexSet.size() - current + 1;
        }
        set<int> clique;
        clique.insert(nextNode);
        vector<int> nodes;
        for (auto it : currentDegreeDouble[nextNode]) {
            nodes.emplace_back(it);
        }
        sort(nodes.begin(), nodes.end(), [](const int &i, const int &j) { return getRating(i) > getRating(j); });
        for (auto it : nodes) {
            bool ok = true;
            for (auto it2 : clique) {
                if (!currentEdges.count(make_pair(it, it2)) || !currentEdges.count(make_pair(it2, it))) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                clique.insert(it);
            }
        }
        for (auto it : clique) {
            if (it != nextNode) {
                currentFeedbackVertexSet.insert(it);
            }
            eraseNode(it);
        }
        /*
        vector<int> nodes = {nextNode};
        int bestNeighbour = -1;
        double bestValue = -1e18;
        for (auto it : currentDegreeDouble[nextNode]) {
            if (getRating(it) > bestValue) {
                bestValue = getRating(it);
                bestNeighbour = it;
            }
        }
        nodes.emplace_back(bestNeighbour);
        currentFeedbackVertexSet.insert(nextNode);
        while (!nodes.empty()) {
            eraseNode(nodes.back());
            nodes.pop_back();
        }
        */
        loop();
    }
    return (int) currentFeedbackVertexSet.size() - current;
}

void branchAndBound(int nodeX, const string &type, unordered_set<int> candidatesNodes,
                    vector<unordered_set<int>> inDegreeSimple, vector<unordered_set<int>> outDegreeSimple,
                    vector<unordered_set<int>> degreeDouble, unordered_set<pair<int, int>, pair_hash> edges,
                    vector<bool> availableNode, unordered_set<int> feedbackVertexSet) {

    ///////////////
    currentCandidatesNodes = candidatesNodes;
    currentInDegreeSimple = inDegreeSimple;
    currentOutDegreeSimple = outDegreeSimple;
    currentDegreeDouble = degreeDouble;
    currentEdges = edges;
    currentAvailableNode = availableNode;
    currentFeedbackVertexSet = feedbackVertexSet;
    ///////////////
    if (type == "erase") {
        eraseNode(nodeX);
        currentFeedbackVertexSet.insert(nodeX);
    } else {
        mergeNode(nodeX);
    }
    loop();
    ///////////////
    candidatesNodes = currentCandidatesNodes;
    inDegreeSimple = currentInDegreeSimple;
    outDegreeSimple = currentOutDegreeSimple;
    degreeDouble = currentDegreeDouble;
    edges = currentEdges;
    availableNode = currentAvailableNode;
    feedbackVertexSet = currentFeedbackVertexSet;
    ///////////////
    if (candidatesNodes.empty()) {
        if (bestFeedbackVertexSet.size() > feedbackVertexSet.size()) {
            bestFeedbackVertexSet = feedbackVertexSet;
        }
        return;
    }
    int size = feedbackVertexSet.size();
    if (getBound() + size >= bestFeedbackVertexSet.size()) {
        return;
    }
    currentCandidatesNodes = candidatesNodes;
    currentInDegreeSimple = inDegreeSimple;
    currentOutDegreeSimple = outDegreeSimple;
    currentDegreeDouble = degreeDouble;
    currentEdges = edges;
    currentAvailableNode = availableNode;
    currentFeedbackVertexSet = feedbackVertexSet;
    int nextNode;
    long long maxValue = -1;
    for (auto it : candidatesNodes) {
        if (getFitness(it) > maxValue) {
            maxValue = getFitness(it);
            nextNode = it;
        }
    }
    ///////////////
    branchAndBound(nextNode, "erase", candidatesNodes, inDegreeSimple, outDegreeSimple, degreeDouble, edges,
                   availableNode, feedbackVertexSet);
    branchAndBound(nextNode, "merge", candidatesNodes, inDegreeSimple, outDegreeSimple, degreeDouble, edges,
                   availableNode, feedbackVertexSet);
}

void solveTestcase() {
    begin_ = std::chrono::high_resolution_clock::now();
    readData();
    for (int i = 1; i <= n; ++i) {
        checkNodeCanBeReduced(i);
    }
    for (int i = 1; i <= n; ++i) {
        currentCandidatesNodes.insert(i);
    }
    loop();


    auto candidatesNodes = currentCandidatesNodes;
    auto inDegreeSimple = currentInDegreeSimple;
    auto outDegreeSimple = currentOutDegreeSimple;
    auto degreeDouble = currentDegreeDouble;
    auto edges = currentEdges;
    auto availableNode = currentAvailableNode;
    auto feedbackVertexSet = currentFeedbackVertexSet;
    for (auto it : candidatesNodes) {
        availableNodes.insert(make_pair(getFitness(it), it));
    }
    cout << candidatesNodes.size() << '\n';
    findDFVS();
    currentCandidatesNodes = candidatesNodes;
    currentInDegreeSimple = inDegreeSimple;
    currentOutDegreeSimple = outDegreeSimple;
    currentDegreeDouble = degreeDouble;
    currentEdges = edges;
    currentAvailableNode = availableNode;
    currentFeedbackVertexSet = feedbackVertexSet;
    ///////////
    if (currentCandidatesNodes.empty()) {
        for (auto it : currentFeedbackVertexSet) {
            cout << it << '\n';
        }
        exit(0);
    }
    int nextNode;
    long long maxValue = -1;
    for (auto it : currentCandidatesNodes) {
        if (getFitness(it) > maxValue) {
            maxValue = getFitness(it);
            nextNode = it;
        }
    }
    branchAndBound(nextNode, "erase", currentCandidatesNodes, currentInDegreeSimple, currentOutDegreeSimple,
                   currentDegreeDouble,
                   currentEdges,
                   currentAvailableNode,
                   currentFeedbackVertexSet);
    branchAndBound(nextNode, "merge", currentCandidatesNodes, currentInDegreeSimple, currentOutDegreeSimple,
                   currentDegreeDouble,
                   currentEdges,
                   currentAvailableNode,
                   currentFeedbackVertexSet);

    string path_out =
            R"(C:\Users\andre\OneDrive\Desktop\PACE2022\exact-results\grader_test)" +
            to_string(71) + ".out";
    ofstream out(path_out);
    cout << bestFeedbackVertexSet.size() << '\n';
    out << bestFeedbackVertexSet.size() << '\n';
    for (auto it : bestFeedbackVertexSet) {
        out << it << '\n';
    }
}