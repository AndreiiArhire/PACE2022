#include <bits/stdc++.h>

using namespace std;

set<pair<int, int>> edges;
vector<vector<int>> ctc;
const int NR_SECONDS = 30 - 5;
vector<int> used, ctc_ind, st, used_pie, not_piv, individual_gene;
int n, nr_ctc, m, theta = 3;
time_t c_start;
int initial_population_size = 100, number_of_generations = 300, best_female_fitness;
vector<vector<int>> male_population, female_population;
vector<int> candidates_nodes, individual, fitness_chromosome, best_female_chromosome;
vector<set<int>> in_degree, out_degree;
vector<vector<int>> in_degree_, out_degree_;
set<int> feedback_vertex_set;
vector<double> phi_males, phi_females, probability_males, probability_females;

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

void dfs(int nod) {
    used[nod] = 1;
    for (int i : out_degree[nod]) {
        if (!used[i] && !in_degree[nod].count(i)) dfs(i);
    }
    st.emplace_back(nod);
}

void dfs_(int nod) {
    used[nod] = 1;
    for (int i : out_degree_[nod]) {
        if (!used[i]) dfs_(i);
    }
    st.emplace_back(nod);
}

void dfs_individual(int nod) {
    used[nod] = 1;
    for (int i : out_degree_[nod]) {
        if (!used[i] && individual_gene[i]) dfs_individual(i);
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

void dfs_t_male(int nod) {
    used[nod] = 2;
    for (int i : in_degree_[nod]) {
        if (used[i] != 2 && individual_gene[i]) dfs_t_male(i);
    }
    ctc_ind[nod] = nr_ctc;
    ctc[nr_ctc].emplace_back(nod);
}

void compute_SCC_individual() {
    nr_ctc = 0;
    ctc.clear();
    ctc.resize(n + 1, vector<int>());
    for (auto i : individual) used[i] = 0;
    for (auto i : individual) if (!used[i]) dfs_individual(i);
    while (!st.empty()) {
        int nod = st.back();
        if (used[nod] != 2) dfs_t_male(nod), ++nr_ctc;
        st.pop_back();
    }
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

void loop() {
    bool change = true;
    while (change) {
        change = false;
        change |= erase_self_loop_nodes();
        change |= erase_0in_out_degree_nodes();
        change |= erase_1in_out_degree_nodes();
    }
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
    best_female_fitness = 0;
    best_female_chromosome.clear();
    individual_gene.clear();
    male_population.clear();
    female_population.clear();
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

int compute_males_fitness() {
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        if (fitness_chromosome[i]) {
            individual_gene[candidates_nodes[i]] = 1;
            individual.emplace_back(candidates_nodes[i]);
        }
    }
    compute_SCC_individual();
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        if (fitness_chromosome[i]) {
            individual_gene[candidates_nodes[i]] = 0;
            individual.pop_back();
        }
    }
    return nr_ctc;
}

int compute_females_fitness() {
    int ret = 0;
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        ret += fitness_chromosome[i];
    }
    return ret;
}


void run_GA() {
    if (candidates_nodes.empty()) return;
    vector<pair<int, int>> best_males, best_females;
    vector<pair<int, int>> new_best_males, new_best_females;
    phi_females.resize(initial_population_size);
    phi_males.resize(initial_population_size);
    probability_females.resize(initial_population_size);
    probability_males.resize(initial_population_size);
    for (int sz = 1; sz <= initial_population_size; ++sz) {
        map<int, int> position_in_top;
        for (int candidates_node : candidates_nodes) {
            used[candidates_node] = 0;
        }
        for (auto i : candidates_nodes) if (!used[i]) dfs_(i);
        vector<int> top_sort = st;
        st.clear();
        for (int j = (int) top_sort.size() - 1; j >= 0; --j) {
            int i = top_sort[j];
            vector<int> not_ok;
            used[i] = 0;
            for (auto it2 : out_degree_[i]) {
                if (used[it2] == 0) {
                    not_ok.emplace_back(it2);
                }
            }
            if (rand() % 2) {
                used[i] = 1;
            } else {
                for (auto it2 : not_ok)
                    used[it2] = 1;
            }
        }
        vector<int> chromosome(candidates_nodes.size() + ceil(log2(candidates_nodes.size())) + 2);
        for (int i = 0; i < top_sort.size(); ++i) {
            position_in_top[top_sort[i]] = i;
        }
        for (int i = 0; i < candidates_nodes.size(); ++i) {
            chromosome[i] = 1 - used[top_sort[position_in_top[candidates_nodes[i]]]];
        }
        for (int i = candidates_nodes.size(); i < candidates_nodes.size() + ceil(log2(candidates_nodes.size())); ++i) {
            chromosome[i] = rand() % 2;
        }
        for (int i = (int) candidates_nodes.size() + (int) ceil(log2(candidates_nodes.size()));
             i < candidates_nodes.size() + ceil(log2(candidates_nodes.size())) + 2; ++i) {
            if (!(rand() % 10)) {
                chromosome[i] = 1;
            } else {
                chromosome[i] = 0;
            }
        }
        fitness_chromosome = chromosome;
        best_females.emplace_back(female_population.size(), compute_females_fitness());
        female_population.emplace_back(chromosome);
        for (int i = 0; i < candidates_nodes.size() + ceil(log2(candidates_nodes.size())); ++i) {
            if (!(rand() % 4))
                chromosome[i] = 1;
            else
                chromosome[i] = 0;
        }
        for (int i = (int) candidates_nodes.size() + (int) ceil(log2(candidates_nodes.size()));
             i < candidates_nodes.size() + ceil(log2(candidates_nodes.size())) + 2; ++i) {
            if (!(rand() % 10)) {
                chromosome[i] = 1;
            } else {
                chromosome[i] = 0;
            }
        }
        fitness_chromosome = chromosome;
        compute_males_fitness();
        best_males.emplace_back(male_population.size(), compute_males_fitness());
        male_population.emplace_back(chromosome);
    }

    sort(best_males.begin(), best_males.end(),
         [](const pair<int, int> i, const pair<int, int> j) { return i.second > j.second; });
    while (best_males.size() > initial_population_size) {
        best_males.pop_back();
    }
    sort(best_females.begin(), best_females.end(),
         [](const pair<int, int> i, const pair<int, int> j) { return i.second > j.second; });

    while (best_females.size() > initial_population_size) {
        best_females.pop_back();
    }
    if (best_females[0].second > best_female_fitness) {
        best_female_fitness = best_females[0].second;
        best_female_chromosome = female_population[best_females[0].first];
    }
    int best_male_fitness = 0;

    double sum_males = 0, sum_females = 0;
    for (int i = 0; i < phi_males.size(); ++i) {
        phi_males[i] = (double) (phi_males.size() - i - 1) / (double) theta + 1;
        phi_females[i] = (double) ((phi_females.size() - i - 1)) / (double) theta + 1;
        sum_males += phi_males[i];
        sum_females += phi_females[i];
    }
    for (int i = 0; i < phi_males.size(); ++i) {
        probability_males[i] = phi_males[i] / sum_males;
        probability_females[i] = phi_females[i] / sum_females;
    }


    for (int gen = 1; gen <= number_of_generations; ++gen) {
        vector<vector<int>> new_female_population, new_male_population;

        for (int k = 1; k <= 800; ++k) {
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
            auto male = best_males[male_index];
            auto female = best_females[female_index];
            vector<int> offspring(male_population[male.first].size());
            for (int i = 0; i < offspring.size() - 2; ++i) {
                offspring[i] =
                        rand() % 2 == 1 ? male_population[male.first][i] : female_population[female.first][i];
            }
            offspring[offspring.size() - 2] =
                    rand() % 2 == 1 ? male_population[male.first][offspring.size() - 2]
                                    : male_population[male.first][offspring.size() - 1];
            offspring[offspring.size() - 1] =
                    rand() % 2 == 1 ? female_population[female.first][offspring.size() - 2]
                                    : female_population[female.first][offspring.size() - 1];

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
                for (int i = candidates_nodes.size(); i < candidates_nodes.size() + length; ++i) {
                    offspring[i] = rand() % 2;
                }
                offspring[offspring.size() - 1] = 0;
                offspring[offspring.size() - 2] = 0;
            }
            fitness_chromosome = offspring;
            int males_fitness = compute_males_fitness();
            int females_fitness = compute_females_fitness();
            if (males_fitness == females_fitness) {
                new_best_females.emplace_back(new_female_population.size(), females_fitness);
                new_female_population.emplace_back(offspring);
            } else {
                new_best_males.emplace_back(new_male_population.size(), males_fitness);
                new_male_population.emplace_back(offspring);
            }
            if ((double) (clock() - c_start) / CLOCKS_PER_SEC > NR_SECONDS) {
                break;
            }
        }
        for (int i = 0; new_best_males.size() < initial_population_size; ++i) {
            new_best_males.emplace_back(best_males[i]);
            new_male_population.emplace_back(male_population[best_males[i].first]);
        }
        sort(new_best_males.begin(), new_best_males.end(),
             [](const pair<int, int> i, const pair<int, int> j) { return i.second > j.second; });
        while (new_best_males.size() > initial_population_size) {
            new_best_males.pop_back();
        }
        male_population.clear();
        for (int i = 0; i < new_best_males.size(); ++i) {
            male_population.emplace_back(new_male_population[new_best_males[i].first]);
            new_best_males[i].first = i;
        }

        for (int i = 0; new_best_females.size() < initial_population_size; ++i) {
            new_best_females.emplace_back(best_females[i]);
            new_female_population.emplace_back(female_population[best_females[i].first]);
        }
        sort(new_best_females.begin(), new_best_females.end(),
             [](const pair<int, int> i, const pair<int, int> j) { return i.second > j.second; });
        while (new_best_females.size() > initial_population_size) {
            new_best_females.pop_back();
        }
        female_population.clear();
        for (int i = 0; i < new_best_females.size(); ++i) {
            female_population.emplace_back(new_female_population[new_best_females[i].first]);
            new_best_females[i].first = i;
        }
        best_males = new_best_males;
        best_females = new_best_females;
        new_best_males.clear();
        new_best_females.clear();
        if (best_females[0].second > best_female_fitness) {
            best_female_fitness = best_females[0].second;
            best_female_chromosome = female_population[best_females[0].first];
        }
        best_male_fitness = max(best_male_fitness, best_males[0].second);
        cout << best_female_fitness << ' ' << best_male_fitness << '\n';
        if ((double) (clock() - c_start) / CLOCKS_PER_SEC > NR_SECONDS) {
            break;
        }
    }

    for (int i = 0; i < candidates_nodes.size(); ++i) {
        if (!best_female_chromosome[i]) {
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
    cout << candidates_nodes.size() << '\n';
    run_GA();
    out << feedback_vertex_set.size() << '\n';
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
    for (int t = 1; t <= 60; ++t) {
        c_start = clock();
        cout << "test " << t << " began\n";
        testcase(path_input + to_string(t) + ".in", path_output + to_string(t) + ".out");
        cout << "test " << t << " finished\n";
    }

    return 0;
}