#include <bits/stdc++.h>
#include <chrono>

using namespace std;

set<pair<int, int>> edges;
vector<vector<int>> ctc;
const int NR_SECONDS = 600 - 5;
vector<int> used, ctc_ind, st, used_pie, not_piv, individual_gene;
int n, nr_ctc, m, initial_population_size = 200, number_of_generations = 20, number_of_offsprings = 2000;
time_t c_start;
set<int> candidates_nodes_set;
vector<int> candidates_nodes, posInCandidatesNodes;
vector<set<int>> in_degree, out_degree;
vector<vector<int>> in_degree_, out_degree_;
set<int> feedback_vertex_set;
vector<vector<int>> femaleInitialGeneration, maleInitialGeneration;
vector<double> phi_males, phi_females, probability_males, probability_females;

ofstream out2("output.txt");

void add_edge(int x, int y) {
    /*O(lgN)*/
    edges.insert(make_pair(x, y));
    in_degree[y].insert(x);
    out_degree[x].insert(y);
}


void erase_edge(int x, int y) {
    /*O(lgN)*/
    edges.erase(make_pair(x, y));
    in_degree[y].erase(x);
    out_degree[x].erase(y);
}

bool erase_0in_out_degree_nodes() {
    /*O((N+M)lgN)*/
    bool ret = false;
    vector<int> new_candidates_nodes;
    for (auto i : candidates_nodes) {
        if (!in_degree[i].empty() && !out_degree[i].empty()) {
            new_candidates_nodes.emplace_back(i);
            continue;
        }
        vector<pair<int, int>> to_be_erased;
        for (auto it : in_degree[i]) {
            to_be_erased.emplace_back(it, i);
        }
        for (auto it : out_degree[i]) {
            to_be_erased.emplace_back(i, it);
        }
        for (auto it : to_be_erased) {
            erase_edge(it.first, it.second);
        }
    }
    ret |= new_candidates_nodes.size() != candidates_nodes.size();
    candidates_nodes = new_candidates_nodes;
    return ret;
}

bool erase_1in_out_degree_nodes() {
    /*O((N+M)lgN)*/
    bool ret = false;
    vector<int> new_candidates_nodes;
    for (auto i : candidates_nodes) {
        if ((in_degree[i].size() != 1 && out_degree[i].size() != 1) || edges.count(make_pair(i, i))) {
            new_candidates_nodes.emplace_back(i);
            continue;
        }
        vector<pair<int, int>> to_be_erased;
        if (in_degree[i].size() == 1) {
            int node = *in_degree[i].begin();
            to_be_erased.emplace_back(node, i);
            for (auto it : out_degree[i]) {
                to_be_erased.emplace_back(i, it);
            }
            for (auto it : out_degree[i]) {
                add_edge(node, it);
            }
        }
        if (out_degree[i].size() == 1) {
            int node = *out_degree[i].begin();
            to_be_erased.emplace_back(i, node);
            for (auto it : in_degree[i]) {
                to_be_erased.emplace_back(it, i);
            }
            for (auto it : in_degree[i]) {
                add_edge(it, node);
            }
        }
        for (auto it : to_be_erased) {
            erase_edge(it.first, it.second);
        }
    }
    ret |= new_candidates_nodes.size() != candidates_nodes.size();
    candidates_nodes = new_candidates_nodes;
    return ret;
}

