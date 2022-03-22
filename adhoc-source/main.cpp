#include <bits/stdc++.h>

using namespace std;

set<pair<int, int>> edges;
vector<vector<int>> ctc;
vector<int> used, ctc_ind, st, not_piv;
int n, nr_ctc, m;
vector<int> candidates_nodes;
vector<set<int>> in_degree, out_degree;
set<int> feedback_vertex_set, fvs, best_fvs;
vector<double> priority;
clock_t begin_;
vector<int> sol_;
vector<int> v_t_[1000001], ctc_[1000001], st_, v_[1000001];
int used_[1000001], nrctc_;
vector<int> bad_;
int init_n, init_m, markov_count;
vector<pair<int, int>> curr_edges;
int curr_testcase, ad_hoc_counter, current_iteration;
vector<int> double_edges_in_node;
vector<bool> always_in_fvs;

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
        if (!used[i] && !edges.count(make_pair(i, nod))) {
            dfs(i);
        }
    }
    st.emplace_back(nod);
}

void dfs2(int nod) {
    used[nod] = 1;
    for (int i : out_degree[nod]) {
        if (!used[i]) dfs2(i);
    }
    st.emplace_back(nod);
}

void dfs_t(int nod) {
    used[nod] = 2;
    for (int i : in_degree[nod]) {
        if (used[i] != 2 && !edges.count(make_pair(nod, i))) dfs_t(i);
    }
    ctc_ind[nod] = nr_ctc;
    ctc[nr_ctc].emplace_back(nod);
}

void dfs2_t(int nod) {
    used[nod] = 2;
    for (int i : in_degree[nod]) {
        if (used[i] != 2) dfs2_t(i);
    }
    ctc_ind[nod] = nr_ctc;
    ctc[nr_ctc].emplace_back(nod);
}


void compute_SCC_edges() {
    /*O((N+M)lgN)*/
    nr_ctc = 0;
    ctc.clear();
    ctc.resize(candidates_nodes.size() + 1, vector<int>());
    for (int i = 0; i <= init_n; ++i) {
        used[i] = -1;
    }
    for (auto i : candidates_nodes) used[i] = 0;
    for (auto i : candidates_nodes) if (!used[i]) dfs2(i);
    while (!st.empty()) {
        int nod = st.back();
        if (used[nod] != 2) dfs2_t(nod), ++nr_ctc;
        st.pop_back();
    }
}

