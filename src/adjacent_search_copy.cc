#include "utils_copy.h"

using namespace Utils;

u32 K = 2, L = 2;
u32 solu_bnd = 1000000;
f64 time_bnd = 60;
Logger lgr;
Graph grph;
std::set<Vec<u32>> sol;

bool contains(Vec<u32> const& s, u32 v) {
    return std::binary_search(s.begin(), s.end(), v);
}

u32 calc_out_deg(u32 v, Vec<u32> const& s) {
    u32 k = std::count_if(s.begin(), s.end(), [&](u32 y)
                          { return contains(grph.from[v], y); });
    return k;
}

u32 calc_in_deg(u32 v, Vec<u32> const& s) {
    u32 l = std::count_if(s.begin(), s.end(), [&](u32 y)
                          { return contains(grph.to[v], y); });
    return l;
}

bool sat(Vec<u32> const& s) {
    u32 n = s.size();
    for (auto v : s) {
        u32 k = calc_out_deg(v, s);
        u32 l = calc_in_deg(v, s);
        if (k + K < n || l + L < n) return false;
    }
    return true;
}

void extend_to_maximal(Vec<u32>& s) {
    u32 n = grph.vrt_size;
    std::sort(s.begin(), s.end());
    u32 original_size = s.size();
    for (u32 i = 0; i < n; ++i) {
        if (std::binary_search(s.begin(), std::next(s.begin(), original_size), i)) continue;
        s.push_back(i);
        if (sat(s)) continue;
        s.pop_back();
    }
}

// void extend_to_maximal(Vec<u32>& s) {
//     u32 n = grph.vrt_size;
//     Vec<u32> dout, din;
//     std::sort(s.begin(), s.end());
//     std::transform(s.begin(), s.end(), std::back_inserter(dout), [&](u32 x) { return calc_out_deg(x, s); });
//     std::transform(s.begin(), s.end(), std::back_inserter(din), [&](u32 x) { return calc_in_deg(x, s); });
//     u32 original_size = s.size();
//     std::cout << "cand || " << s << dout << din;
//     for (u32 i = 0; i < n; ++i) {
//         if (std::binary_search(s.begin(), std::next(s.begin(), original_size), i)) continue;
//         u32 dout_i = 0, din_i = 0;
//         bool satisfied = true;
//         for (u32 j = 0; j < s.size(); ++j) {
//             u32 i2sj = contains(grph.to[i], s[j]);
//             u32 sj2i = contains(grph.from[i], s[j]);
//             dout_i += i2sj;
//             din_i += sj2i;
//             dout[j] += sj2i;
//             din[j] += i2sj;
//             std::cout << i2sj << " <> " << sj2i << std::endl;
//             satisfied &= dout[j] + K >= s.size() + 1 && din[j] + L >= s.size() + 1;
//         }
//         std::cout << "  In vertex " << i << std::endl;
//         satisfied &= dout_i + K >= s.size() + 1 && din_i + L >= s.size() + 1;
//         if (satisfied) {
//             s.push_back(i);
//             dout.push_back(dout_i);
//             din.push_back(din_i);
//         } else {
//             for (u32 j = 0; j < s.size(); ++j) {
//                 u32 i2sj = contains(grph.to[i], s[j]);
//                 u32 sj2i = contains(grph.from[i], s[j]);
//                 dout[j] -= sj2i;
//                 din[j] -= i2sj;
//             }
//         }
//     }
//     std::cout << "   extends to || " << s;
// }

bool operator<(Vec<u32> const& lhs, Vec<u32> const& rhs) {
    if (lhs.size() != rhs.size()) return lhs.size() < rhs.size();
    for (u32 i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) return lhs[i] < rhs[i];
    }
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
    std::sort(cand.begin(), cand.end());
    
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
    std::copy_if(s.begin(), s.end(), std::back_inserter(incl), [&](u32 x) {
        u32 k = calc_out_deg(x, s) + contains(grph.to[v], x);
        u32 l = calc_in_deg(x, s) + contains(grph.from[v], x);
        return k + K >= n && l + L >= n;
    });
    std::set_difference(s.begin(), s.end(), incl.begin(), incl.end(), std::back_inserter(excl));
    incl.push_back(v);

    u32 m_bnd = std::min(u32(excl.size()), K + L - 2);

    for (u32 m = 0; m <= m_bnd; ++m) {
        Combinations comb(excl, excl.size(), m);
        while (comb.next(cand)) {
            std::copy(incl.begin(), incl.end(), std::back_inserter(cand));
            if (sat(cand)) extend_and_check(cand, used);
        }
    }
}

void recursion(Vec<u32> const& s, Vec<u32>& used) {
    u32 n = grph.vrt_size;
    u32 used_cnt = 0;
    for (u32 i = 0; i < n; ++i) {
        if (contains(s, i) || std::find(used.begin(), used.end(), i) != used.end()) continue;
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