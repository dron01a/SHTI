#include <string>
#include <cassert>
#include <iostream>

#include <unordered_map>
#include <map>

#include "../test/benchmarks/benchmark.h"

#include <fstream>

int main() {
	std::ofstream file("result.txt");
	benchmark bench(file);
	bench.run_all_tests();
	file.close();
	return 0;
}
