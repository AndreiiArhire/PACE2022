#include <bits/stdc++.h>

using namespace std;

set<pair<int, int>> edges;
vector<vector<int>> ctc;
vector<int> used, ctc_ind, st, used_pie, not_piv, individual_gene, feindividual_gene;
int n, nr_ctc, m;
time_t c_start;
int pie_counter, initial_population_size = 20, number_of_generations = 3;
vector<vector<int>> male_population, female_population;
vector<int> candidates_nodes, individual, fitness_chromosome;
vector<set<int>> in_degree, out_degree;
set<int> feedback_vertex_set;


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

void dfs_pie(int nod) {
    used_pie[nod] = pie_counter;
    for (int i : out_degree[nod]) {
        if (!used_pie[i] && in_degree[nod].count(i)) {
            dfs_pie(i);
        }
    }
}

void dfs(int nod) {
    used[nod] = 1;
    for (int i : out_degree[nod]) {
        if (!used[i]) dfs(i);
    }
    st.emplace_back(nod);
}

void dfs_individual(int nod) {
    used[nod] = 1;
    for (int i : out_degree[nod]) {
        if (!used[i] && individual_gene[i]) dfs_individual(i);
    }
    st.emplace_back(nod);
}

void dfs_t(int nod) {
    used[nod] = 2;
    for (int i : in_degree[nod]) {
        if (used[i] != 2) dfs_t(i);
    }
    ctc_ind[nod] = nr_ctc;
    ctc[nr_ctc].emplace_back(nod);
}

void dfs_t_male(int nod) {
    used[nod] = 2;
    for (int i : in_degree[nod]) {
        if (used[i] != 2 && individual_gene[i]) dfs_t_male(i);
    }
    ctc_ind[nod] = nr_ctc;
    ctc[nr_ctc].emplace_back(nod);
}


bool erase_PI_edges() {
    /*O((N+M)lgN)*/
    bool ret = false;
    for (auto i : candidates_nodes) used_pie[i] = 0;
    for (auto i : candidates_nodes) {
        if (!used_pie[i]) {
            ++pie_counter;
            dfs_pie(i);
        }
    }
    vector<pair<int, int>> temp;
    for (auto it : edges) {
        if (used_pie[it.first] == used_pie[it.second] && !edges.count(make_pair(it.second, it.first))) {
            temp.emplace_back(it);
        }
    }
    ret |= (bool) (temp.size());
    for (auto it : temp) {
        erase_edge(it.first, it.second);
    }
    return ret;
}

bool compute_SCC_individual() {
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
    bool change = true;
    for (int i = 1; i <= n; ++i) {
        candidates_nodes.emplace_back(i);
    }
    loop();
    erase_PI_edges();
    erase_SCC_edges();
    loop();
    erase_CORE_nodes();
    loop();
    erase_DOM_edges();
    loop();
}


void set_size() {
    /*O(N*lgN)*/
    nr_ctc = 0;
    pie_counter = 0;
    individual_gene.resize(n + 1, 0);
    feindividual_gene.resize(n + 1, 0);
    ctc.resize(n + 1, vector<int>());
    in_degree.resize(n + 1, set<int>());
    out_degree.resize(n + 1, set<int>());
    used.resize(n + 1, 0);
    not_piv.resize(n + 1, 0);
    used_pie.resize(n + 1, 0);
    ctc_ind.resize(n + 1);
}

void clear_sets() {
    /*O(N*lgN)*/
    individual_gene.clear();
    feindividual_gene.clear();
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
    out_degree.clear();
    feedback_vertex_set.clear();
    not_piv.clear();
}

int compute_males_fitness(int pos) {
    for (int i = 0; i < fitness_chromosome.size(); ++i) {
        if (fitness_chromosome[i]) {
            individual_gene[candidates_nodes[i]] = 1;
            individual.emplace_back(candidates_nodes[i]);
        }
    }
    compute_SCC_individual();
    for (int i = 0; i < fitness_chromosome.size(); ++i) {
        if (fitness_chromosome[i]) {
            individual_gene[candidates_nodes[i]] = 0;
            individual.pop_back();
        }
    }
    return nr_ctc;
}

