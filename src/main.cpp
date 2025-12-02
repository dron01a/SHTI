#include <string>
#include <cassert>
#include <iostream>

#include <unordered_map>
#include <map>

#include "../test/benchmarks/benchmark.h"

int main() {
	benchmark bench(std::cout);
	bench.run_all_tests();
	return 0;
}
