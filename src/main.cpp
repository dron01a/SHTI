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

	std::cout << "test ===> " << "find(const key_type &) <===\n";
	{
		auto finded = ht.find(1);
	}
	std::cout << "passed\n";

	std::cout << "test ===> " << " at(const key_type &) <===\n";
	{
		auto finded = ht.at(1);
	}
	std::cout << "passed\n";
	return 0;
}