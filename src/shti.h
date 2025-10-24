#ifndef __SHTI__
#define __SHTI__

#ifdef USE_STD_CONTAINERS
	
#include <array>
#include <algorithm>
#include <vector>

#endif

#define CAPACITY 8
#define REHASH_COEF 1
#define SIZE_INC_MULTIPLIER 1.5

namespace shti {

	template<typename key_type, typename value_type, typename hash_f = std::hash<key_type>>
	class hash_table {

		using size_type = std::size_t;

	private:
		// класс узла хеш таблицы
		template<typename key_type, typename value_type> class node {
			friend class hash_table;
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

		// класс итератора
		template<typename key_type, typename value_type, typename table_type>
		class _iterator {
			friend table_type;
		private:
			table_type * _owner = nullptr; // таблица которой принадлежит итератор
			size_type _index = 0; // текущий индекс
			node<key_type, value_type> * cur_node = nullptr; // указатель на текущий узел

															 // конструктор класса
			explicit _iterator(table_type * owner) : _owner(owner) {
				do {
					_index++;
				} while (_owner->data[_index] == nullptr); // пока не встретим инициализированую ячейку
				cur_node = _owner->data[_index];
			}
			explicit _iterator(table_type * owner, size_type index) : _owner(owner), _index(index) {
				cur_node = _owner->data[index];
			}
			explicit _iterator(table_type * owner, node<key_type, value_type> * _node) : _owner(owner) {
				cur_node = _node;
			}
			template <typename _key_type, typename _value_type, typename table_type>
			_iterator(const _iterator<_key_type, _value_type, typename table_type> &other)
				: _owner(other._owner), _index(other._index) {
				cur_node = other.cur_node;
			}
		public:

			// оператры сравнения
			bool operator==(const _iterator & it) const {
				return _owner == it._owner && _index == it._index;
			}
			bool operator!=(const _iterator & it) const {
				return !(*this == it);
			}

			node<key_type, value_type> & operator*() const {
				return *cur_node;
			}

			node<key_type, value_type> * operator->() const {
				return cur_node;
			}

			// инкремент
			_iterator & operator++() {
				if (cur_node->next() != nullptr) {
					cur_node = cur_node->next();
				}
				else {
					do {
						_index++;
					} while (_owner->data[_index] == nullptr); // пока не встретим инициализированую ячейку
					cur_node = _owner->data[_index];
				}
				return *this;
			}
		};

		using iterator = _iterator<key_type, value_type, hash_table>;
		using const_iterator = _iterator<const key_type, const value_type, const hash_table>;

		// конструктор класса
		hash_table() {
			data = new node<key_type, value_type>*[_capacity];
			for (size_type i = 0; i < _capacity; ++i) {
				data[i] = nullptr;
			}
		}
		hash_table(size_type capasity) {
			this->_capacity = capasity;
			data = new node<key_type, value_type>*[_capacity];
			for (size_type i = 0; i < _capacity; ++i) {
				data[i] = nullptr;
			}
		}
		hash_table(hash_table & _other) : hash_table(_other._capacity) {
			// ????
		}
		hash_table(hash_table && _other) {
			// ????
		}

		// деструктор класса
		~hash_table() {
			for (size_type i = 0; i < _capacity; ++i) {
				delete data[i];
			}
			delete[] data;
		}

		// итераторы указывающие на начало
		iterator begin() noexcept { return iterator(this); }
		const_iterator begin() const noexcept { return const_iterator(this); }
		const_iterator cbegin() const noexcept { return const_iterator(this); }

		// итераторы указывающие на конец
		iterator end() noexcept {
			return iterator(this, _capacity -1);
		}
		const_iterator end() const noexcept {
			return const_iterator(this, _capacity - 1);
		}
		const_iterator cend() const noexcept {
			return const_iterator(this, _capacity - 1);
		}

		// выполняет вставку элемента в таблицу
		iterator insert(const node<key_type, value_type> & _node) {
			return emplace_implementation(_node.key(), _node.value());
		}

		iterator insert(key_type key, value_type value) {
			node<key_type, value_type> _node(key, value);
			return emplace_implementation(_node.key(), _node.value());
		}
	
	private:

		// изменяет размер 
		void resize(size_type new_size) {
			if (new_size > _capacity) {
				rehash(new_size);
			}
		}

		// перераспределение элементов в таблице
		void rehash(size_type new_size) {
			size_type last_capacity = _capacity; 
			_capacity = new_size;
			node<key_type, value_type> ** temp_data = new node<key_type, value_type> *[new_size];
			for (size_type i = 0; i < new_size; ++i) {
				temp_data[i] = nullptr;
			}
			_size = 0;
			std::swap(data, temp_data); // меняем местами области памяти
			for (size_type i = 0; i < last_capacity; ++i) {
				node<key_type, value_type> * cur_node = temp_data[i];
				while (cur_node != nullptr) {
					emplace_implementation(cur_node->key(), cur_node->value());
					cur_node = cur_node->next();
				}
			}
			for (size_type i = 0; i < last_capacity; ++i) { // очищаем выделенную память
				delete temp_data[i];
			}
			delete[] temp_data;
		}

		// шаблон ставки элементов в таблицу
		template <typename key_type, typename ... elements>
		iterator emplace_implementation(const key_type & key, elements &&... elem) {
			if (_size + 1 > int(rehash_coef * _capacity)) {
				rehash(_capacity * size_multiplier);
			}
			size_type index = key_to_index(key); // получаем индекс
			node<key_type, value_type> * cur_node = data[index];
			if (cur_node == nullptr) { // если ячейка пустая
				data[index] = new node<key_type, value_type>(key, value_type(std::forward<elements>(elem)...));
				_size++;
				return iterator(this, index); // возвращаем итератор на вставленный элемент
			}
			while (cur_node->next() != nullptr) { // ищем не занятую ячейку внутри уже существуещей
				cur_node = cur_node->next();
			}
			cur_node->_next = new node<key_type, value_type>(key, value_type(std::forward<elements>(elem)...));
			_size++;
			return iterator(this, cur_node->next()); // возвращаем итератор на вставленный элемент
		}

		// переход от ключа к индексу
		template <typename key_type>
		size_type key_to_index(const key_type & key) {
			return hash_f()(key) % _capacity;
		}

		size_type _capacity = CAPACITY; // размер таблицы
		size_type _size = 0; // текущее число элементов
		float rehash_coef = REHASH_COEF; // коэфициент при котором размер таблицы будет меняться
		float size_multiplier = SIZE_INC_MULTIPLIER; // коэффициент увеличения размера таблицы
		node<key_type, value_type> ** data; // данные
	};

};

#endif