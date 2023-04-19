#include "utils_copy.h"

using namespace Utils;

u32 K = 2, L = 2;
u32 solu_bnd = 1000000;
f64 time_bnd = 60;
Logger lgr;
Graph grph;
std::set<Vec<u32>> sol;

bool operator<(Vec<u32> const &lhs, Vec<u32> const &rhs) {
    return lhs.size() != rhs.size()?
            (lhs.size() < rhs.size()) :
            std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

bool set_contains(Vec<u32> const& s, u32 v) {
    return std::binary_search(s.begin(), s.end(), v);
}

u32 induced_out_dgr(u32 v, Vec<u32> const& s) {
    u32 k = std::count_if(s.begin(), s.end(), [&](u32 y)
                          { return set_contains(grph.from[v], y); });
    return k;
}

u32 induced_in_dgr(u32 v, Vec<u32> const& s) {
    u32 l = std::count_if(s.begin(), s.end(), [&](u32 y)
                          { return set_contains(grph.to[v], y); });
    return l;
}

bool sat(Vec<u32> const& s) {
    u32 n = s.size();
    return std::all_of(s.begin(), s.end(), [&](u32 x) {
        u32 k = induced_out_dgr(x, s);
        u32 l = induced_in_dgr(x, s);
        return k + K >= n && l + L >= n;
    });
}

// void extend_to_maximal(Vec<u32>& s) {
//     u32 n = grph.vrt_size;
//     u32 original_size = s.size();
//     for (u32 i = 0; i < n; ++i) {
//         if (std::binary_search(s.begin(), std::next(s.begin(), original_size), i)) continue;
//         s.push_back(i);
//         if (sat(s)) continue;
//         s.pop_back();
//     }
//     std::inplace_merge(s.begin(), std::next(s.begin(), original_size), s.end());
// }

void extend_to_maximal(Vec<u32>& s) {
    static Vec<u32> outd, ind, tmp_outd, tmp_ind;
    outd.clear();
    ind.clear();
    std::transform(s.begin(), s.end(), std::back_inserter(outd), [&](u32 x) { return induced_out_dgr(x, s); });
    std::transform(s.begin(), s.end(), std::back_inserter(ind), [&](u32 x) { return induced_in_dgr(x, s); });

    u32 original_size = s.size();
    for (u32 i = 0, n = grph.vrt_size; i < n; ++i) {
        if (std::binary_search(s.begin(), std::next(s.begin(), original_size), i)) continue;
        u32 outd_i = 0, ind_i = 0, ns = s.size() + 1;
        bool satisfied = true;
        tmp_outd.clear();
        tmp_ind.clear();
        for (u32 j = 0; j < s.size(); ++j) {
            u32 from_i = set_contains(grph.from[i], s[j]);
            u32 to_i = set_contains(grph.to[i], s[j]);
            outd_i += from_i;
            ind_i += to_i;
            tmp_outd.push_back(to_i);
            tmp_ind.push_back(from_i);
            satisfied &= (outd[j] + to_i + K >= ns && ind[j] + from_i + L >= ns);
        }
        satisfied &= (outd_i + K >= ns && ind_i + L >= ns);
        if (satisfied) {
            s.push_back(i);
            for (u32 j = 0; j < s.size(); ++j) {
                outd[j] += tmp_outd[j];
                ind[j] += tmp_ind[j];
            }
            outd.push_back(outd_i);
            ind.push_back(ind_i);
        }
    }
    std::inplace_merge(s.begin(), std::next(s.begin(), original_size), s.end());
}

void record_and_check_enough(Vec<u32> const& cand) {
    lgr.record_and_print(cand);
    if (lgr.record_count() >= solu_bnd || lgr.time_count() >= time_bnd) {
        lgr.finish_and_print();
        std::exit(0);
    }
}

void recursion(Vec<u32> const&, Vec<u32>&);

void extend_and_check(Vec<u32>& cand, Vec<u32>& used) {
    static u32 depth = 0;
    depth += 1;

    extend_to_maximal(cand);
    
    if (sol.find(cand) == sol.end()) {
        sol.insert(cand);
        if (depth & 1) record_and_check_enough(cand);
        recursion(cand, used);
        if (!(depth & 1)) record_and_check_enough(cand);
    }

    depth -= 1;
}

void enum_almost(Vec<u32> const& s, u32 v, Vec<u32>& used) {
    u32 n = s.size() + 1;
    Vec<u32> incl, excl, cand;
    std::partition_copy(s.begin(), s.end(), std::back_inserter(incl), std::back_inserter(excl), [&](u32 x)
                        { return set_contains(grph.to[v], x) && set_contains(grph.from[v], x); });
    incl.insert(std::lower_bound(incl.begin(), incl.end(), v), v);

    u32 m_bnd = std::min(u32(excl.size()), K + L - 2);

    for (u32 m = 0; m <= m_bnd; ++m) {
        Combinations comb(excl, excl.size(), m);
        while (comb.next(cand)) {
            // if (!cand.empty() && v < cand.front()) break; // removed because it makes the performance worse
            u32 original_size = cand.size();
            std::copy(incl.begin(), incl.end(), std::back_inserter(cand));
            std::inplace_merge(cand.begin(), std::next(cand.begin(), original_size), cand.end());
            if (sat(cand)) extend_and_check(cand, used);
        }
    }
}

void recursion(Vec<u32> const& s, Vec<u32>& used) {
    u32 n = grph.vrt_size;
    u32 start = s.empty()? 0 : s.front();
    u32 used_cnt = 0;
    for (u32 i = start; i < n; ++i) {
        if (set_contains(s, i) || std::find(used.begin(), used.end(), i) != used.end()) continue;
        enum_almost(s, i, used);
        used.push_back(i);
        used_cnt += 1;
    }
    used.resize(used.size() - used_cnt);
}

void run() {
    Vec<u32> used;
    recursion({}, used); 
}

int main(int argc, char* argv[]) {
    Str input_path = "../data/small_graph.txt";

    if (argc >= 2) input_path = Str(argv[1]);
    if (argc >= 3) {
        i32 tmp = sscanf(argv[2], "%llu", &solu_bnd);
        if (tmp != 1) {
            return -1;
        }
    }
    if (argc >= 4) {
        i32 tmp = sscanf(argv[3], "%lf", &time_bnd);
        if (tmp != 1) {
            return -1;
        }
    }
    if (argc >= 6) {
        i32 tmp1 = sscanf(argv[4], "%u", &K);
        i32 tmp2 = sscanf(argv[5], "%u", &L);
        if (tmp1 != 1 || tmp2 != 1) {
            return -1;
        }
    }

    std::ifstream fin(input_path);
    fin >> grph;
    fin.close();

    lgr.start_timer();
    run();
    lgr.finish_and_print();
    
    return 0;
}