bool erase_SCC_edges() {
    /*O((N+M)lgN)*/
    bool ret = false;
    nr_ctc = 0;
    ctc.clear();
    ctc.resize(candidates_nodes.size() + 5, vector<int>());
    for (int i = 0; i <= init_n; ++i) {
        used[i] = -1;
    }
    for (auto i : candidates_nodes) used[i] = 0;
    for (auto i : candidates_nodes)
        if (!used[i]) {
            dfs(i);
        }
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
    /*O(2*M*N*log(M)*log(M)*/
    bool ret = false;
    vector<pair<int, int>> to_be_erased;
    for (auto it : edges) {
        int dome = 1;
        for (auto node : in_degree[it.first]) {
            if (edges.count(make_pair(it.first, node)) || !edges.count(make_pair(node, it.second))) {
                dome = 0;
                break;
            }
        }
        if (!dome) {
            dome = 1;
            for (auto node : out_degree[it.second]) {
                if (edges.count(make_pair(node, it.second)) || !edges.count(make_pair(it.first, node))) {
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
        erase_edge(it.first, it.second);
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
        if (edges.size() < 5e3) {
            change |= erase_DOM_edges();
        }
    }
}

void contract_graph() {
    /*O(N(N+M)lgN*/
    loop();
    if (candidates_nodes.size() < 3e4) { /// prevention to not stack overflow
        /// if EXIT CODE 0xC00000FD then comment the line below
        erase_SCC_edges();
        loop();
    }
    erase_CORE_nodes();
    loop();
    if (edges.size() < 5e3) {
        erase_DOM_edges();
    }
    loop();
}


void set_size() {
    /*O(N*lgN)*/
    nr_ctc = 0;
    always_in_fvs.resize(n + 1, false);
    priority.resize(n + 1);
    ctc.resize(n + 1, vector<int>());
    in_degree.resize(n + 1, set<int>());
    out_degree.resize(n + 1, set<int>());
    used.resize(n + 1, 0);
    not_piv.resize(n + 1, 0);
    ctc_ind.resize(n + 1);
}

void clear_sets() {
    /*O(N*lgN)*/
    always_in_fvs.clear();
    priority.clear();
    edges.clear();
    ctc.clear();
    used.clear();
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

void ad_hoc(int type) {
    int counter = 0;
    ++ad_hoc_counter;
    type = rand() % 2;
    /*
    if (3 > current_iteration % 10) {
        type = 0;
    }
    if (6 > current_iteration % 10 && current_iteration % 10 > 2) {
        type = 1;
    }
    if (8 > current_iteration % 10 && current_iteration % 10 > 5) {
        type = 2;
    }
    if (current_iteration % 10 > 7) {
        type = 3;
    }
    */
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
            if (type == 0) {
                priority[it] = -((in_degree[it].size()) *
                                 (out_degree[it].size()));
            }
            if (type == 1) {
                priority[it] = -((double) in_degree[it].size() + out_degree[it].size() -
                                 0.3 * abs((double) in_degree[it].size() - out_degree[it].size()));
            }
            if (type == 2) {
                priority[it] = -(min(in_degree[it].size(), out_degree[it].size()));
            }
            if (type == 3) {
                priority[it] = -(((double) (in_degree[it].size() + out_degree[it].size())) +
                                 ((double) (double_edges_in_node[it])) * 3.5);
            }
        }

        sort(candidates_nodes.begin(), candidates_nodes.end(),
             [](int a, int b) { return priority[a] > priority[b]; });
        int node;
        for (int i = 1; i <= 1 + candidates_nodes.size() / 10000; ++i) {
            node = candidates_nodes.back();
            candidates_nodes.pop_back();
            if (in_degree[node].size() < out_degree[node].size()) {
                for (auto it : in_degree[node]) {
                    if (edges.count(make_pair(node, it))) {
                        --double_edges_in_node[it];
                    }
                }
            } else {
                for (auto it : out_degree[node]) {
                    if (edges.count(make_pair(it, node))) {
                        --double_edges_in_node[it];
                    }
                }
            }
            double_edges_in_node[node] = 0;
            erase_node(node);
            ++counter;
        }
        /*
        if (counter % 100 == 0) {
            cout << candidates_nodes.size() << ' ' << edges.size() << '\n';
        }
        */
    }

}


void compute_markov(vector<int> list_of_nodes) {
    // nodes from this list are in the same scc, it calculates some fvs and returns it
    /// !!! must use scc grade when
    candidates_nodes = list_of_nodes;
    loop();
    compute_SCC_edges();
    if (candidates_nodes.empty()) return;
    vector<int> ret, temp;
    if (nr_ctc > 1) {
        vector<vector<int>> all;
        all.reserve(nr_ctc);
        for (int i = 0; i < nr_ctc; ++i) {
            all.emplace_back(ctc[i]);
        }
        for (const auto &i : all) {
            compute_markov(i);
        }
        return;
    }
    if (candidates_nodes.empty()) return;
    candidates_nodes = ctc[0];
    clock_t end_ = clock();
    double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
    if (elapsed_secs >= 600 - 5) {
        /*abort mision*/
        cout << "ad_hoc stopped!\n";
        for (int i = 1; i <= n; ++i) {
            feedback_vertex_set.insert(i);
        }
        return;
    }
    vector<double> arr(candidates_nodes.size() + 2, 0.);
    vector<vector<double>> nums(candidates_nodes.size() + 2, vector<double>(candidates_nodes.size() + 2, 0.));
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        for (int j = 0; j < candidates_nodes.size(); ++j) {
            if (out_degree[candidates_nodes[i]].count(candidates_nodes[j])) {
                nums[i][j] = 1. / (double) (out_degree[candidates_nodes[i]].size());
            }
            if (i == j) {
                nums[i][j] -= 1.;
            }
        }
    }
    for (int i = 0; i <= candidates_nodes.size(); ++i) {
        nums[candidates_nodes.size()][i] = 1.;
    }
    double eps = 1e-11;
    n = candidates_nodes.size();
    int m = n;
    n++;
    int i = 0, j = 0, k;
    while (i < n && j < m) {
        for (k = i; k < n; k++)
            if (abs(nums[k][j]) > eps)
                break;
        if (k == n) {
            j++;
            continue;
        }
        if (k != i)
            for (int l = 0; l <= m; l++)
                swap(nums[i][l], nums[k][l]);
        for (int l = j + 1; l <= m; l++)
            nums[i][l] /= nums[i][j];
        nums[i][j] = 1;
        for (int nx = i + 1; nx < n; nx++) {
            for (int l = j + 1; l <= m; l++)
                nums[nx][l] -= nums[nx][j] * nums[i][l];
            nums[nx][j] = 0;
        }
        i++;
        j++;
    }
    for (int i = n - 1; i >= 0; i--)
        for (int j = 0; j <= m; j++)
            if (abs(nums[i][j]) > eps) {
                if (j == m) {
                    cout << "Imposibil\n";
                    return;
                }
                arr[j] = nums[i][m];
                for (k = j + 1; k < m; k++)
                    arr[j] -= arr[k] * nums[i][k];
                break;
            }

    vector<pair<double, int>> nodes_;
    map<int, double> mapp;
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        mapp[candidates_nodes[i]] = arr[i];
    }
    arr.clear();
    arr.resize(candidates_nodes.size() + 2, 0.);
    nums.clear();
    nums.resize(candidates_nodes.size() + 2, vector<double>(candidates_nodes.size() + 2, 0.));
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        for (int j = 0; j < candidates_nodes.size(); ++j) {
            if (out_degree[candidates_nodes[i]].count(candidates_nodes[j])) {
                nums[j][i] = 1. / (double) (out_degree[candidates_nodes[i]].size());
            }
            if (i == j) {
                nums[i][j] -= 1.;
            }
        }
    }
    for (int i = 0; i <= candidates_nodes.size(); ++i) {
        nums[candidates_nodes.size()][i] = 1.;
    }

    n = candidates_nodes.size();
    m = n;
    n++;
    i = 0, j = 0, k;
    while (i < n && j < m) {
        for (k = i; k < n; k++)
            if (abs(nums[k][j]) > eps)
                break;
        if (k == n) {
            j++;
            continue;
        }
        if (k != i)
            for (int l = 0; l <= m; l++)
                swap(nums[i][l], nums[k][l]);
        for (int l = j + 1; l <= m; l++)
            nums[i][l] /= nums[i][j];
        nums[i][j] = 1;
        for (int nx = i + 1; nx < n; nx++) {
            for (int l = j + 1; l <= m; l++)
                nums[nx][l] -= nums[nx][j] * nums[i][l];
            nums[nx][j] = 0;
        }
        i++;
        j++;
    }
    for (int i = n - 1; i >= 0; i--)
        for (int j = 0; j <= m; j++)
            if (abs(nums[i][j]) > eps) {
                if (j == m) {
                    cout << "Imposibil\n";
                    return;
                }
                arr[j] = nums[i][m];
                for (k = j + 1; k < m; k++)
                    arr[j] -= arr[k] * nums[i][k];
                break;
            }

    nodes_.reserve(candidates_nodes.size());
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        nodes_.emplace_back(arr[i] + mapp[candidates_nodes[i]], candidates_nodes[i]);
    }
    sort(nodes_.begin(), nodes_.end());
    for (int i = 0; i < candidates_nodes.size(); ++i) {
        candidates_nodes[i] = nodes_[i].second;
    }

    int node = candidates_nodes.back();
    candidates_nodes.pop_back();
    erase_node(node);
    if (!candidates_nodes.empty()) {
        compute_markov(candidates_nodes);
    }
    //if (erase_self_loop_node(node)) continue;
    //if (erase_0in_out_degree_node(node)) continue;
    //if (erase_1in_out_degree_node(node)) continue;
}


