#include <iostream>
#include <string>

#include <vector>
#include <map>
#include <unordered_map>

#include <functional>
#include <chrono>
#include <algorithm>

#include "shti.h"

struct int_hash {
	size_t operator()(int i) { return abs(i); }; // простая хеш функция
};

void print_ht(shti::hash_multitable<int, int, int_hash> & ht) {
	for (auto it = ht.begin(); it != ht.end(); it++) {
		std::cout << it->first << " " << it->second << std::endl;
	}
}

void m() {

}

int main() {

	shti::hash_multitable<int, int, int_hash> ht;

	std::cout << "test ===> " << "insert(const value_pair &) <===\n";

	ht.insert({ 21231 , 123123214 });

	std::cout << "passed\n";

	std::cout << "test ===> " << "emplace(K, V) <===\n";

	ht.emplace(4123123, 1231234);

	std::cout << "passed\n";

	std::cout << "test ===> " << "insert(std::initializer_list<value_pair>) <===\n";

	ht.insert({ { 4 , 4 },{ 2,1 } ,{ -1, 2 },{ 1, 1 },{ 1,2 } ,{ 3, 2 },{ -1, 5 },{ 1, 22 } });

	std::cout << "passed\n";

	std::cout << "test ===> " << "insert(iterator, iterator) <===\n";
	{
		shti::hash_multitable<int, int, int_hash> ht1 = ht;
		shti::hash_multitable<int, int, int_hash> ht2 = ht;
		std::cout << "dest\n";
		print_ht(ht1);
		ht1.insert(ht2.begin(), ht2.end());
		std::cout << "res\n";
		print_ht(ht1);
	}
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

	std::cout << "test ===> " << "erase(const key_type & key) <===\n";
	std::cout << "dest\n";
	print_ht(ht);
	size_t deleted = ht.erase(1);
	std::cout << "res\n";
	print_ht(ht);
	std::cout << "removed -> " << deleted << std::endl;

	std::cout << "passed\n";

	std::cout << "test ===> " << "erase(iterator, iterator) <===\n";
	{
		shti::hash_multitable<int, int, int_hash> ht1 = ht;
		std::cout << "dest\n";
		print_ht(ht1);
		ht1.erase(ht1.begin(), ht1.end());
		std::cout << "res\n";
		print_ht(ht1);
	}
	std::cout << "passed\n";
	return 0;
}