bool erase_CORE_nodes() {
    /*O(NlgN+M)*/
    bool ret = false;
    for (auto i : candidates_nodes) not_piv[i] = 0;
    for (auto it : edges) {
        if (!edges.count(make_pair(it.second, it.first))) {
            not_piv[it.first] = 1;
            not_piv[it.second] = 1;
        }
    }
    sort(candidates_nodes.begin(), candidates_nodes.end(), [](const int i, const int j) {
        return in_degree[i].size() + out_degree[i].size() < in_degree[j].size() + out_degree[j].size();
    });

    for (auto it : candidates_nodes) {
        if (not_piv[it]) continue;
        set<int> clique;
        clique.insert(it);
        for (auto it2 : out_degree[it]) {
            clique.insert(it2);
        }
        int is_core = 1;
        for (auto it2 : clique) {
            int sz = 1;
            for (auto it3 : out_degree[it2]) {
                if (clique.count(it3)) {
                    sz++;
                }
            }
            if (sz != clique.size()) {
                is_core = 0;
            }
            not_piv[it2] = 1;
        }
        vector<pair<int, int>> to_be_erased;
        if (is_core == 1) {
            for (auto it2 : clique) {
                feedback_vertex_set.insert(it2);
                for (auto it3 : in_degree[it2]) {
                    to_be_erased.emplace_back(it3, it2);
                }
                for (auto it3 : out_degree[it2]) {
                    to_be_erased.emplace_back(it2, it3);
                }
            }
            feedback_vertex_set.erase(it);
        }
        for (auto it2 : to_be_erased) {
            erase_edge(it2.first, it2.second);
        }
    }
    vector<int> new_candidates_nodes;
    for (auto i: candidates_nodes) {
        if (!feedback_vertex_set.count(i)) {
            new_candidates_nodes.emplace_back(i);
        }
    }
    ret |= candidates_nodes.size() != new_candidates_nodes.size();
    candidates_nodes = new_candidates_nodes;
    return ret;
}

bool erase_DOM_edges() {
    /*O(N*M)*/
    bool ret = false;
    vector<pair<int, int>> to_be_erased;
    for (auto it : edges) {
        int dome = 1;
        for (auto node : in_degree[it.first]) {
            if (!edges.count(make_pair(it.first, node)) && !edges.count(make_pair(node, it.second))) {
                dome = 0;
                break;
            }
        }
        if (!dome) {
            dome = 1;
            for (auto node : out_degree[it.second]) {
                if (!edges.count(make_pair(node, it.second)) && !edges.count(make_pair(it.first, node))) {
                    dome = 0;
                    break;
                }
            }
        }
        if (dome) {
            to_be_erased.emplace_back(it);
        }
    }
    ret |= (bool) (to_be_erased.size());
    for (auto it : to_be_erased) {
        edges.erase(it);
    }
    return ret;
}


bool erase_self_loop_nodes() {
    /*O((N+M)lgN)*/
    bool ret = false;
    vector<int> new_candidates_nodes;
    for (auto i : candidates_nodes) {
        if (!edges.count(make_pair(i, i))) {
            new_candidates_nodes.emplace_back(i);
            continue;
        }
        vector<pair<int, int>> to_be_erased;
        feedback_vertex_set.insert(i);
        for (auto it : in_degree[i]) {
            to_be_erased.emplace_back(it, i);
        }
        for (auto it : out_degree[i]) {
            to_be_erased.emplace_back(i, it);
        }
        for (auto it : to_be_erased) {
            erase_edge(it.first, it.second);
        }
    }
    ret |= new_candidates_nodes.size() != candidates_nodes.size();
    candidates_nodes = new_candidates_nodes;
    return ret;
}


void loop() {
    bool change = true;
    while (change) {
        change = false;
        change |= erase_self_loop_nodes();
        change |= erase_0in_out_degree_nodes();
        change |= erase_1in_out_degree_nodes();
    }
}

void dfs(int nod) {
    used[nod] = 1;
    for (int i : out_degree[nod]) {
        if (!used[i] && !in_degree[nod].count(i)) dfs(i);
    }
    st.emplace_back(nod);
}


void dfs_t(int nod) {
    used[nod] = 2;
    for (int i : in_degree[nod]) {
        if (used[i] != 2 && !out_degree[nod].count(i)) dfs_t(i);
    }
    ctc_ind[nod] = nr_ctc;
    ctc[nr_ctc].emplace_back(nod);
}


bool erase_SCC_edges() {
    /*O((N+M)lgN)*/
    bool ret = false;
    nr_ctc = 0;
    ctc.clear();
    ctc.resize(n + 1, vector<int>());
    for (auto i : candidates_nodes) used[i] = 0;
    for (auto i : candidates_nodes) if (!used[i]) dfs(i);
    while (!st.empty()) {
        int nod = st.back();
        if (used[nod] != 2) dfs_t(nod), ++nr_ctc;
        st.pop_back();
    }
    vector<pair<int, int>> to_be_erased;
    for (auto it : edges) {
        if (edges.count(make_pair(it.second, it.first))) continue;
        if (ctc_ind[it.first] != ctc_ind[it.second]) {
            to_be_erased.emplace_back(it);
            ret = true;
        }
    }
    ret |= (bool) (to_be_erased.size());
    for (auto it : to_be_erased) {
        erase_edge(it.first, it.second);
    }
    return ret;
}