void ad_hoc_markov() {
    compute_markov(candidates_nodes);
}

void dfs3(int nod) {
    used_[nod] = 1;
    for (int vecin : v_[nod]) {
        if (!used_[vecin] && !bad_[vecin]) {
            dfs3(vecin);
        }
    }
    st_.emplace_back(nod);
}

void dfs4(int nod) {
    used_[nod] = 1;
    for (int vecin : out_degree[nod]) {
        if (!used_[vecin] && !bad_[vecin]) {
            dfs4(vecin);
        }
    }
    st_.emplace_back(nod);
}

void dfs4_t(int nod) {
    used_[nod] = 2;
    for (int vecin : in_degree[nod]) {
        if (used_[vecin] != 2 && !bad_[vecin]) {
            dfs4_t(vecin);
        }
    }
    ctc_[nrctc_].emplace_back(nod);
}

void dfs3_t(int nod) {
    used_[nod] = 2;
    for (int vecin : v_t_[nod]) {

        if (used_[vecin] != 2 && !bad_[vecin]) {
            dfs3_t(vecin);
        }
    }
    ctc_[nrctc_].emplace_back(nod);
}

int solve2_() {
    int cnt = 0;
    for (auto i : candidates_nodes) {
        if (!used_[i] && !bad_[i]) {
            ++cnt;
            dfs4(i);
        }
    }
    while (!st_.empty()) {
        int nod = st_.back();
        if (used_[nod] != 2 && !bad_[nod]) dfs4_t(nod), ++nrctc_;
        st_.pop_back();
    }
    for (int i = 0; i < nrctc_; ++i) {
        if (ctc_[i].size() != 1) {
            return 0;
        }
    }
    return 1;
}

