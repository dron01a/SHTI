#include "benchmark.h"

benchmark::benchmark(std::ostream & stream, size_t iterations){
	this->iterations = iterations;
	_stream = &stream;
}

benchmark::~benchmark(){
	_stream = nullptr;
}

void benchmark::run_all_tests(){
	//bench_linear_insert();
	bench_random_insert();
}

benchmark::result_vector benchmark::bench(bench_func bfunc, size_t count) {
	result_vector _result;
	for (size_t _i = 0; _i < iterations; ++_i) {
		result res = bfunc(count); // проводим тестирование
		_result.push_back(res);
	}
	return _result;
}

void benchmark::bench_linear_insert() {
	*_stream << "=== linear insert test start ===" << std::endl;
	std::vector<size_t> sizes = { 
		100, 1000, 5000, 10000, 25000, 50000, 100000, 250000, 500000, 1000000, 2500000, 5000000,  
	};
	*_stream << "> bench shti::hash_table<int, int>" << std::endl; 
	bench<shti::hash_table<int, int>>(sizes, linear_insert_test<shti::hash_table<int, int>>);
	*_stream << std::endl;
	struct int_hash {
		size_t operator()(int i) { return abs(i); }; // простая хеш функция
	};
	*_stream << "> bench shti::hash_table<int, int, int_hash>" << std::endl;
	bench<shti::hash_table<int, int>>(sizes, random_insert_test<shti::hash_table<int, int, int_hash>>);
	*_stream << std::endl;
	*_stream << "> bench std::unordered_map<int, int>" << std::endl;
	bench<std::unordered_map<int, int>>(sizes, linear_insert_test<std::unordered_map<int, int>>);
	*_stream << std::endl;
	*_stream << "=== linear insert test end ===" << std::endl;
}

void benchmark::bench_random_insert(){
	*_stream << "=== random insert test start ===" << std::endl;
	std::vector<size_t> sizes = {
		100, 1000, 5000, 10000, 25000, 50000, 100000, 250000, 500000, 1000000, 2500000, 5000000,
	};
	*_stream << "> bench shti::hash_table<int, int>" << std::endl;
	bench<shti::hash_table<int, int>>(sizes, random_insert_test<shti::hash_table<int, int>>);
	*_stream << std::endl;
	struct int_hash {
		size_t operator()(int i) { return abs(i); }; // простая хеш функция
	};
	*_stream << "> bench shti::hash_table<int, int, int_hash>" << std::endl;
	bench<shti::hash_table<int, int>>(sizes, random_insert_test<shti::hash_table<int, int, int_hash>>);
	*_stream << std::endl;
	*_stream << "> bench std::unordered_map<int, int>" << std::endl;
	bench<std::unordered_map<int, int>>(sizes, random_insert_test<std::unordered_map<int, int>>);
	*_stream << std::endl;
	*_stream << "=== random insert test end ===" << std::endl;
}

double benchmark::random(double begin, double end) {
	static double last = 0;
	last = fmod((last * 73129 + 95121), 100000) / clock();
	last = begin + fmod(last, end - begin);
	return last;
}

std::vector<int> benchmark::generate_random_numbers(size_t count){
	std::vector<int> _result;
	for (size_t _i = 0; _i < count; ++_i) {
		_result.push_back(random(-(int)count, count));
	}
	//std::shuffle(_result.begin(), _result.end(), d);
 	return _result;
}

bool benchmark::result::operator==(const result & other){
	return this->time == other.time && this->c_per_t == other.c_per_t && this->count == other.count;
}

bool benchmark::result::operator!=(const result & other){
	return !(*this == other);
}

bool benchmark::result::operator>(const result & other){
	return this->time > other.time;
}

bool benchmark::result::operator<(const result & other){
	return this->time < other.time;
}

bool benchmark::result::operator>=(const result & other){
	return !(*this < other);
}

bool benchmark::result::operator<=(const result & other) {
	return !(*this > other);
}

std::ostream & operator<<(std::ostream & stream, const benchmark::result & res){
	stream << " [ " << res.time << " , " << res.count << " , " << res.c_per_t << " ] ";
	return stream;
}

std::ostream & operator<<(std::ostream & stream, const benchmark::result_vector & res_vect){
	stream << "time = [ " << res_vect[0].time;
	for (size_t _i = 1; _i < res_vect.size(); ++_i) {
		stream << " , " << res_vect[_i].time;
	}
	stream << " ] " << std::endl;
	stream << "count = [ " << res_vect[0].count;
	for (size_t _i = 1; _i < res_vect.size(); ++_i) {
		stream << " , " << res_vect[_i].count;
	}
	stream << " ] " << std::endl;
	stream << "c_per_t = [ " << res_vect[0].c_per_t;
	for (size_t _i = 1; _i < res_vect.size(); ++_i) {
		stream << " , " << res_vect[_i].c_per_t;
	}
	stream << " ] " << std::endl;
	return stream;
}
