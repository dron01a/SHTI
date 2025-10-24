#include <iostream>
#include <string>

#include <vector>
#include <map>
#include <unordered_map>

#include <chrono>

#include "shti.h"

int main() {
	//shti::bucket<std::string, int> bucket; // корзина
	//for (size_t i = 0; i < 1000000; ++i) {
	//	bucket.add(std::to_string(i), i * 10);
	//}
	//shti::node<std::string, int> res = bucket.get("1000");
	/*for (size_t i = 0; i < bucket.size(); ++i) {
		std::cout << bucket[i].value() << std::endl;
	}*/
	//shti::hash_table<std::string, int> table;
	//table.add("A", 1);
	//table.add("B", 2);
	//table.add("C", 1);
	//table.add("vA", 1);
	//table.add("vA", 2);
	//table.add("vA1212", 1);
	//table.add("vA1212", 2);
	//table.add("vs12", 1);
	//table.add("vAasd1212", 1);
	//for (size_t i = 0; i < 1000000; ++i) {
	//	table.add(std::to_string(i), i * 10);
	//}
//	shti::hash_table::;
	//shti::hash_table<int, int> ht;
	////it1++;
	//for (size_t i = 0; i < 1000000; ++i) {
	//	ht.insert(i, i);
	//}

	//std::map<int, int> m;
	//for (size_t i = 0; i < 1000000; ++i) {
	//	m.insert({ i, i });
	//}

	//std::multimap<int, int> mm;
	//for (size_t i = 0; i < 1000000; ++i) {
	//	mm.insert({ i, i });
	//}

	//std::unordered_map<int, int> um;
	//for (size_t i = 0; i < 1000000; ++i) {
	//	um.insert({ i, i });
	//}

	/*for (shti::hash_table<int, int>::iterator it = ht.begin(); it != ht.end(); ++it) {
		std::cout << it->key() << " " << it->value() << std::endl;
	}*/
//	std::cout << ht.end()->key() << " " << ht.end()->value() << std::endl;
	std::cout << "inset <int, int> 1000000 test" << std::endl;
	auto start_ht = std::chrono::steady_clock::now();
	shti::hash_table<int, int> ht;
	for (size_t i = 0; i < 1000000; ++i) {
		ht.insert(i, i);
	}
	auto stop_ht = std::chrono::steady_clock::now();
	auto duration_ht = stop_ht - start_ht;
	std::cout << "shti::hash_table result: " 
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration_ht).count()
		<<std::endl;

	auto start_map = std::chrono::steady_clock::now();
	std::map<int, int> m;
	for (size_t i = 0; i < 1000000; ++i) {
		m.insert({ i, i });
	}
	auto stop_map = std::chrono::steady_clock::now();
	auto duration_map = stop_map - start_map;
	std::cout << "std::map result: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration_map).count()
		<< std::endl;

	auto start_mmap = std::chrono::steady_clock::now();
	std::multimap<int, int> mm;
	for (size_t i = 0; i < 1000000; ++i) {
		mm.insert({ i, i });
	}
	auto stop_mmap = std::chrono::steady_clock::now();
	auto duration_mmap = stop_mmap - start_mmap;
	std::cout << "std::multimap result: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration_mmap).count()
		<< std::endl;

	auto start_umap = std::chrono::steady_clock::now();
	std::unordered_map<int, int> um;
	for (size_t i = 0; i < 1000000; ++i) {
		um.insert({ i, i });
	}
	auto stop_umap = std::chrono::steady_clock::now();
	auto duration_umap = stop_umap - start_umap;
	std::cout << "std::unordered_map result: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration_umap).count()
		<< std::endl;

	return 0;
}