void contract_graph() {
    /*O(N(N+M)lgN*/
    for (int i = 1; i <= n; ++i) {
        candidates_nodes.emplace_back(i);
    }
    loop();
    erase_SCC_edges();
    loop();
    erase_CORE_nodes();
    loop();
    erase_DOM_edges();
    loop();

    for (auto it : candidates_nodes) {
        for (auto it2 : in_degree[it]) {
            in_degree_[it].emplace_back(it2);
        }
        for (auto it2 : out_degree[it]) {
            out_degree_[it].emplace_back(it2);
        }
    }

}

void set_size() {
    /*O(N*lgN)*/
    nr_ctc = 0;
    posInCandidatesNodes.resize(n + 1, 0);
    individual_gene.resize(n + 1, 0);
    ctc.resize(n + 1, vector<int>());
    in_degree.resize(n + 1, set<int>());
    in_degree_.resize(n + 1, vector<int>());
    out_degree.resize(n + 1, set<int>());
    out_degree_.resize(n + 1, vector<int>());
    used.resize(n + 1, 0);
    not_piv.resize(n + 1, 0);
    used_pie.resize(n + 1, 0);
    ctc_ind.resize(n + 1);
}

void clear_sets() {
    /*O(N*lgN)*/
    phi_females.clear();
    phi_males.clear();
    probability_females.clear();
    probability_males.clear();
    individual_gene.clear();
    edges.clear();
    ctc.clear();
    used.clear();
    used_pie.clear();
    ctc_ind.clear();
    in_degree.clear();
    out_degree.clear();
    st.clear();
    nr_ctc = 0;
    candidates_nodes.clear();
    in_degree.clear();
    in_degree_.clear();
    out_degree.clear();
    out_degree_.clear();
    feedback_vertex_set.clear();
    not_piv.clear();
}

struct cmp {
    bool inline operator()(const pair<int, int> i, const pair<int, int> j) {
        return i.second < j.second || (i.second == j.second && i.first < j.first);
    }
};

priority_queue<pair<int, int>, vector<pair<int, int>>, cmp> pq;


