#ifndef __SHTI__
#define __SHTI__

#ifdef USE_STD_CONTAINERS
	
#include <array>
#include <algorithm>
#include <vector>

#endif

#define CAPACITY 4
#define REHASH_COEF 1
#define SIZE_INC_MULTIPLIER 2

namespace shti {

	enum error_type {
		out_of_range,
	};

	template< typename key_type,
		      typename value_type, 
		      typename hash_f = std::hash<key_type>,
			  typename allocator = std::allocator<std::pair<key_type, value_type>>, 
		      typename size_type = std::size_t>
	class hash_table {
	private:
		// класс узла хеш таблицы
		template<typename key_type, typename value_type> class node {
			friend class hash_table;
		public:
			// конструктор класса
			node(key_type key, value_type value, node<key_type, value_type> * next = nullptr) 
				: _key(key), _value(value) , _next(next){}
/* ? */		node(std::pair<key_type, value_type> data) : _key(data.first), _value(data.second), _next(nullptr) { }
			node() {}

			// деструктор класса
			~node() {
				delete _next;
				_next = nullptr;
			}

			// возвращает ключ
			key_type key() { return _key; };

			// возвращает значение
			value_type & value() { return _value; }

			// оператор присваивания 
			node<key_type, value_type> & operator=(node<key_type, value_type> other) {
				_key = other._key;
				_value = other._value;
				_next = other._next;
				return *this;
			}

		private:
			key_type _key; // ключ 
			value_type _value; // занчение
			node<key_type, value_type> * _next = nullptr; // указатель на следующий элемент
		}; 

		using node_type = node<key_type, value_type>;
		using node_alloc = typename allocator::template rebind<node_type>::other;
		using node_pointer_alloc = typename allocator::template rebind<node_type*>::other;
		
		// выделение памяти под узлы таблицы
		node_type ** allocate_nodes(size_type count) {
			node_type **data = np_alloc.allocate(count);
			for (size_type i = 0; i < count; ++i) {
				data[i] = nullptr;
			}
			return data;
		}

		// освобождение памяти 
		void deallocate_nodes(node_type ** nodes, size_type count) {
			//np_alloc.destroy(nodes);
			np_alloc.deallocate(nodes, count);
		}

		// копирование памяти
		/*void copy_nodes(hash_table & tab) {
			for (size_type i = 0; i < tab._capacity; ++i) {

			}

		}*/

	public:

		// класс итератора
		template<typename key_type, typename value_type, typename table_type>
		class _iterator {
			friend table_type;
		protected: 
			// выполняет поиск следующей инициализированной ячейки
			void find_next_init_node() {
				while (_owner->data[_index] == nullptr && _index < _owner->_capacity - 1) {
					_index++; // пока не встретим инициализированую ячейку
				};
				cur_node = _owner->data[_index];
			}

		private:
			table_type * _owner = nullptr; // таблица которой принадлежит итератор
			size_type _index = 0; // текущий индекс
			node<key_type, value_type> * cur_node = nullptr; // указатель на текущий узел

			// конструктор класса
			explicit _iterator(table_type * owner, size_type index = 0) : _owner(owner), _index(index) {
				find_next_init_node();
			}
			explicit _iterator(table_type * owner, node<key_type, value_type> * _node, size_type pos = 0) 
				: _owner(owner), _index(pos) {
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
				return _owner == it._owner && _index == it._index && it.cur_node == cur_node;
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
				if (cur_node->_next != nullptr) {
					cur_node = cur_node->_next;
				}
				else {
					_index++;
					find_next_init_node();
				}
				return *this;
			}
			_iterator & operator++(int) {
				++(*this);
				return *this;
			}
		};

		using iterator = _iterator<key_type, value_type, hash_table>;
		using const_iterator = _iterator<const key_type, const value_type, const hash_table>;

		// конструктор класса
		hash_table() {
			data = allocate_nodes(_capacity);
		}
		hash_table(size_type capasity) {
			this->_capacity = capasity;
			data = allocate_nodes(_capacity);
		}
		hash_table(iterator begin, iterator end) {
			insert(begin, end);
		}
		hash_table(hash_table & _other) : hash_table(_other._capacity) {
			insert(_other.begin(), _other.end());
		}
		hash_table(hash_table && _other) {
			std::swap(_size, _other._size);
			std::swap(_capacity, _other._capacity);
			std::swap(data, _other.data);
			hasher = std::move(_other.hasher);
			np_alloc = std::move(_other.np_alloc);
			n_alloc = std::move(_other.n_alloc);
		}

		// деструктор класса
		~hash_table() {
			clear();
			deallocate_nodes(data, _capacity);
		}

