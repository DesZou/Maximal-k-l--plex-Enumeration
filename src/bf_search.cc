#include "utils.h"

using namespace Utils;

u32 K = 2, L = 2;
u32 B = 1000;
double U = 1000;
Graph G;
Logger T;

bool satisfy(Set const& s) {
    static u32* deg_out = new u32[G.vrt_size];
    static u32* deg_in  = new u32[G.vrt_size];
    std::fill(deg_out, deg_out + G.vrt_size, 0);
    std::fill(deg_in , deg_in  + G.vrt_size, 0);

    for (auto& x : s.data) {
        for (auto& y : G.to  [x]) { deg_out[y] += 1; }
        for (auto& y : G.from[x]) { deg_in [y] += 1; }
    }

    for (auto& x : s.data) {
        if (deg_out[x] + K < s.data.size() ||
            deg_in [x] + L < s.data.size()) {
            return false;
        }
    }

    return true;
}

bool maximal_sat(Set const& s) {
    Set s_cp(s);
    for (u32 i = 0; i < G.vrt_size; ++i) {
        if(s_cp.exist(i)) continue;
        s_cp.add(i);
        if (satisfy(s_cp)) return false;
        s_cp.del(i);
    }
    return true;
}

void dfs(Set const& s, u32 v) {
    if (!satisfy(s)) return;

    if (maximal_sat(s)) {
        T.record_and_print(s);
        if (T.record_count() >= B || T.time_count() >= U) {
            T.finish_and_print();
            std::exit(0);
        }
        return;
    }

    for (u32 i = v; i < G.vrt_size; ++i) {
        dfs(s + i, i + 1);
    }
}

i32 main(i32 argc, char* argv[]) {
	Str input_path = "../data/SNAP_modified/Wiki-Vote.txt";

	if (argc >= 2) input_path = Str(argv[1]);
	if (argc >= 3) {
		i32 tmp = sscanf(argv[2], "%llu", &B);
		if (tmp != 1) {
			return -1;
		}
	}
    if (argc >= 4) {
        i32 tmp = sscanf(argv[3], "%lf", &U);
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

	std::ifstream input_stream(input_path);

	input_stream >> G;
	input_stream.close();

    T.start_timer();

    dfs({}, 0);

    T.finish_and_print();

    return 0;
}