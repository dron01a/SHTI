#include <iostream>
#include <string>

#include <vector>
#include <map>
#include <unordered_map>

#include <functional>
#include <chrono>
#include <algorithm>

#include <cassert>

#include "shti.h"

void basic_operations_test() {
	std::cout << "=== basic operations test begin ===" << std::endl;
	shti::hash_table<int, std::string> table;
	table.insert({ 1, "one"});
	table.insert({ 2, "two" });
	table.insert({ 3, "three" });
	assert(table.size() == 3);
	std::cout << "> insert test complete" << std::endl;
	auto find_res_1 = table.find(1);
	auto find_res_2 = table.find(56);
	assert(find_res_1 != table.end());
	assert(find_res_1->second == "one");
	assert(find_res_2 == table.end());
	std::cout << "> find test complete" << std::endl;
	table[4] = "chetire";
	assert(table[4] == "chetire");
	assert(table.size() == 4);
	std::cout << "> operator[] test complete" << std::endl;
	assert(table.at(2) == "two");
	try {
		table.at(456);
		assert(false);
	}
	catch(shti::error_type err){
		assert(err == shti::error_type::out_of_range);
	}
	std::cout << "> at test complete" << std::endl;
	std::cout << "=== basic operations test passed ===" << std::endl << std::endl;
}

void iterator_test() {
	std::cout << "=== iterator test begin ===" << std::endl;

	shti::hash_table<int, int> table;

	table.insert({ { 1,1 }, {2,2} , {3,2}, {4,1}, {5,1} });

	int count = 0;
	for (auto it = table.begin(); it != table.end(); ++it) {
		count++;
	}
	assert(count == 5);

	count = 0;
	for (const auto& n : table) {
		count++;
	}
	assert(count == 5);

	std::cout << "=== iterator test passed ===" << std::endl << std::endl;
}

void erase_test() {
	std::cout << "=== erase test begin ===" << std::endl;
	shti::hash_table<int, std::string> table;
	table.insert({ 1, "one" });
	table.insert({ 2, "two" });
	table.insert({ 3, "three" });
	table.insert({ 4, "chetiri" });
	size_t count = table.erase(3);
	assert(count == 1);
	assert(table.size() == 3);
	assert(table.find(3) == table.end());
	std::cout << "> erase from key passed" << std::endl;
	auto it = table.find(2);
	if (it != table.end()) {
		table.erase(it);
	}
	assert(table.size() == 2);
	assert(table.find(2) == table.end());
	std::cout << "> erase from iterator passed" << std::endl;
	table.insert({ 5, "5" });
	table.insert({ 6, "6" });
	auto begin = table.find(5);
	auto end = table.find(6);
	if (begin != table.end() && end != table.end()) {
		table.erase(begin, ++end);
	}
	assert(table.size() == 1);
	std::cout << "> erase from range passed" << std::endl;
	std::cout << "=== erase test passed ===" << std::endl << std::endl;
}

void resize_and_rehash_test() {
	std::cout << "=== resize and rehash test begin ===" << std::endl;
	shti::hash_table<int, std::string> original;
	size_t orig_size = original.capacity();
	original.insert({ 0, "zero" });
	original.insert({ 1, "one" });
	original.insert({ 2, "two" });
	original.insert({ 3, "three" });
	original.insert({ 4, "chetiri" });
	original.insert({ 5, "piat" });
	original.insert({ 6, "six" });
	assert(original.size() == 7);
	assert(original.capacity() > orig_size);
	std::cout << "> auto rehash test passed" << std::endl;
	original.resize(20);
	assert(original.size() == 7);
	for (size_t i = 0; i < 7; ++i) {
		assert(original.find(i) != original.end());
	}
	std::cout << "> resize test passed" << std::endl;
	std::cout << "=== resize and rehash test passed ===" << std::endl << std::endl;
}

void move_test() {
	std::cout << "=== resize and rehash test begin ===" << std::endl;

	shti::hash_table<int, std::string> original;
	size_t orig_size = original.capacity();
	original.insert({ 0, "zero" });
	original.insert({ 1, "one" });
	original.insert({ 2, "two" });
	original.insert({ 3, "three" });
	original.insert({ 4, "chetiri" });
	original.insert({ 5, "piat" });
	original.insert({ 6, "six" });
	shti::hash_table<int, std::string> moved(std::move(original));
	std::cout << "> move constructor test passed" << std::endl;
	shti::hash_table<int, std::string> moved2;
	moved2 = std::move(moved);
	std::cout << "> move assign test passed" << std::endl;
	std::cout << "=== resize and rehash test passed ===" << std::endl << std::endl;
}

void copy_test() {

}

int main() {
	/*std::cout << "=== Test begin ===" << std::endl;
	try {
		basic_operations_test();
		iterator_test();
		erase_test();
		resize_and_rehash_test();
		move_test();
	}
	catch (shti::error_type err) {
		std::cerr << "TEST FALLED" << std::endl;
	}
	catch (...) {
		std::cerr << "TEST FALLED" << std::endl;
	}
	std::cout << "=== Test passed ===" << std::endl;*/
	shti::hash_table<int, std::string> original;
	size_t orig_size = original.capacity();
	original.insert({ 0, "zero" });
	original.insert({ 1, "one" });
	original.insert({ 2, "two" });
	original.insert({ 3, "three" });
	original.insert({ 4, "chetiri" });
	original.insert({ 5, "piat" });
	original.insert({ 6, "six" });
	original.insert({ 101, "1six" });

	for (auto it = original.begin(); it != original.end(); it++) {
		std::cout << it->first << " " << it->second << std::endl;
	}
	std::cout << "-------\n";

	shti::hash_table<int, std::string> clone(original);

	for (auto it = clone.begin(); it != clone.end(); it++) {
		std::cout << it->first << " " << it->second << std::endl;
	}

	std::cout << "-------\n";
	return 0;
}