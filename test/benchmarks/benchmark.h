#ifndef __BENCH__1__
#define __BENCH__1__

#include <vector>
#include <map>
#include <chrono>
#include <algorithm>
#include <ostream>
#include <random>

#include <unordered_map>

#include "../../src/shti.h"

// класс бенчмарка
class benchmark {
	struct result;
	using clock_type = std::chrono::high_resolution_clock;
	using time_point = std::chrono::time_point<clock_type>;
	using duration = std::chrono::duration<double, std::milli>;
	using result_vector = std::vector<result>;
	using bench_func = result(*)(size_t count);
	friend std::ostream & operator<<(std::ostream & stream, const result & res);
	friend std::ostream & operator<<(std::ostream & stream, const result_vector & res_vect);
public:

	// конструктор класса
	benchmark(std::ostream & stream, size_t iterations = 10);

	// деструтор класса
	~benchmark();

	// запускает все тесты
	void run_all_tests();

private:

	// структура результата
	struct result {
		// операторы сравнения
		bool operator==(const result & other);
		bool operator!=(const result & other);
		bool operator>(const result & other);
		bool operator<(const result & other);
		bool operator>=(const result & other);
		bool operator<=(const result & other);

		// поля структуры
		double time; // время
		size_t count; // колличество операций
		double c_per_t; // количество операуций наединицу времени
	};

	// базовая функция для тестирования и сбора данных в вектор
	result_vector bench(bench_func bfunc, size_t count);

	// выполняет бенчмарк функции
	template <typename table_t>
	void bench(std::vector<size_t> & sizes, bench_func bfunc) {
		result_vector max_vect, min_vect, midle_vect, median_vect; // вестора для хранения определенных значений
		for (size_t _i = 0; _i < sizes.size(); ++_i) {
			result_vector res = bench(bfunc, sizes[_i]);
			std::sort(res.begin(), res.end());
			result max = *std::max_element(res.begin(), res.end());
			result min = *std::min_element(res.begin(), res.end());
			max_vect.push_back(max);
			min_vect.push_back(min);
			result midle = { 0, 0, 0 };
			for (size_t i = 0; i < res.size(); ++i) {
				midle.count += res[i].count;
				midle.time += res[i].time;
				midle.c_per_t += res[i].c_per_t;
			}
			midle.count /= res.size();
			midle.time /= res.size();
			midle.c_per_t /= res.size();
			result median = { 0, 0, 0 };
			if (res.size() % 2 == 0) {
				median = res[res.size() / 2];
			}
			else {
				median.time = 0.5 * (res[res.size() / 2 - 1].time + res[res.size() / 2].time);
				median.count = 0.5 * (res[res.size() / 2 - 1].count + res[res.size() / 2].count);
				median.c_per_t = 0.5 * (res[res.size() / 2 - 1].c_per_t + res[res.size() / 2].c_per_t);
			}
			median_vect.push_back(median);
			midle_vect.push_back(midle);
			*_stream << "\t\tmin = " << min << std::endl;
			*_stream << "\t\tmax = " << max << std::endl;
			*_stream << "\t\tmedium = " << midle << std::endl;
			*_stream << "\t\tmedian = " << median << std::endl;
		}
		*_stream << "min\n" << min_vect << std::endl;
		*_stream << "max\n" << max_vect << std::endl;
		*_stream << "median\n" << median_vect << std::endl;
		*_stream << "midle\n" << midle_vect << std::endl;
	}

	// выполняет тест функции вставки, где ключи возрастают по порядку
	void bench_linear_insert();

	// тестирование функции вставки, где ключи возрастают по порядку
	template <typename table_t>
	static result linear_insert_test(size_t count) {
		table_t _table;
		time_point begin = clock_type::now(); // начало 
		for (size_t _i = 0; _i < count; ++_i) {
			_table.insert({ _i , _i });
		}
		time_point end = clock_type::now(); // конец
		duration res = end - begin;
		return { res.count(), count, count / res.count() };
	}
	// выполняет тест функции вставки, где ключи в случайном порядке
	void bench_random_insert();

	// тестирование функции вставки, где ключи в случайном порядке
	template <typename table_t>
	static result random_insert_test(size_t count) {
		table_t _table;
		std::vector<int> keys = generate_random_numbers(count);
		time_point begin = clock_type::now(); // начало 
		for (size_t _i = 0; _i < count; ++_i) {
			_table.insert({ keys[_i] , _i });
		}
		time_point end = clock_type::now(); // конец
		duration res = end - begin;
		return{ res.count(), count, count / res.count() };
	}

    static double random(double begin, double end);


	static std::vector<int> generate_random_numbers(size_t count);

	std::ostream * _stream; // поток вывода данных
	size_t iterations = 0; // колличество итераций в тестах
};


#endif // !__BENCH__1__