#include <bits/stdc++.h>

using namespace std;

set<pair<int, int>> edges;
vector<vector<int>> ctc;
vector<int> used, ctc_ind, st, used_pie, not_piv, individual_gene;
int n, nr_ctc, m;
vector<vector<int>> male_population, female_population;
vector<int> candidates_nodes, best_female_chromosome;
vector<set<int>> in_degree, out_degree;
set<int> feedback_vertex_set, fvs, best_fvs;
vector<double> phi_males, phi_females, probability_males, probability_females;
vector<double> priority;
clock_t begin_;

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

void erase_node(int node) {
    vector<pair<int, int>> to_be_erased;
    for (auto it : in_degree[node]) {
        to_be_erased.emplace_back(it, node);
    }
    for (auto it : out_degree[node]) {
        to_be_erased.emplace_back(node, it);
    }
    for (auto it : to_be_erased) {
        erase_edge(it.first, it.second);
    }
    feedback_vertex_set.insert(node);
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
    loop();
    erase_SCC_edges();
    loop();
    erase_CORE_nodes();
    loop();
    //erase_DOM_edges();
    //loop();
}


void set_size() {
    /*O(N*lgN)*/
    nr_ctc = 0;
    priority.resize(n + 1);
    individual_gene.resize(n + 1, 0);
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
    priority.clear();
    phi_females.clear();
    phi_males.clear();
    probability_females.clear();
    probability_males.clear();
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
    out_degree.clear();
    feedback_vertex_set.clear();
    not_piv.clear();
}

void ad_hoc() {
    contract_graph();
    int counter = 0;
    while (!candidates_nodes.empty()) {
        clock_t end_ = clock();
        double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
        if (elapsed_secs >= 600 - 5) {
            /*abort mision*/
            cout << "ad_hoc stopped!\n";
            for (int i = 1; i <= n; ++i) {
                feedback_vertex_set.insert(i);
            }
            break;
        }
        loop();
        if (candidates_nodes.empty()) continue;
        for (auto it : candidates_nodes) {
            priority[it] = -(in_degree[it].size() * out_degree[it].size());
        }
        /*
        for (auto it : candidates_nodes) {
            priority[it] = -((double) in_degree[it].size() + out_degree[it].size() -
                             0.3 * abs((double) in_degree[it].size() - out_degree[it].size()));
        }
        */
        sort(candidates_nodes.begin(), candidates_nodes.end(),
             [](int a, int b) { return priority[a] > priority[b]; });
        int node;
        for (int i = 1; i <= 1 + candidates_nodes.size() / 10000; ++i) {
            node = candidates_nodes.back();
            candidates_nodes.pop_back();
            erase_node(node);
            ++counter;
        }

        if (counter % 100 == 0) {
            cout << candidates_nodes.size() << '\n';
        }
    }

}


vector<int> sol_;
vector<int> v_t_[1000001], ctc_[1000001], st_, v_[1000001];
int used_[1000001], nrctc_;
vector<int> bad_;
int init_n, init_m;
vector<pair<int, int>> curr_edges;


void dfs_(int nod) {
    used_[nod] = 1;
    for (int vecin : v_[nod]) {
        if (!used_[vecin] && !bad_[vecin]) dfs_(vecin);
    }
    st_.emplace_back(nod);
}

void dfs_t_(int nod) {
    used_[nod] = 2;
    for (int vecin : v_t_[nod]) {
        if (used_[vecin] != 2 && !bad_[vecin]) dfs_t_(vecin);
    }
    ctc_[nrctc_].emplace_back(nod);
}

int solve_() {
    int cnt = 0;
    for (int i = 1; i <= n; ++i) {
        if (!used_[i] && !bad_[i]) {
            ++cnt;
            dfs_(i);
        }
    }
    while (!st_.empty()) {
        int nod = st_.back();
        if (used_[nod] != 2 && !bad_[nod]) dfs_t_(nod), ++nrctc_;
        st_.pop_back();
    }
    for (int i = 0; i < nrctc_; ++i) {
        if (ctc_[i].size() != 1) {
            return 0;
        }
    }
    return 1;
}


