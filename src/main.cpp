#include <iostream>
#include <string>

#include <vector>
#include <map>
#include <unordered_map>

#include <functional>
#include <chrono>
#include <algorithm>

#include "shti.h"


int main() {
	struct int_hash {
		size_t operator()(int i) { return abs(i); }; // простая хеш функция
	};
	shti::hash_multitable<int, int, int_hash> ht;

	std::cout << "test ===> " << "insert(const value_pair &) <===\n";

	ht.insert({ 21231 , 123123214 });

	std::cout << "passed\n";

	std::cout << "test ===> " << "emplace(K, V) <===\n";

	ht.emplace( 4123123 , 1231234 );

	std::cout << "passed\n";

	std::cout << "test ===> " << "insert(std::initializer_list<value_pair>) <===\n";

	ht.insert({ {4 , 4}, {1,2} , {2,1} , { -1, 2 }, {1, 22}, { 3, 2 } });
	
	std::cout << "passed\n";

	std::cout << "test ===> " << "insert(iterator, iterator) <===\n";

	shti::hash_multitable<int, int, int_hash> ht2 = ht;

	ht.insert(ht2.begin(), ht2.end());

	std::cout << "passed\n";

//	const shti::hash_multitable<int, int, int_hash> gh(std::move(ht));
//	/*for (auto it = ht.begin(); it < ht.end) {
//
//	}
//*/
//	//shti::hash_table<int, int, int_hash> gh(std::move(ht));
//	//shti::hash_table<int, int, int_hash>::iterator it1 = gh.find(1);
//	//shti::hash_table<int, int, int_hash>::iterator it2 = gh.find(2);
//	//auto res = gh.end();
//	//gh.insert(ht.begin(), ht.end());
//	for (auto it = gh.begin(); it != gh.end(); ++it ) {
//		std::cout << it->first << " " << it->second << std::endl;
//	}
	//for (auto it = tested_data_count.begin(); it != tested_data_count.end(); it++) {
	//	insert_erase_test_int_int(it->first, it->second);
	//}
	//ht_test_erase(2500000);
	return 0;
}