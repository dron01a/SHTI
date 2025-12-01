#include <iostream>
#include <string>
#include <cassert>
#include "shti.h"

void basic_operations_test() {
	std::cout << "=== basic operations test begin ===" << std::endl;
	shti::hash_multitable<int, std::string> table;
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

void insert_test() {
	std::cout << "=== insert test begin ===" << std::endl;
	shti::hash_table<int, std::string> table;
	table.insert({ 1, "one" });
	table.insert({ 2, "two" });
	table.insert({ 3, "three" });
	table.insert({ 4, "chetiri" });
	auto res_ins_1 = table.insert({ 1, "2"});
	assert(res_ins_1.second == false);
	std::cout << "> insert(value_pair) passed" << std::endl;
	auto res_ins_or_ass = table.insert_or_assign({ 1 , "2"});
	assert(res_ins_or_ass.second != false);
	assert(table.find(1)->second == "2");
	std::cout << "> insert_or_assign passed" << std::endl;
	table.insert({ {5, "3"}, {6, "6"}, {7, "45"} });
	assert(table.size() == 7);
	assert(table.find(5) != table.end());
	assert(table.find(6) != table.end());
	assert(table.find(7) != table.end());
	std::cout << "> insert(std::initializer_list<value_pair>) passed" << std::endl;
	shti::hash_table<int, std::string> table2;
	table2.insert({ 145, "one" });
	table2.insert({ 278, "two" });
	table.insert(table2.begin(), table2.end());
	assert(table.size() == 9);
	assert(table.find(145) != table.end());
	assert(table.find(278) != table.end());
	std::cout << "> insert from range passed" << std::endl;
	std::cout << "=== insert test passed ===" << std::endl << std::endl;
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
	std::cout << "=== move test begin ===" << std::endl;
	shti::hash_table<int, std::string> original;
	original.insert({ 0, "zero" });
	original.insert({ 1, "one" });
	original.insert({ 2, "two" });
	original.insert({ 3, "three" });
	original.insert({ 4, "chetiri" });
	original.insert({ 5, "piat" });
	original.insert({ 6, "six" });
	size_t orig_size = original.capacity();
	shti::hash_table<int, std::string> moved(std::move(original));
	assert(moved.capacity() == orig_size);
	assert(original.capacity() == 0);
	std::cout << "> move constructor test passed" << std::endl;
	shti::hash_table<int, std::string> moved2;
	moved2 = std::move(moved);
	assert(moved2.capacity() == orig_size);
	assert(moved.capacity() == 0);
	std::cout << "> move assign test passed" << std::endl;
	std::cout << "=== move test passed ===" << std::endl << std::endl;
}

void copy_test() {
	std::cout << "=== copy test begin ===" << std::endl;
	shti::hash_table<int, std::string> original;
	original.insert({ 0, "zero" });
	original.insert({ 1, "one" });
	original.insert({ 2, "two" });
	original.insert({ 3, "three" });
	original.insert({ 4, "chetiri" });
	original.insert({ 5, "piat" });
	original.insert({ 6, "six" });
	size_t orig_size = original.capacity();
	shti::hash_table<int, std::string> copy(original);
	assert(copy.capacity() == orig_size);
	std::cout << "> copy constructor test passed" << std::endl;
	shti::hash_table<int, std::string> copy2;
	copy2 = copy;
	assert(copy2.capacity() == orig_size);
	assert(copy.capacity() == orig_size);
	std::cout << "> copy assign test passed" << std::endl;
	std::cout << "=== copy test passed ===" << std::endl << std::endl;
}

void cmp_operatros_test() {
	std::cout << "=== compare operators test begin ===" << std::endl;
	shti::hash_table<int, std::string> original;
	original.insert({ 0, "zero" });
	original.insert({ 1, "one" });
	original.insert({ 2, "two" });
	original.insert({ 3, "three" });
	original.insert({ 4, "chetiri" });
	original.insert({ 5, "piat" });
	original.insert({ 6, "six" });
	shti::hash_table<int, std::string> copy(original);
	assert(copy == original);
	std::cout << "> == test passed" << std::endl;
	copy.erase(5);
	copy.erase(6);
	assert(copy != original);
	std::cout << "> != test passed" << std::endl;
	std::cout << "=== compare operators test passed ===" << std::endl << std::endl;
}

void merge_test() {
	std::cout << "=== merge test begin ===" << std::endl;
	shti::hash_table<int, std::string> original;
	original.insert({ 0, "zero" });
	original.insert({ 1, "one" });
	original.insert({ 2, "two" });
	original.insert({ 3, "three" });
	original.insert({ 4, "chetiri" });
	original.insert({ 5, "piat" });
	original.insert({ 6, "six" });
	shti::hash_table<int, std::string> merged;
	merged.insert({ 7, "seven" });
	merged.insert({ 8, "vosem" });
	merged.insert({ 9, "devat" });
	merged.merge(original);
	assert(merged.size() == 10);
	std::cout << "> copy merge test passed" << std::endl;
	shti::hash_table<int, std::string> moved;
	moved.merge(std::move(merged));
	assert(moved.size() == 10);
	assert(merged.size() == 0);
	std::cout << "> move merge test passed" << std::endl;
	std::cout << "=== merge test passed ===" << std::endl << std::endl;
}

int main() {
	std::cout << "=== Test begin ===" << std::endl << std::endl;
	try {
		basic_operations_test();
		iterator_test();
		insert_test();
		erase_test();
		resize_and_rehash_test();
		copy_test();
		move_test();
		cmp_operatros_test();
		merge_test();
	}
	catch (shti::error_type err) {
		std::cerr << "TEST FALLED" << std::endl;
	}
	catch (...) {
		std::cerr << "TEST FALLED" << std::endl;
	}
	std::cout << "=== Test passed ===" << std::endl;
	return 0;
}
