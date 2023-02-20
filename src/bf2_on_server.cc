#include "base_on_server.h"

using namespace BaseOnServer;

u32 K = 2, L = 2;
u64 C = 0, B = 1000;
double U = 1000;
Graph G;
Timer T;

bool satisfy(Set const& s) {
    static u32* deg_out = new u32[G.vrt_num];
    static u32* deg_in  = new u32[G.vrt_num];
    std::fill(deg_out, deg_out + G.vrt_num, 0);
    std::fill(deg_in , deg_in  + G.vrt_num, 0);

    for (auto& x : s.data) {
        for (auto& y : G.edges_r[x]) { deg_out[y] += 1; }
        for (auto& y : G.edges  [x]) { deg_in [y] += 1; }
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
    for (u32 i = 0; i < G.vrt_num; ++i) {
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
        std::cout << "[" << T.get_delay() << "s] No."
            << ++C << " solution: " << s;
        if (C >= B || T.get_total() >= U) {
            std::cout << "---------------------\n"
                // << "Reading file: " << input_path << "\n"
                << "Total found (" << K << ", " << L << ")-plex(es): " << C << "\n"
                << "Total runtime: " << T.get_total() << "s\n"
                << "---------------------\n";
            std::exit(0);
        }
        return;
    }

    for (u32 i = v; i < G.vrt_num; ++i) {
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

    T.start();

    dfs({}, 0);

    std::cout << "---------------------\n"
        << "Reading file: " << input_path << "\n"
        << "Total found (" << K << ", " << L << ")-plex(es): " << C << "\n"
        << "Total runtime: " << T.get_total() << "s\n"
        << "---------------------\n";

    return 0;
}