void try_to() {
    clock_t end_ = clock();
    double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
    if (elapsed_secs >= 600 - 5) {
        return;
    }
    for (int i = 1; i <= n; ++i) {
        candidates_nodes.emplace_back(i);
    }
    contract_graph();
    vector<int> ap(n + 2, 0);
    for (auto it : fvs) {
        ap[it] = 1;
        erase_node(it);
    }
    vector<int> c2;
    for (auto it : candidates_nodes) {
        if (!ap[it]) {
            c2.emplace_back(it);
        }
    }
    candidates_nodes = c2;
    vector<int> fvs_vector;
    for (auto it : fvs) {
        fvs_vector.emplace_back(it);
    }
    sort(candidates_nodes.begin(), candidates_nodes.end(),
         [](int a, int b) { return priority[a] > priority[b]; });
    for (int ii = (int) fvs_vector.size() - 1; ii >= 0; --ii) {
        auto it2 = fvs_vector[ii];
        bad_.clear();
        bad_.resize(n + 5, false);
        for (auto it : fvs) {
            bad_[it] = true;
        }
        bad_[it2] = false;
        if (solve_()) {
            fvs.erase(it2);
        }
        sol_.clear();
        for (int i = 0; i <= n; ++i) {
            ctc_[i].clear();
            used_[i] = false;
        }
        nrctc_ = 0;
        st.clear();
        clock_t end_ = clock();
        double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
        if (elapsed_secs >= 600 - 5) {
            break;
        }
    }
}

void find_fvs() {
    ad_hoc();
    fvs = feedback_vertex_set;
    clear_sets();
    n = init_n;
    m = init_m;
    set_size();
    int x, y;
    for (auto it : curr_edges) {
        x = it.first;
        y = it.second;
        v_[x].push_back(y);
        v_t_[y].push_back(x);
        add_edge(x, y);
    }
    try_to();
    if (best_fvs.empty() || best_fvs.size() > fvs.size()) {
        best_fvs = fvs;
    }
    fvs.clear();
    clear_sets();
    for (int i = 0; i <= n; ++i) {
        v_[i].clear();
        v_t_[i].clear();
    }
}

void testcase(const string &p_in, const string &p_out) {
    ifstream in(p_in);
    ofstream out(p_out);
    int x, y;
    in >> init_n >> init_m;
    n = init_n;
    m = init_m;
    set_size();
    for (int i = 1; i <= m; ++i) {
        in >> x >> y;
        add_edge(x, y);
        curr_edges.emplace_back(x, y);
    }
    for (int i = 1; i <= n; ++i) {
        candidates_nodes.emplace_back(i);
    }
    find_fvs();

    for (int i = 1;; ++i) {
        clock_t end_ = clock();
        double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
        if (elapsed_secs >= 600 - 5) {
            break;
        }
        cout << "iter no -> " << i << '\n';
        n = init_n;
        m = init_m;
        set_size();
        for (auto it : curr_edges) {
            x = it.first;
            y = it.second;
            v_[x].push_back(y);
            v_t_[y].push_back(x);
            add_edge(x, y);
        }
        vector<int> curr_fvs;
        for (auto it : best_fvs) {
            curr_fvs.emplace_back(it);
        }
        unsigned num = chrono::system_clock::now().time_since_epoch().count();
        shuffle(curr_fvs.begin(), curr_fvs.end(), default_random_engine(num));
        vector<int> ap(n + 2, 0);
        int sz = (int) curr_fvs.size() / 2;
        for (int j = 0; j < sz; ++j) {
            ap[curr_fvs[j]] = 1;
            erase_node(curr_fvs[j]);
        }
        for (int j = 1; j <= n; ++j) {
            if (!ap[j]) {
                candidates_nodes.emplace_back(j);
            }
        }
        find_fvs();
    }

    cout << best_fvs.size() << '\n';
    out << best_fvs.size() << '\n';
    for (auto it : best_fvs) out << it << ' ';
    out << '\n';
    curr_edges.clear();
    best_fvs.clear();
    in.close();
    out.close();
}


signed main() {
    srand(0);
    string path_input = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\correct-testcases\grader_test)";
    string path_output = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\adhoc-results\grader_test)";
    for (int t = 66; t <= 70; ++t) {
        begin_ = clock();
        cout << "test " << t << " began\n";
        testcase(path_input + to_string(t) + ".in", path_output + to_string(t) + ".out");
        cout << "test " << t << " finished\n";
        clock_t end_ = clock();
        double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
        cout << "time elapsed in seconds: " << elapsed_secs << '\n';
    }

    return 0;
}