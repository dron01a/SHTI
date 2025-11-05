#include <iostream>
#include <string>

#include <vector>
#include <map>
#include <unordered_map>

#include <functional>
#include <chrono>
#include <algorithm>

#include "shti.h"

#define TIME_CAST_TYPE std::chrono::nanoseconds
#define DURATION_TYPE long long
#define DURATION_PREF std::nano

typedef std::chrono::time_point<std::chrono::steady_clock> time_point;
typedef std::chrono::duration<DURATION_TYPE, DURATION_PREF> duration;
typedef std::vector<DURATION_TYPE> time_duration_vect;
typedef std::map <std::string, std::function<duration(size_t)>> func_map;

std::vector<std::pair<int, int>> tested_data_count{
	std::make_pair(1000, 10),
	std::make_pair(5000, 10),
	std::make_pair(10000, 10),
	std::make_pair(25000, 10),
	std::make_pair(50000, 10),
	std::make_pair(100000, 10),
	std::make_pair(250000, 10),
	std::make_pair(500000, 10),
	std::make_pair(1000000, 10),
	std::make_pair(2500000, 10),
	std::make_pair(5000000, 10),
};

/*
	тест скорости работы метода find
	данный тест проводится с коллизиями
*/
void erase_test_int_int(size_t items_count, size_t n);

// тест хеш таблицы
duration ht_test_erase(size_t items_count);

// тест хеш таблицы с пользовательской функцией
duration ht_test_erase_ch(size_t items_count);

// тест std::map
duration map_test_erase(size_t items_count);

// тест std::multimap
duration multimap_test_erase(size_t items_count);

// тест std::unordered_map
duration unordered_map_test_erase(size_t items_count);

// талица с тестовыми функциями
func_map test_erase_map{
	{ "ht test + std::hash", ht_test_erase },
	{ "ht test + custrom hash", ht_test_erase_ch },
	{ "map test", map_test_erase },
	{ "multimap test", multimap_test_erase },
	{ "unordered_map test", unordered_map_test_erase },
};

int main() {
	struct int_hash {
		size_t operator()(int i) { return abs(i); }; // простая хеш функция
	};
	shti::hash_table<int, int, int_hash> ht;
	ht.insert({ {4 , 4}, {1,2} , {2,1} , { -1, 2 }, {1, 22}, { 3, 2 } });

	ht.erase(1);

	/*for (auto it = ht.begin(); it < ht.end) {

	}
*/
	//shti::hash_table<int, int, int_hash> gh(std::move(ht));
	//shti::hash_table<int, int, int_hash>::iterator it1 = gh.find(1);
	//shti::hash_table<int, int, int_hash>::iterator it2 = gh.find(2);
	//auto res = gh.end();
	////gh.insert(ht.begin(), ht.end());
	//for (auto it = gh.begin(); it != gh.end(); ++it ) {
	//	std::cout << it->key() << " " << it->value() << std::endl;
	//}
	/*for (auto it = tested_data_count.begin(); it != tested_data_count.end(); it++) {
		erase_test_int_int(it->first, it->second);
	}*/
	//ht_test_erase(250000);
	return 0;
}

void erase_test_int_int(size_t items_count, size_t n){
	std::cout << "find test. items count = " << items_count << " iters = " << n << std::endl;
	for (func_map::iterator it = test_erase_map.begin(); it != test_erase_map.end(); it++) {
		std::cout << '\t' << it->first << std::endl;
		time_duration_vect res_vect;
		for (size_t i = 0; i < n; ++i) {
			duration res = it->second(items_count);
			res_vect.push_back(std::chrono::duration_cast<TIME_CAST_TYPE>(res).count());
		}
		std::cout << "\t\tall_data  = [ " << res_vect[0];
		for (size_t i = 1; i < res_vect.size(); ++i) {
			std::cout << ", " << res_vect[i];
		}
		std::cout << " ]" << std::endl;
		std::sort(res_vect.begin(), res_vect.end());
		DURATION_TYPE min = *std::min_element(res_vect.begin(), res_vect.end());
		DURATION_TYPE max = *std::max_element(res_vect.begin(), res_vect.end());
		DURATION_TYPE midle = 0;
		for (size_t i = 0; i < res_vect.size(); ++i) {
			midle += res_vect[i];
		}
		midle /= res_vect.size();
		DURATION_TYPE median = 0;
		if (res_vect.size() % 2 == 0) {
			median = res_vect[res_vect.size() / 2];
		}
		else {
			median = 0.5 * (res_vect[res_vect.size() / 2 - 1] + res_vect[res_vect.size() / 2]);
		}
		std::cout << "\t\tmin, t = " << min << std::endl;
		std::cout << "\t\tmax, t = " << max << std::endl;
		std::cout << "\t\tmedium, t = " << midle << std::endl;
		std::cout << "\t\tmedian, t = " << median << std::endl;
	}
}

duration ht_test_erase(size_t items_count){
	shti::hash_table<int, int> ht;
	for (size_t i = 0; i < items_count; ++i) {
		ht.insert(i, i);
	}
	auto start_ht = std::chrono::steady_clock::now();
	auto find_res = ht.erase(items_count / 2);
	auto stop_ht = std::chrono::steady_clock::now();
	auto duration_ht = stop_ht - start_ht;
	return duration_ht;
}

duration ht_test_erase_ch(size_t items_count){
	struct int_hash {
		size_t operator()(int i) { return abs(i); }; // простая хеш функция
	};
	shti::hash_table<int, int, int_hash> ht;
	for (size_t i = 0; i < items_count; ++i) {
		ht.insert(i, i);
	}
	auto start_ht = std::chrono::steady_clock::now();
	auto find_res = ht.erase(items_count / 2);
	auto stop_ht = std::chrono::steady_clock::now();
	auto duration_ht = stop_ht - start_ht;
	return duration_ht;
}

duration map_test_erase(size_t items_count){
	std::map<int, int> m;
	for (size_t i = 0; i < items_count; ++i) {
		m.insert({ i, i });
	}
	auto start_map = std::chrono::steady_clock::now();
	auto find_res = m.erase(items_count / 2);
	auto stop_map = std::chrono::steady_clock::now();
	auto duration_map = stop_map - start_map;
	return duration_map;
}

duration multimap_test_erase(size_t items_count){
	std::multimap<int, int> mm;
	for (size_t i = 0; i < items_count; ++i) {
		mm.insert({ i, i });
	}
	auto start_mmap = std::chrono::steady_clock::now();
	auto find_res = mm.erase(items_count / 2);
	auto stop_mmap = std::chrono::steady_clock::now();
	auto duration_mmap = stop_mmap - start_mmap;
	return duration_mmap;
}

duration unordered_map_test_erase(size_t items_count){
	std::unordered_map<int, int> um;
	for (size_t i = 0; i < items_count; ++i) {
		um.insert({ i, i });
	}
	auto start_umap = std::chrono::steady_clock::now();
	auto find_res = um.erase(items_count / 2);
	auto stop_umap = std::chrono::steady_clock::now();
	auto duration_umap = stop_umap - start_umap;
	return duration_umap;
}