		// операторы присвоения 
		hash_table & operator=(const hash_table & table) {
			if (this != table) {
				hash_table temp(table);
				swap(temp);
			}
			return *this;
		}
		hash_table & operator=(const hash_table && _other) {
			if (this != _other) {
				clear();
				deallocate_nodes(data, _capacity);
				std::swap(_size, _other._size);
				std::swap(_capacity, _other._capacity);
				std::swap(data, _other.data);
				hasher = std::move(_other.hasher);
				np_alloc = std::move(_other.np_alloc);
				n_alloc = std::move(_other.n_alloc);
			}
			return *this;
		}

		// итераторы указывающие на начало
		iterator begin() noexcept { return iterator(this); }
		const_iterator begin() const noexcept { return const_iterator(this); }
		const_iterator cbegin() const noexcept { return const_iterator(this); }

		// итераторы указывающие на конец
		iterator end() noexcept {
			return iterator(this, _capacity - 1);
		}
		const_iterator end() const noexcept {
			return const_iterator(this, _capacity - 1);
		}
		const_iterator cend() const noexcept {
			return const_iterator(this, _capacity - 1);
		}

		// выполняет вставку элемента в таблицу
		iterator insert(key_type key, const value_type & value) {
			return emplace_implementation(key, value);
		}
		iterator insert(key_type key, value_type && value) {
			return emplace_implementation(key, std::move(value));
		}
		iterator insert(iterator begin, iterator end) {
			for (; begin != end; ++begin) {
				insert(begin->key(), begin->value()); // добавляем элементы из списка
			}
			return begin;
		}
		iterator insert(std::pair<key_type, value_type> data) {
			return emplace_implementation(data.first, data.second);
		}
		iterator insert(std::initializer_list<std::pair<key_type, value_type>> data_list) {
			for (auto it = data_list.begin(); it != data_list.end(); it++) {
				insert(*it); // добавляем элементы из списка
			} 
			return find(data_list.end()->first);
		}
		template <typename... elem> iterator emplace(elem &&... _elem) {
			return emplace_implementation(std::forward<elem>(_elem)...);
		}

		// выполняет поиск элемента в таблице по ключу
		iterator find(const key_type &key) {
			return find_implementation(key); 
		}
		const_iterator find(const key_type &key) const {
			return find_implementation(key);
		}

		// оператор выдачи по индексу
		value_type & operator[](const key_type &key) {
			return find_implementation(key)->value();
		}

		// выдает элемент, но с проверкой
		value_type & at(const key_type & key) {
			return at_implementation(key);
		}
		const value_type & at(const key_type & key) const {
			return at_implementation(key);
		}

		// методы для удаления элементов
		size_type erase(const key_type & key) {
			size_type result = 0;
			auto it = find_implementation(key);
			while (it != end()) {
				if (it->key() == key) {
					erase_implementation(it);
					result++;
				}
				it = find_implementation(key);
			}
			return result;
		}
		iterator erase(iterator itr) {
			return erase_implementation(itr);
		}
		iterator erase(iterator begin, iterator end) {	
			while (begin != end) {
				begin = erase_implementation(begin);
			}
			return erase_implementation(begin);
		}

		// меняет местами с другой таблицей
		void swap(hash_table & ht) {
			std::swap(data, ht);
			std::swap(_capacity, ht._capacity);
			std::swap(_size, ht._size);
			std::swap(hasher, _other.hasher);
			std::swap(np_alloc, _other.np_alloc);
			std::swap(n_alloc, _other.n_alloc);
		}

		// возвращает колличество элементов
		size_type size() const noexcept { return _size; }

		// возвращает зарезервиврованное колличество элементов
		size_type capacity() const noexcept { return _capacity; }

		// очищает контенер
		void clear() {
			for (size_type i = 0; i < _capacity; ++i) {
				if (data[i] != nullptr) {
					n_alloc.destroy(data[i]);
					n_alloc.deallocate(data[i], 1);
					data[i] = nullptr;
				}
			}
			_size = 0;
		}
		
		// возвращает колличесво элементов с указанным ключем
		size_type count(const key_type &key) {
			size_type result = 0;
			size_type index = key_to_index(key); // получаем индекс элемента
			if (data[index] == nullptr) {
				return 0;
			}
			node<key_type, value_type> * cur_node = data[index];
			while (cur_node != nullptr) { // ищем не занятую ячейку внутри уже существуещей
				if (cur_node->key() == key) {
					result++;
				}
				cur_node = cur_node->_next;
			}
			return result;
		}