int compute_females_fitness(int pos) {
    int ret = 0;
    for (auto it : female_population[pos]) {
        ret += it;
    }
    return ret;
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
    for (int sz = 1; sz <= initial_population_size; ++sz) {
        map<int, int> position_in_top;
        for (int candidates_node : candidates_nodes) {
            used[candidates_node] = 0;
        }
        for (auto i : candidates_nodes) if (!used[i]) dfs(i);
        vector<int> top_sort = st;
        st.clear();
        for (int j = (int) top_sort.size() - 1; j >= 0; --j) {
            int i = top_sort[j];
            vector<int> not_ok;
            used[i] = 0;
            for (auto it2 : out_degree[i]) {
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
        vector<int> chromosome(candidates_nodes.size());
        for (int i = 0; i < top_sort.size(); ++i) {
            position_in_top[top_sort[i]] = i;
        }
        for (int i = 0; i < candidates_nodes.size(); ++i) {
            chromosome[i] = 1 - used[top_sort[position_in_top[candidates_nodes[i]]]];
        }
        female_population.emplace_back(chromosome);
        for (int i = 0; i < chromosome.size(); ++i) {
            if (!(rand() % 4))
                chromosome[i] = 1;
            else
                chromosome[i] = 0;
        }
        male_population.emplace_back(chromosome);
    }

    for (int gen = 1; gen <= number_of_generations; ++gen) {
        vector<pair<int, int>> best_males, best_females;
        for (int i = 0; i < male_population.size(); ++i) {
            fitness_chromosome = male_population[i];
            best_males.emplace_back(i, compute_males_fitness(i));
        }
        sort(best_males.begin(), best_males.end(),
             [](const pair<int, int> i, const pair<int, int> j) { return i.second > j.second; });
        while (best_males.size() > initial_population_size) {
            best_males.pop_back();
        }
        for (int i = 0; i < female_population.size(); ++i) {
            best_females.emplace_back(i, compute_females_fitness(i));
        }
        sort(best_females.begin(), best_females.end(),
             [](const pair<int, int> i, const pair<int, int> j) { return i.second > j.second; });

        while (best_females.size() > initial_population_size) {
            best_females.pop_back();
        }
        vector<vector<int>> new_population;
        for (auto male : best_males) {
            for (auto female : best_females) {
                vector<int> offspring(male_population[male.first].size());
                for (int i = 0; i < offspring.size(); ++i) {
                    offspring[i] =
                            rand() % 2 == 1 ? male_population[male.first][i] : female_population[female.first][i];
                }
                new_population.emplace_back(offspring);
            }
        }

        for (int i = 0; i < new_population.size(); ++i) {

        }


    }



    /*
    int counter = 0;
    for (int i = 0; i < female_population[0].size(); ++i) {
        counter += female_population[0][i];
    }

    out << counter << '\n';
    for (int i = 0; i < female_population[0].size(); ++i) {
        if (female_population[0][i]) {
            out << candidates_nodes[i] << ' ';
        }
    }
    */
    out.close();
    in.close();
    /*
    out << feedback_vertex_set.size() << '\n';
    for (auto it : feedback_vertex_set) out << it << ' ';
    out << '\n';
     */
    clear_sets();
}


signed main() {
    srand(0);
    string path_input = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\correct-testcases\grader_test)";
    string path_output = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\GA-results\grader_test)";
    for (int t = 3; t <= 3; ++t) {
        c_start = clock();
        cout << "test " << t << " began\n";
        testcase(path_input + to_string(t) + ".in", path_output + to_string(t) + ".out");
        cout << "test " << t << " finished\n";
    }

    return 0;
}
