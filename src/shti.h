#ifndef __SHTI__
#define __SHTI__

#ifdef USE_STD_CONTAINERS
	
#include <array>
#include <algorithm>
#include <vector>

#endif

namespace shti {

	template<typename key_type, typename value_type, typename hash_f = std::hash<key_type>>
	class hash_table {
	private:
		// класс узла хеш таблицы
		template<typename key_type, typename value_type> class node {
		public:
			// конструктор класса
			node(key_type key, value_type value): _key(key), _value(value) {}
			node() {}

			// деструктор класса
			~node() {
				_next = nullptr;
				delete _next;
			}

			// возвращает указтель на следующий элемент
			node<key_type, value_type> * next() { return _next; };

			// возвращает ключ
			key_type key() { return _key; };

			// возвращает значение
			value_type & value() { return _value; }

		private:
			key_type _key; // ключ 
			value_type _value; // занчение
			node<key_type, value_type> * _next = nullptr; // указатель на следующий элемент
		}; 

	public:

		// конструктор класса
		hash_table() {}
		hash_table(size_type capasity) {}
		hash_table(hash_table & _other) {}

		// класс итератора
		class iterator {
			friend class hash_table;
		private:
			hash_table * _owner = nullptr; // таблица которой принадлежит итератор
			size_type _index; // текущий индекс
			node<key_type, value_type> * cur_node = nullptr; // текущий узел

			// конструктор класса
			explicit iterator(hash_table * owner) : _owner(owner) { 
				do {
					_index++;
				} while (_owner->data[_index] != nullptr); // пока не встретим инициализированую ячейку
				cur_node = _owner->data[_index];
			}
			explicit iterator(hash_table * owner, size_type index) : _owner(owner), index(_index) {}
			template <typename _key_type, typename _value_type>
			iterator(const iterator<_key_type, _value_type> &other)
				: _owner(other._owner), _index(other._index) {}
		public:

			// оператры сравнения
			bool operator==(const iterator & it) const {
				return _owner == it.owner && _index == it._index;
			}
			bool operator!=(const iterator & it) const {
				return !(*this == it);
			}

			node<key_type, value_type> & operator*() const {
				return *cur_node;
			}

			node<key_type, value_type> * operator->() const {
				return cur_node;
			}

			// инкремент
			iterator & operator++() {
				if (cur_node->next() != nullptr) {
					cur_node = cur_node->next();
				}
				else {
					do {
						_index++;
					} while (_owner->data[_index] != nullptr); // пока не встретим инициализированую ячейку
					cur_node = _owner->data[_index];
				}
			}
		};
	
	private:
		 
		node<key_type, valu_type> ** data; // данные
	};

};

#endif