		// изменяет размер 
		void resize(size_type new_size) {
			if (new_size > _capacity) {
				rehash(new_size);
			}
			return;
		}

		// возвращает хеш функцию
		hash_f get_hash_func() const {
			return hasher;
		}

	private:
		// перераспределение элементов в таблице
		void rehash(size_type new_size) {
			size_type last_capacity = _capacity; 
			_capacity = new_size;
			node_type ** temp_data = allocate_nodes(new_size);
			_size = 0;
			std::swap(data, temp_data); // меняем местами области памяти
			for (size_type i = 0; i < last_capacity; ++i) {
				node_type * cur_node = std::move(temp_data[i]);
				while (cur_node != nullptr) {
					emplace_implementation(cur_node->key(), cur_node->value());
					cur_node = std::move(cur_node->_next);
				}
			}
			for (size_type i = 0; i < last_capacity; ++i) {
				if (temp_data[i] != nullptr) {
					n_alloc.destroy(temp_data[i]);
					n_alloc.deallocate(temp_data[i], 1);
					temp_data[i] = nullptr;
				}
			}
			deallocate_nodes(temp_data, last_capacity);
		}

		// реализация ставки элементов в таблицу
		template <typename _key, typename _value>
		iterator emplace_implementation(_key && key, _value && value) {
			if (_size + 1 > int(rehash_coef * _capacity)) {
				rehash(_capacity * size_multiplier);
			}
			size_type index = key_to_index(key); // получаем индекс
			node_type * cur_node = data[index];
			if (cur_node == nullptr) { // если ячейка пустая
				data[index] = n_alloc.allocate(1);
				n_alloc.construct(data[index], std::forward<_key>(key), std::forward<_value>(value));
				_size++;
				return iterator(this, index); // возвращаем итератор на вставленный элемент
			}
			while (cur_node->_next != nullptr) { // ищем не занятую ячейку внутри уже существуещей
				cur_node = cur_node->_next;
			}
			_size++;
			cur_node->_next = n_alloc.allocate(1);
			n_alloc.construct(cur_node->_next, std::forward<_key>(key), std::forward<_value>(value));
			return iterator(this, cur_node->_next); // возвращаем итератор на вставленный элемент
		}

		// реализация поиска элемента
		template <typename _key>
		iterator find_implementation(const _key & key) {
			size_type index = key_to_index(key); // получаем индекс элемента
			if (data[index] == nullptr) {
				return end();
			}
			node_type * cur_node = data[index];
			while (cur_node != nullptr) { // ищем не занятую ячейку внутри уже существуещей
				if (cur_node->key() == key) {
					return iterator(this, cur_node, index);
				}
				cur_node = cur_node->_next;
			}
			return end();
		}
		template <typename _key> 
		const_iterator find_implementation(const _key & key) const { 
			return const_cast<hash_table *>(this)->find_implementation(key);
		}

		// реализация выдачи по индексу
		template <typename _key>
		value_type & at_implementation(const _key & key) {
			iterator res = find_implementation(key);
			if (res == end()) {
				throw shti::error_type::out_of_range;
			}
			return res->value();
		}
		template <typename _key>
		const value_type & at_implementation(const _key & key) const {
			return const_cast<hash_table*>(this)->at_implementation(key);
		}

		// реализация удаления элементов
		iterator erase_implementation(iterator itr) {
			node_type * cur = data[itr._index]; // текущий
			node_type * perv = nullptr; // предыдущий узел 
			while (cur) {
				if (cur->key() == itr.cur_node->key()) {
					_size--;
					node_type * res;
					if (perv == nullptr) {
						data[itr._index] = cur->_next;
						res = data[itr._index];
					}
					else {
						perv->_next = cur->_next;
						res = cur->_next;
					}
					++itr;
					cur->_next = nullptr; 
					n_alloc.destroy(cur);
					n_alloc.deallocate(cur, 1);
					return itr;
				}
				perv = cur; 
				cur = cur->_next;
			}
			return end();
		}

		// переход от ключа к индексу
		template <typename key_type>
		size_type key_to_index(const key_type & key) {
			return hasher(key) % _capacity;
		}

		size_type _capacity = CAPACITY; // размер таблицы
		size_type _size = 0; // текущее число элементов
		float rehash_coef = REHASH_COEF; // коэфициент при котором размер таблицы будет меняться
		float size_multiplier = SIZE_INC_MULTIPLIER; // коэффициент увеличения размера таблицы
		node_type ** data; // данные
		hash_f hasher; // хеш функция 
		node_alloc n_alloc; 
		node_pointer_alloc np_alloc; 
	};

};

#endif