int getMaleFitness(vector<int> offspring) {
    /*return minimum number of arcs to be deleted to obtain DAG*/
    /*creates the subgraph with nodes in subset*/
    vector<set<int>> in_degree_local(n + 1);
    vector<set<int>> out_degree_local(n + 1);
    vector<int> sources, sinks;
    for (auto it : edges) {
        if (candidates_nodes_set.count(it.first) && candidates_nodes_set.count(it.second)) {
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
    while (!pq.empty()) {
        /* remove all sinks */
        while (!sinks.empty()) {
            auto node = sinks.back();
            sinks.pop_back();
            if (alreadyChecked[node]) {
                continue;
            }
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
        /* remove all sources */
        while (!sources.empty()) {
            auto node = sources.back();
            sources.pop_back();
            if (alreadyChecked[node]) {
                continue;
            }
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

        auto node = pq.top();
        pq.pop();
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
    }
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
    for (auto it : edges) {
        if (candidates_nodes_set.count(it.first) && candidates_nodes_set.count(it.second) &&
            posInTopSort[it.first] >= posInTopSort[it.second]) {
            ++missMatch;
        }
    }
    return missMatch;
}

int getFemaleFitness(vector<int> offspring) {
    int nodesCounter = 0;
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        nodesCounter += offspring[i];
    }
    return nodesCounter;
}


void generateFemalePopulation() {
    vector<int> randomPermutation;
    randomPermutation.reserve(candidates_nodes.size());
    for (auto it : candidates_nodes) {
        randomPermutation.emplace_back(it);
    }
    /* consider a topological sorting which is subset of a random permutation */
    vector<int> nodePosInTopSort(n + 1, 0);
    set<vector<int> > femaleInitialGenerationSet;
    for (; femaleInitialGenerationSet.size() < initial_population_size;) {
        vector<bool> present(n + 1, false);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle(randomPermutation.begin(), randomPermutation.end(), std::default_random_engine(seed));
        for (int j = 0; j < randomPermutation.size(); ++j) {
            auto it = randomPermutation[j];
            nodePosInTopSort[it] = j;
            present[it] = false; // initially all nodes are part of subset
            vector<int> notOk;
            /* all children of current node should be after */
            for (auto it2 : out_degree[it]) {
                if (!present[it2]) {
                    notOk.emplace_back(it2);
                }
            }
            /* either insert current node and remove all children that are before or do nothing */
            if (rand() % 2) {
                for (auto it2 : notOk) {
                    present[it2] = true; // all previous children become unavailable
                }
            } else {
                present[it] = true; // current node is not part of solution
            }
        }
        vector<pair<int, int>> output_edges;
        for (auto it : edges) {
            if (!present[randomPermutation[nodePosInTopSort[it.first]]] &&
                !present[randomPermutation[nodePosInTopSort[it.second]]]) {
                output_edges.emplace_back(it);
            }
        }
        out2 << n << ' ' << output_edges.size() << '\n';
        for (auto it : output_edges) {
            out2 << it.first << ' ' << it.second << '\n';
        }
        out2 << "\n\n\n\n";
        /* first part of chromosome - subset description */
        vector<int> chromosome(candidates_nodes.size() + ceil(log2(candidates_nodes.size())) + 2, 0);
        for (int j = 0; j < candidates_nodes.size(); ++j) {
            chromosome[j] = 1 - present[randomPermutation[nodePosInTopSort[candidates_nodes[j]]]];
        }
        /* second part of chromosome - virus description */
        for (int j = candidates_nodes.size(); j < candidates_nodes.size() + ceil(log2(candidates_nodes.size())); ++j) {
            chromosome[j] = rand() % 2;
        }
        /* third part of chromosome - virus reproduction description */
        for (int j = (int) candidates_nodes.size() + (int) ceil(log2(candidates_nodes.size()));
             j < candidates_nodes.size() + ceil(log2(candidates_nodes.size())) + 2; ++j) {
            if (!(rand() % 10)) {
                chromosome[j] = 1;
            } else {
                chromosome[j] = 0;
            }
        }
        if (femaleInitialGenerationSet.count(chromosome)) {
            continue;
        }
        femaleInitialGenerationSet.insert(chromosome);
        femaleInitialGeneration.emplace_back(chromosome);
        //   cout << "F " << getFemaleFitness(chromosome) << ' ' << getMaleFitness(chromosome) << '\n';
    }
}

void generateMalePopulation() {
    for (int i = 1; i <= initial_population_size; ++i) {
        vector<int> chromosome(candidates_nodes.size() + ceil(log2(candidates_nodes.size())) + 2, 0);
        /* first part of chromosome - subset description */
        for (int j = 0; j < candidates_nodes.size(); ++j) {
            chromosome[j] = rand() % 2;
        }
        /* second part of chromosome - virus description */
        for (int j = candidates_nodes.size(); j < candidates_nodes.size() + ceil(log2(candidates_nodes.size())); ++j) {
            chromosome[j] = rand() % 2;
        }
        /* third part of chromosome - virus reproduction description */
        for (int j = (int) candidates_nodes.size() + (int) ceil(log2(candidates_nodes.size()));
             j < candidates_nodes.size() + ceil(log2(candidates_nodes.size())) + 2; ++j) {
            if (!(rand() % 10)) {
                chromosome[j] = 1;
            } else {
                chromosome[j] = 0;
            }
        }
        /* if it is not DAG becomes part of initial population */
        if (getMaleFitness(chromosome)) {
            maleInitialGeneration.emplace_back(chromosome);
        } else {
            --i;
        }
        //   cout << "M " << getFemaleFitness(chromosome) << ' ' << getMaleFitness(chromosome) << '\n';
    }
}

void run_GA() {
    generateFemalePopulation();
    generateMalePopulation();
    phi_females.resize(initial_population_size);
    phi_males.resize(initial_population_size);
    probability_females.resize(initial_population_size);
    probability_males.resize(initial_population_size);

    cout << femaleInitialGeneration.size() << ' ' << maleInitialGeneration.size() << '\n';
    vector<pair<int, int>> femalePopulation, malePopulation;
    for (int i = 0; i < femaleInitialGeneration.size(); ++i) {
        femalePopulation.emplace_back(i, getFemaleFitness(femaleInitialGeneration[i]));
    }
    sort(femalePopulation.begin(), femalePopulation.end(),
         [&](const pair<int, int> i, const pair<int, int> j) { return i.second > j.second; });
    for (int i = 0; i < maleInitialGeneration.size(); ++i) {
        malePopulation.emplace_back(i, getMaleFitness(maleInitialGeneration[i]));
    }
    sort(malePopulation.begin(), malePopulation.end(),
         [&](const pair<int, int> i, const pair<int, int> j) { return i.second > j.second; });
    int theta = 3;
    double sum_males = 0, sum_females = 0;
    for (int i = 0; i < initial_population_size; ++i) {
        phi_males[i] = (double) (phi_males.size() - i - 1) / (double) theta + 1;
        phi_females[i] = (double) ((phi_females.size() - i - 1)) / (double) theta + 1;
        sum_males += phi_males[i];
        sum_females += phi_females[i];
    }
    for (int i = 0; i < phi_males.size(); ++i) {
        probability_males[i] = phi_males[i] / sum_males;
        probability_females[i] = phi_females[i] / sum_females;
    }
    /*
    for (auto it : femalePopulation) {
        cout << it.first << ' ' << it.second << '\n';
    }
    cout << "\n\n";
    for (auto it : malePopulation) {
        cout << it.first << ' ' << it.second << '\n';
    }
    cout << "\n\n";
    */
    for (int gen = 1; gen <= number_of_generations; ++gen) {
        vector<vector<int>> newFemaleGeneration = femaleInitialGeneration, newMaleGeneration = maleInitialGeneration;
        set<vector<int>> newFemaleGenerationSet, newMaleGenerationSet;
        vector<pair<int, int>> newFemalePopulation = femalePopulation, newMalePopulation;
        cout << gen << ' ' << femalePopulation[0].second << ' ' << malePopulation[0].second << '\n';
        for (int k = 1; k <= number_of_offsprings; ++k) {
            double rand_males = ((double) rand() / (RAND_MAX));
            double rand_females = ((double) rand() / (RAND_MAX));
            double curr_males = 0;
            double curr_females = 0;
            int male_index;
            int female_index;
            for (int i = 0;; ++i) {
                curr_males += probability_males[i];
                if (curr_males >= rand_males - 1e-10) {
                    male_index = i;
                    break;
                }
            }
            for (int i = 0;; ++i) {
                curr_females += probability_females[i];
                if (curr_females >= rand_females - 1e-10) {
                    female_index = i;
                    break;
                }
            }
            auto male = femalePopulation[male_index];
            auto female = malePopulation[female_index];
            vector<int> offspring(maleInitialGeneration[male.first].size());
            for (int i = 0; i < offspring.size() - 2; ++i) {
                offspring[i] =
                        rand() % 2 == 1 ? maleInitialGeneration[male.first][i]
                                        : femaleInitialGeneration[female.first][i];
            }
            offspring[offspring.size() - 2] =
                    rand() % 2 == 1 ? maleInitialGeneration[male.first][offspring.size() - 2]
                                    : maleInitialGeneration[male.first][offspring.size() - 1];
            offspring[offspring.size() - 1] =
                    rand() % 2 == 1 ? femaleInitialGeneration[female.first][offspring.size() - 2]
                                    : femaleInitialGeneration[female.first][offspring.size() - 1];

            if (offspring[offspring.size() - 1] == 0 && offspring[offspring.size() - 2] == 0) {
                if (!(rand() % 10)) {
                    offspring[offspring.size() - 2] = 1;
                }
            }
            int length = ceil(log2(candidates_nodes.size()));
            if (offspring[offspring.size() - 1] == 1 && offspring[offspring.size() - 2] == 1) {
                for (int i = 0; i < candidates_nodes.size(); ++i) {
                    offspring[i] ^= offspring[candidates_nodes.size() + i % length];
                }
                /*
                for (int i = candidates_nodes.size(); i < candidates_nodes.size() + length; ++i) {
                    offspring[i] = rand() % 2;
                }
                */
                offspring[offspring.size() - 1] = 0;
                offspring[offspring.size() - 2] = 0;
            }
            auto maleFitness = getMaleFitness(offspring);
            auto femaleFitness = getFemaleFitness(offspring);
            if (!maleFitness) {
                if (newFemaleGenerationSet.count(offspring)) {
                    continue;
                }
                newFemalePopulation.emplace_back(newFemalePopulation.size(), femaleFitness);
                newFemaleGeneration.emplace_back(offspring);
                newFemaleGenerationSet.insert(offspring);
            } else {
                if (newMaleGenerationSet.count(offspring)) {
                    continue;
                }
                newMalePopulation.emplace_back(newMalePopulation.size(), maleFitness);
                newMaleGeneration.emplace_back(offspring);
                newMaleGenerationSet.insert(offspring);
            }
        }
        sort(newFemalePopulation.begin(), newFemalePopulation.end(),
             [&](const pair<int, int> i, const pair<int, int> j) {
                 return i.second > j.second || (i.second == j.second && i.first < j.first);
             });
        sort(newMalePopulation.begin(), newMalePopulation.end(),
             [&](const pair<int, int> i, const pair<int, int> j) {
                 return i.second < j.second || (i.second == j.second && i.first < j.first);
             });
        /*
        int maleSize = newMalePopulation.size();
        for (int i = maleSize, j = maleSize; newMaleGenerationSet.size() < initial_population_size; ++i) {
            if (newMaleGenerationSet.count(maleInitialGeneration[malePopulation[i - maleSize].first])) {
                continue;
            }
            newMaleGeneration.emplace_back(maleInitialGeneration[malePopulation[i - maleSize].first]);
            newMalePopulation.emplace_back(j, malePopulation[i - maleSize].second);
            newMaleGenerationSet.insert(maleInitialGeneration[malePopulation[i - maleSize].first]);
            ++j;
        }
        */
        maleInitialGeneration.clear();
        femaleInitialGeneration.clear();
        malePopulation.clear();
        femalePopulation.clear();
        for (int i = 0; i < initial_population_size; ++i) {
            maleInitialGeneration.emplace_back(newMaleGeneration[newMalePopulation[i].first]);
            femaleInitialGeneration.emplace_back(newFemaleGeneration[newFemalePopulation[i].first]);
            malePopulation.emplace_back(i, newMalePopulation[i].second);
            femalePopulation.emplace_back(i, newFemalePopulation[i].second);
        }
    }
    cout << getMaleFitness(femaleInitialGeneration[0]) << '\n';
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        if (!femaleInitialGeneration[0][i]) {
            feedback_vertex_set.insert(candidates_nodes[i]);
        }
    }
}

void testcase(const string &p_in, const string &p_out) {
    ifstream in(p_in);
    ofstream out(p_out);
    int x, y;
    in >> n >> m;
    set_size();
    for (int i = 1; i <= m; ++i) {
        in >> x >> y;
        add_edge(x, y);
    }
    contract_graph();
    for (auto it : candidates_nodes) {
        candidates_nodes_set.insert(it);
    }
    cout << n << ' ' << feedback_vertex_set.size() << '\n';
    cout << candidates_nodes.size() << ' ' << edges.size() << '\n';
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        posInCandidatesNodes[candidates_nodes[i]] = i;
    }
    run_GA();
    out << feedback_vertex_set.size() << '\n';
    cout << feedback_vertex_set.size() << '\n';
    for (auto it : feedback_vertex_set) out << it << ' ';
    out << '\n';
    cout << "************************************\n";
    clear_sets();
    out.close();
    in.close();
}


signed main() {
    srand(0);
    string path_input = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\public-testcases\grader_test)";
    string path_output = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\GA-results2\grader_test)";
    for (int t = 49; t <= 49; ++t) {
        c_start = clock();
        cout << "test " << t << " began\n";
        testcase(path_input + to_string(t) + ".in", path_output + to_string(t) + ".out");
        cout << "test " << t << " finished\n";
    }
    return 0;
}