int solve_() {
    int cnt = 0;
    for (int i = 1; i <= n; ++i) {
        if (!used_[i] && !bad_[i]) {
            ++cnt;
            dfs3(i);
        }
    }
    while (!st_.empty()) {
        int nod = st_.back();
        if (used_[nod] != 2 && !bad_[nod]) dfs3_t(nod), ++nrctc_;
        st_.pop_back();
    }
    for (int i = 0; i < nrctc_; ++i) {
        if (ctc_[i].size() != 1) {
            return 0;
        }
    }
    return 1;
}


void try_to(int type) {
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
    for (auto it : candidates_nodes) {
        if (type == 0) {
            priority[it] = -(in_degree[it].size() * out_degree[it].size());
        }
        if (type == 1) {
            priority[it] = -((double) in_degree[it].size() + out_degree[it].size() -
                             0.3 * abs((double) in_degree[it].size() - out_degree[it].size()));
        }
    }
    sort(candidates_nodes.begin(), candidates_nodes.end(),
         [](int a, int b) { return priority[a] > priority[b]; });
    for (int ii = (int) fvs_vector.size() - 1; ii >= 0; --ii) {
        auto it2 = fvs_vector[ii];
        bad_.clear();
        bad_.resize(n + 5, false);
        for (int i = 0; i <= n + 5; ++i) {
            used_[i] = 0;
        }
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
            used_[i] = 0;
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
    contract_graph();
    double_edges_in_node.clear();
    double_edges_in_node.resize(n + 5, 0);
    if (current_iteration == 0) {
        for (auto it : feedback_vertex_set) {
            always_in_fvs[it] = true;
        }
    }
    for (auto it : edges) {
        if (it.first < it.second && edges.count(make_pair(it.second, it.first))) {
            ++double_edges_in_node[it.first];
            ++double_edges_in_node[it.second];
        }
    }
    /// GRAFUL SE CONTRACTA SI LISTELE DE ADIACENTA NU MAI SUNT VALIDE !!! POT APAREA NOI MUCHII
    bad_.clear();
    bad_.resize(n + 5, true);
    for (int i = 0; i <= n + 5; ++i) {
        used_[i] = -1;
    }
    if (candidates_nodes.size() < 21) {
        for (auto it : candidates_nodes) {
            bad_[it] = false;
        }
        int lim = (1 << candidates_nodes.size());
        int best_sz = candidates_nodes.size(), best_mask = lim;
        for (int mask = 0; mask < lim; ++mask) {
            int sz = 0;
            for (int i = 0; i < candidates_nodes.size(); ++i) {
                if ((1 << i) & mask) {
                    bad_[candidates_nodes[i]] = true;
                    ++sz;
                } else {
                    used_[candidates_nodes[i]] = 0;
                }
            }
            int temp = solve2_();
            if (temp && sz < best_sz) {
                best_sz = sz;
                best_mask = mask;
            }
            for (int i = 0; i < candidates_nodes.size(); ++i) {
                bad_[candidates_nodes[i]] = false;
                used_[candidates_nodes[i]] = -1;
            }
            sol_.clear();
            for (int i = 0; i <= nrctc_; ++i) {
                ctc_[i].clear();
            }
            nrctc_ = 0;
            st_.clear();
        }
        for (int i = 0; i < candidates_nodes.size(); ++i) {
            erase_node(candidates_nodes[i]);
        }
        for (int i = 0; i < candidates_nodes.size(); ++i) {
            if (!((1 << i) & best_mask)) {
                feedback_vertex_set.erase(candidates_nodes[i]);
            }
        }
        int sz = 0;
        for (int i = 0; i < candidates_nodes.size(); ++i) {
            if ((1 << i) & best_mask) {
                sz++;
                bad_[candidates_nodes[i]] = true;
            } else {
                used_[candidates_nodes[i]] = 0;
            }
        }
        for (int i = 0; i < candidates_nodes.size(); ++i) {
            bad_[candidates_nodes[i]] = false;
            used_[candidates_nodes[i]] = -1;
        }
        sol_.clear();
        for (int i = 0; i <= nrctc_; ++i) {
            ctc_[i].clear();
        }
        nrctc_ = 0;
        st_.clear();
        candidates_nodes.clear();
    } else {
        if (candidates_nodes.size() <= 100) {
            if (rand() % 2 == 0 && markov_count < 100) {
                //ad_hoc_markov();
                ++markov_count;
                ad_hoc(rand() % 2);
            } else {
                ad_hoc(rand() % 2);
            }
        } else {
            if (candidates_nodes.size() <= 200) {
                if (rand() % 2 == 0 && markov_count < 40) {
                    ++markov_count;
                    //        ad_hoc_markov();
                    ad_hoc(rand() % 2);
                } else {
                    ad_hoc(rand() % 2);
                }
            } else {
                if (candidates_nodes.size() <= 300) {
                    if (rand() % 3 == 0 && markov_count < 10) {
                        ++markov_count;
                        ad_hoc(rand() % 2);
                        //            ad_hoc_markov();
                    } else {
                        ad_hoc(rand() % 2);
                    }
                } else {
                    if (candidates_nodes.size() <= 400) {
                        if (rand() % 3 == 0 && markov_count < 5) {
                            //              ad_hoc_markov();
                            ad_hoc(rand() % 2);
                            ++markov_count;
                        } else {
                            ad_hoc(rand() % 2);
                        }
                    } else {
                        ad_hoc(rand() % 2);
                    }
                }
            }
        }
    }
    fvs = feedback_vertex_set;
    for (int i = 0; i <= n; ++i) {
        v_[i].clear();
        v_t_[i].clear();
    }
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
    try_to(rand() % 2);
    if (best_fvs.empty() || best_fvs.size() > fvs.size()) {
        best_fvs = fvs;
    }
    fvs.clear();
    clear_sets();
    for (int i = 0; i <= n; ++i) {
        v_[i].clear();
        v_t_[i].clear();
    }
    ad_hoc_counter = 0;
}

void testcase(const string &p_in, const string &p_out) {
    markov_count = 0;
    ifstream in(p_in);
    ofstream out(p_out);
    int x, y;
    in >> init_n >> init_m;
    if (init_n == 0) {
        // empty testcase
        return;
    }
    n = init_n;
    m = init_m;
    set_size();
    for (int i = 1; i <= m; ++i) {
        in >> x >> y;
        add_edge(x, y);
        v_[x].push_back(y);
        v_t_[y].push_back(x);
        curr_edges.emplace_back(x, y);
    }
    for (int i = 1; i <= n; ++i) {
        candidates_nodes.emplace_back(i);
    }
    find_fvs();
    cout << "-1--" << best_fvs.size() << '\n';
    for (current_iteration = 1;; ++current_iteration) {
        clock_t end_ = clock();
        double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
        if (elapsed_secs >= 600 - 5) {
            break;
        }
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
        vector<int> parts = {2, 3, 4, 5};
        int part = rand() % 2;
        int sz = (int) curr_fvs.size() / parts[part];
        int counter = 0;
        for (auto it : curr_fvs) {
            if (always_in_fvs[it]) {
                ap[it] = 1;
                erase_node(it);
                ++counter;
            }
        }
        for (int j = 0; counter < sz && j < curr_fvs.size(); ++j) {
            if (always_in_fvs[curr_fvs[j]]) {
                continue;
            }
            ap[curr_fvs[j]] = 1;
            erase_node(curr_fvs[j]);
            ++counter;
        }
        for (int j = 1; j <= n; ++j) {
            if (!ap[j]) {
                candidates_nodes.emplace_back(j);
            }
        }
        find_fvs();
        if (best_fvs.empty()) break;
        cout << "--" << best_fvs.size() << ' ' << current_iteration << '\n';
    }
    cout << "--" << best_fvs.size() << '\n';
    out << best_fvs.size() << '\n';
    cout << best_fvs.size() << '\n';
    for (auto it : best_fvs) out << it << ' ';
    out << '\n';
    curr_edges.clear();
    best_fvs.clear();
    in.close();
    out.close();
}


signed main() {
    srand(0);
    vector<int> tests;
    //tests.emplace_back(10);
    for (int i = 1; i <= 1; ++i) {
        tests.emplace_back(i);
    }
    string path_input = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\public-testcases\grader_test)";
    string path_output = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\public-testcases\grader_test)";
    for (auto i : tests) {
        curr_testcase = i;
        begin_ = clock();
        cout << "test " << curr_testcase << " began\n";
        testcase(path_input + to_string(curr_testcase) + ".in", path_output + to_string(curr_testcase) + ".out");
        cout << "test " << curr_testcase << " finished\n";
        clock_t end_ = clock();
        double elapsed_secs = double(end_ - begin_) / CLOCKS_PER_SEC;
        cout << "time elapsed in seconds: " << elapsed_secs << '\n';
    }
    return 0;
}