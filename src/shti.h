#ifndef __SHTI__
#define __SHTI__

namespace shti {

	enum error_type {
		out_of_range = 0,
		key_is_used,
		value_type_construct_error,
	};

	// политики изменения размера таблицы
	namespace rehash_policy {

		// базовый класс для плолитики изменения размера таблицы
		template <typename size_type>
		class base_rehash_policy {
		public:
			// возвращает новый размер 
			virtual size_type get_next_size(size_type requested, size_type capacity) const = 0;
			// проверяет необходимость изменения размера
			bool check_currient_size(size_type items_count, size_type capacity, float load_factor) const {
				return items_count > capacity * load_factor;
			}
			virtual ~base_rehash_policy() = default;
		};

		// стандартная политика изменения размера таблицы (удвоение)
		template <typename size_type>
		class default_rehash_policy : public base_rehash_policy<size_type> {
		public:
			// возвращает новый размер 
			size_type get_next_size(size_type requested, size_type capacity) const {
				return std::max(requested, capacity * 2);
			}
		};

		// плитика простых чисел
		template <typename size_type>
		class prime_rehash_policy : public base_rehash_policy<size_type> {
		private:

			static constexpr size_type size_table[] {
				17ul, 19ul, 23ul, 29ul, 31ul, 37ul, 43ul, 53ul, 
				61ul, 71ul, 79ul, 83ul, 103ul, 113ul, 137ul, 139ul,
				149ul, 157ul, 179ul, 199ul, 227ul, 241ul, 257ul, 277ul,
				293ul, 313ul, 359ul, 383ul, 467ul, 541ul, 619ul, 709ul,
				761ul, 823ul, 887ul, 953ul, 1031ul, 1109ul, 1289ul, 1493ul,
				1613ul, 1741ul, 2029ul, 2357ul, 2549ul, 2971ul, 3209ul, 
				3469ul, 3739ul, 4027ul, 4349ul, 4703ul, 5087ul, 5503ul, 
				5953ul, 6427ul, 6949ul, 8123ul, 8783ul, 9497ul, 10273ul,
				11113ul, 12983ul, 15173ul, 16411ul, 17749ul, 19183ul, 
				20753ul,22447ul, 24281ul, 26267ul, 28411ul, 30727ul, 
				33223ul, 38873ul, 45481ul, 49201ul, 53201ul, 57557ul, 
				62233ul, 67307ul, 72817ul, 78779ul, 85229ul, 92203ul, 
				99733ul, 116731ul, 136607ul, 147793ul, 159871ul, 172933ul, 
				187091ul, 218971ul, 236897ul, 256279ul, 299951ul,324503ul, 
				351061ul, 379787ul, 410857ul, 444487ul, 480881ul, 562841ul,
				608903ul, 658753ul, 771049ul, 834181ul, 902483ul, 976369ul,
				1056323ul, 1142821ul, 1236397ul, 1337629ul, 1447153ul, 1565659ul, 
				1693859ul, 1982627ul, 2320627ul, 2510653ul, 2716249ul, 2938679ul, 
				3179303ul, 3439651ul, 3721303ul, 4026031ul, 4355707ul, 5098259ul, 
				5967347ul, 6456007ul, 6984629ul, 7556579ul, 8175383ul, 8844859ul, 
				9569143ul, 10352717ul, 11200489ul, 13109983ul, 15345007ul, 16601593ul, 
				17961079ul, 19431899ul, 21023161ul, 22744717ul, 24607243ul, 28802401ul, 
				33712729ul, 36473443ul, 39460231ul, 42691603ul, 46187573ul, 49969847ul,
				54061849ul, 63278561ul, 74066549ul, 80131819ul, 86693767ul, 93793069ul, 
				101473717ul, 109783337ul, 118773397ul, 139022417ul, 162723577ul, 176048909ul,
				190465427ul, 206062531ul, 222936881ul, 241193053ul, 260944219ul, 305431229ul, 
				357502601ul, 386778277ul, 418451333ul, 452718089ul, 489790921ul, 529899637ul, 
				573292817ul, 671030513ul, 785430967ul, 849749479ul, 919334987ul, 994618837ul, 
				1076067617ul, 1164186217ul, 1259520799ul, 1362662261ul, 1474249943ul, 
				1594975441ul, 1725587117ul, 1866894511ul, 2019773507ul, 2185171673ul, 
				2364114217ul, 2557710269ul, 2767159799ul, 2993761039ul, 3238918481ul, 
				3504151727ul, 3791104843ul, 4101556399ul, 4294967291ul,
			};

		public:
			// возвращает новый размер 
			size_type get_next_size(size_type requested, size_type capacity) const {
				for (size_type res : size_table) {
					if (res > requested) {
						return res;
					}
				}
				if (requested > std::numeric_limits<size_type>::max() / 2) {
					return std::numeric_limits<size_type>::max();
				}
				return static_cast<size_type>(requested * 1.5);
			}
		};

		// политика изменения размера с фиксированным шагом 
		template <typename size_type, size_type step = 128>
		class fixed_step_policy : public base_rehash_policy<size_type> {
		public:
			// возвращает новый размер 
			size_type get_next_size(size_type requested, size_type capacity) const {
				size_type new_size = capacity;
				while (new_size < requested) {
					if (new_size > std::numeric_limits<size_type>::max() / 2) {
						return std::numeric_limits<size_type>::max();
					}
					new_size += step;
				}
				return new_size;
			}
		};
	};

	// базовый класс хеш таблицы
	template< typename key_type,
		typename T,
		typename hash_f = std::hash<key_type>,
		typename comp = std::equal_to<key_type>,
		typename allocator = std::allocator<std::pair<key_type, T>>,
		typename size_type = std::size_t, 
		typename rehash_p = rehash_policy::default_rehash_policy<size_type>>
		class basic_hash_table {
		using value_pair = std::pair<const key_type, T>;
		protected:

			// класс узла хеш таблицы
			template<typename key_type, typename T>
			struct node {
			public:
				// конструктор класса
				node(key_type _key, T _value, node * _next = nullptr)
					: data(_key, _value), next(_next) {}
				node() {}

				// деструктор класса
				~node() {
					delete next;
					next = nullptr;
				}

				// оператор присваивания 
				node & operator=(node& other) noexcept {
					data.first = other.first;
					data.second = other.second;
					next = other.next;
					return *this;
				}

				node& operator=(node && other) noexcept {
					delete next;
					data.first = std::move(other.first);
					data.second = std::move(other.second);
					next = other.next;
					other.next = nullptr;
					return *this;
				}

				std::pair<const key_type, T> data; // данные
				node * next; // значение

			};

			using node_type = node<key_type, T>;
			using node_alloc = typename allocator::template rebind<node_type>::other;
			using node_pointer_alloc = typename allocator::template rebind<node_type*>::other;

			node_type ** data; // данные

		private:
			// выделение памяти под узлы таблицы
			node_type ** allocate_nodes(size_type count) {
				node_type **data = np_alloc.allocate(count);
				std::memset(data, 0, count * sizeof(node_type*));
				return data;
			}

			// освобождение памяти 
			void deallocate_nodes(node_type ** nodes, size_type count) {
				np_alloc.deallocate(nodes, count);
			}

		public:

			// класс итератора
			template<typename table_t, bool _const>
			class _iterator {
				friend class _iterator<table_t, true>;
				friend class _iterator<table_t, false>;
			public:
				// для совместимости с STL
				using iterator_category = std::forward_iterator_tag;
				using difference_type = std::ptrdiff_t;
				using value_type = std::pair<const key_type, T>;
				using pointer = std::conditional_t<_const, const value_type *, value_type *>;
				using reference = std::conditional_t<_const, const value_type &, value_type &>;

				using table_type = std::conditional_t<_const, const table_t, table_t>;
				using node_pointer = std::conditional_t<_const, const node_type *, node_type *>;

			private:
				table_type * _owner = nullptr; // таблица которой принадлежит итератор
				size_type _index = 0; // текущий индекс
				node_pointer cur_node = nullptr; // указатель на текущий узел

				// выполняет поиск следующей инициализированной ячейки
				void find_next_init_node() {
					if (!_owner) {
						return;
					}
					while (_owner->data[_index] == nullptr && _index < _owner->_capacity) {
						_index++; // пока не встретим инициализированую ячейку
					};
					cur_node = (_index < _owner->_capacity) ? _owner->data[_index] : nullptr;
				}

			public:
				// конструктор класса
				_iterator() = default;
				_iterator(table_type * owner, size_type index = 0) : _owner(owner), _index(index) {
					find_next_init_node();
				}
				_iterator(table_type * owner, node_pointer _node, size_type pos = 0)
					: _owner(owner), _index(pos), cur_node(_node) {}

				template<bool iter_const, typename = std::enable_if_t<_const || !iter_const>>
				_iterator(const _iterator<table_t, iter_const> & _iter)
					: _owner(_iter._owner), _index(_iter._index), cur_node(_iter.cur_node) {}

				// оператры сравнения
				template<bool oth_const>
				bool operator==(const _iterator<table_t, oth_const> & it) const {
					return _owner == it._owner && _index == it._index && it.cur_node == cur_node;
				}
				template<bool oth_const>
				bool operator!=(const _iterator<table_t, oth_const> & it) const {
					return !(*this == it);
				}

				reference operator*() const {
					return cur_node->data;
				}

				pointer operator->() const {
					return &cur_node->data;
				}

				// инкремент
				_iterator & operator++() {
					if (cur_node->next != nullptr) {
						cur_node = cur_node->next;
					}
					else {
						_index++;
						find_next_init_node();
					}
					return *this;
				}
				_iterator & operator++(int) {
					_iterator temp = *this;
					++(*this);
					return temp;
				}

				node_pointer get_node() const { return cur_node; }
				table_type * get_table() const { return _owner; }
				size_type get_index() const { return _index; }
				bool valid() const { return cur_node != nullptr; }
			};

			using iterator = _iterator<basic_hash_table, false>;
			using const_iterator = _iterator<basic_hash_table, true>;

			// конструктор класса
			basic_hash_table() {
				hash_policy = new rehash_p();
				data = allocate_nodes(_capacity);
			}
			basic_hash_table(size_type capasity) {
				this->_capacity = capasity;
				hash_policy = new rehash_p();
				data = allocate_nodes(_capacity);
			}
			basic_hash_table(const basic_hash_table & _other) {
				this->_capacity = _other._capacity;
				np_alloc = _other.np_alloc;
				hash_policy = new rehash_p(*_other.hash_policy);
				data = allocate_nodes(_capacity);
			}
			basic_hash_table(basic_hash_table && _other) {
				std::swap(_size, _other._size);
				std::swap(_capacity, _other._capacity);
				std::swap(data, _other.data);
				hasher = std::move(_other.hasher);
				hash_policy = std::move(_other.hash_policy);
				np_alloc = std::move(_other.np_alloc);
				n_alloc = std::move(_other.n_alloc);
			}
			template<typename rh_policy> 
			basic_hash_table(rh_policy && _policy) {
				data = allocate_nodes(_capacity);
				hash_policy = new rh_policy(_policy);
			}

			// деструктор класса
			~basic_hash_table() {
				clear();
				deallocate_nodes(data, _capacity);
				delete hash_policy;
			}

			// операторы присвоения 
			basic_hash_table & operator=(const basic_hash_table & table) {
				if (this != table) {
					basic_hash_table temp(table);
					swap(temp);
				}
				return *this;
			}
			basic_hash_table & operator=(const basic_hash_table && _other) {
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
			iterator end() noexcept { return iterator(this, _capacity - 1); }
			const_iterator end() const noexcept { return const_iterator(this, _capacity - 1); }
			const_iterator cend() const noexcept { return const_iterator(this, _capacity - 1); }

			// выполняет вставку элемента в таблицу
			std::pair<iterator, bool> insert(const value_pair & value) {
				return emplace(value.first, value.second);
			}
			std::pair<iterator, bool> insert(value_pair && value) {
				return emplace(std::move(value.first), std::move(value.second));
			}
			template<typename iter_t>
			void insert(iter_t begin, iter_t end) {
				for (auto it = begin; it != end; ++it) {
					insert(*it); // добавляем элементы из списка
				}
			}
			iterator insert(std::initializer_list<value_pair> data_list) {
				for (auto it = data_list.begin(); it != data_list.end(); it++) {
					emplace(it->first, it->second); // добавляем элементы из списка
				}
				return find(data_list.end()->first);
			}
			template <typename K, typename V>
			std::pair<iterator, bool> emplace(K && _key, V && _value) {
				size_type index = key_to_index(_key); // получаем индекс
				if (valid_key(_key, index)) {
					if (hash_policy->check_currient_size(_size + 1, _capacity, rehash_coef)) {
						rehash(hash_policy->get_next_size(_size + 1, _capacity));
					}
					_size++;
					node_type * new_node = n_alloc.allocate(1);
					try {
						n_alloc.construct(new_node, _key, _value, std::move(data[index]));
					}
					catch (...) {
						n_alloc.deallocate(new_node, 1);
						throw error_type::value_type_construct_error;
					}
					data[index] = new_node;
					return{ iterator(this, new_node, index), true }; // возвращаем итератор на вставленный элемент
				}
				return{ find(_key), false }; // возвращаем итератор на существующий элемент
			}

			// выполняет поиск элемента в таблице по ключу
			iterator find(const key_type &key) {
				size_type index = key_to_index(key); // получаем индекс элемента
				node_type * cur_node = data[index];
				while (cur_node != nullptr) { // ищем не занятую ячейку внутри уже существуещей
					if (comparator(cur_node->data.first, key)) {
						return iterator(this, cur_node, index);
					}
					cur_node = cur_node->next;
				}
				return end();
			}
			const_iterator find(const key_type &key) const {
				return const_cast<basic_hash_table *>(this)->find(key);
			}

			// оператор выдачи по индексу
			T & operator[](const key_type &key) {
				return find(key)->second;
			}

			// выдает элемент, но с проверкой
			T & at(const key_type & key) {
				iterator res = find(key);
				if (res == end()) {
					throw shti::error_type::out_of_range;
				}
				return res->second;
			}
			const T & at(const key_type & key) const {
				return const_cast<basic_hash_table *>(this)->at(key);
			}

			// методы для удаления элементов
			size_type erase(const key_type & key) {
				size_type result = 0;
				size_type index = key_to_index(key);
				node_type * cur = data[index];
				node_type * perv = nullptr; // предыдущий узел 
				while (cur) {
					if (comparator(cur->data.first, key)) {
						cur = remove_node(cur, perv, index);
						result++;
					}
					else {
						perv = cur;
						cur = cur->next;
					}
				}
				return result;
			}
			iterator erase(const_iterator itr) {
				if (itr == end() || itr.get_table() != this || !itr.valid()) {
					return end();
				}
				node_type * cur = data[itr.get_index()]; // текущий
				node_type * perv = nullptr; // предыдущий узел 
				while (cur) {
					if (cur->data.first == itr->first) {
						cur = remove_node(cur, perv, itr.get_index());
						if (cur) {
							return iterator(this, cur, itr.get_index());
						}
						else {
							return iterator(this, itr.get_index() + 1);
						}
					}
					perv = cur;
					cur = cur->next;
				}
				return end();
			}
			iterator erase(const_iterator begin, const_iterator end) {
				if (begin == end) {
					return this->end();
				}
				if (begin.get_table() != this || end.get_table() != this) {
					return this->end();
				}
				auto cur = begin;
				while (cur != end) {
					cur = erase(cur);
				}
				return iterator(this, const_cast<node_type *>(end.get_node()), end.get_index());
			}

			// меняет местами с другой таблицей
			void swap(basic_hash_table & ht) {
				std::swap(data, ht.data);
				std::swap(_capacity, ht._capacity);
				std::swap(_size, ht._size);
				std::swap(hasher, ht.hasher);
				std::swap(np_alloc, ht.np_alloc);
				std::swap(n_alloc, ht.n_alloc);
			}

			// возвращает колличество элементов
			size_type size() const noexcept { return _size; }

			// возвращает зарезервиврованное колличество элементов
			size_type capacity() const noexcept { return _capacity; }

			// возвращает true если элемент пустой
			bool empty() { return _size == 0; }

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
				node_type * cur_node = data[index];
				while (cur_node != nullptr) { // ищем не занятую ячейку внутри уже существуещей
					if (comparator(cur_node->key, key)) {
						result++;
					}
					cur_node = cur_node->next;
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

			// возвращает текущий компаратор
			comp get_equal() const {
				return comparator;
			}

			// Возвращает аллокатор
			node_pointer_alloc get_allocator() const {
				return np_alloc;
			}

			// возращает текущий коэффициент загруженности
			float load_factor() const{
				return float(_size / _capacity);
			}

			// возращает максимальный коэффициент загруженности
			float max_load_factor() const {
				return rehash_coef
			}

			// устанавливает максимальный коэффициент загруженности
			void max_load_factor(float factor) {
				if (factor > 1 || factor < 0) {
					return;
				}
				rehash_coef = factor;
			}

			// возвращает текущюю политику хеширования
			rehash_policy::base_rehash_policy * get_rehash_policy() const{
				return hash_policy;
			}

			// устанавливает политику хеширования
			template<typename policy>
			void set_rehash_policy(policy _policy) {
				if (hash_policy) {
					delete hash_policy;
				}
				hash_policy = new rehash_p(_policy);
			}

		protected:
			// возвращает true если можно выполнить вставку
			virtual bool valid_key(const key_type & key, size_type index) = 0;

			// удаляет конкретный узел 
			node_type * remove_node(node_type * cur, node_type * perv, size_type index) {
				node_type * next;
				node_type * target = cur; // цель для удаления
				if (perv == nullptr) {
					data[index] = cur->next;
				}
				else {
					perv->next = cur->next;
				}
				next = cur->next;
				target->next = nullptr;
				n_alloc.destroy(target);
				n_alloc.deallocate(target, 1);
				target = nullptr;
				_size--;
				return next;
			}

			// перераспределение элементов в таблице
			void rehash(size_type new_size) {
				if (new_size == _capacity) {
					return;
				}
				size_type last_capacity = _capacity;
				_capacity = new_size;
				node_type ** temp_data = allocate_nodes(new_size);
				std::swap(data, temp_data); // меняем местами области памяти
				node_type * cur_node = nullptr; // текущий узел
				node_type * next = nullptr; // следующий узел в цепочку
				for (size_type i = 0; i < last_capacity; ++i) {
					cur_node = temp_data[i];
					while (cur_node != nullptr) {
						next = cur_node->next;
						size_type index = key_to_index(cur_node->data.first);
						cur_node->next = data[index];
						data[index] = cur_node;
						cur_node = next;
					}
				}
				deallocate_nodes(temp_data, last_capacity);
			}

			// переход от ключа к индексу
			template <typename key_type>
			size_type key_to_index(const key_type & key) {
				return hasher(key) % _capacity;
			}

			size_type _capacity = 4; // размер таблицы
			size_type _size = 0; // текущее число элементов
			float rehash_coef = 0.8; // коэфициент при котором размер таблицы будет меняться
			hash_f hasher; // хеш функция 
			node_alloc n_alloc;
			node_pointer_alloc np_alloc;
			rehash_p * hash_policy;
			comp comparator; // компаратор
	};

	// хеш таблица
	template< typename key_type,
		typename T,
		typename hash_f = std::hash<key_type>,
		typename comp = std::equal_to<key_type>,
		typename allocator = std::allocator<std::pair<key_type, T>>,
		typename size_type = std::size_t,
		typename rehash_p = rehash_policy::default_rehash_policy<size_type >>
		class hash_table : protected basic_hash_table<key_type, T, hash_f, allocator, size_type, rehash_p> {

		using base_table = basic_hash_table<key_type, T, comp, hash_f, allocator, size_type, rehash_p>;

		public:
			using base_table::begin;
			using base_table::end;
			using base_table::cbegin;
			using base_table::cend;
			using base_table::find;
			using base_table::insert;
			using base_table::emplace;
			using base_table::erase;
			using base_table::operator=;
			using base_table::operator[];
			using base_table::at;
			using base_table::swap;
			using base_table::size;
			using base_table::capacity;
			using base_table::empty;
			using base_table::clear;
			using base_table::count;
			using base_table::resize;
			using base_table::get_hash_func;
			using base_table::get_allocator;

			using base_table::_iterator;

			using iterator = _iterator<basic_hash_table, false>;
			using const_iterator = _iterator<basic_hash_table, true>;

			// конструктор класса
			hash_table() : basic_hash_table() { };
			hash_table(size_type capasity) : basic_hash_table(capasity) { }
			//	hash_table(iterator begin, iterator end) : basic_hash_table(begin, end) { }
			hash_table(const hash_table & _other) :
				basic_hash_table(_other) {
				insert(_other.begin(), _other.end());
			}
			hash_table(hash_table && _other) : basic_hash_table(std::move(_other)) { }

			~hash_table(){}

		protected:

			using base_table::node;
			using base_table::node_type;
			using base_table::data;

			bool valid_key(const key_type & key, size_type index) {
				node_type* result = data[index];
				while (result) {
					if (result->data.first == key) {
						return false;
					}
					result = result->next;
				}
				return true;
			}

	};

	// хеш таблица допускающая хранение значений под одинаковыми ключами
	template< typename key_type,
		typename T,
		typename hash_f = std::hash<key_type>,
		typename comp = std::equal_to<key_type>,
		typename allocator = std::allocator<std::pair<key_type, T>>,
		typename size_type = std::size_t,
		typename rehash_p = rehash_policy::default_rehash_policy<size_type >>
		class hash_multitable : protected basic_hash_table<key_type, T, hash_f, allocator, size_type, rehash_p> {

		using base_table = basic_hash_table<key_type, T, comp, hash_f, allocator, size_type, rehash_p>;

		public:
			using base_table::begin;
			using base_table::end;
			using base_table::cbegin;
			using base_table::cend;
			using base_table::find;
			using base_table::insert;
			using base_table::emplace;
			using base_table::erase;
			using base_table::operator=;
			using base_table::operator[];
			using base_table::at;
			using base_table::swap;
			using base_table::size;
			using base_table::capacity;
			using base_table::empty;
			using base_table::clear;
			using base_table::count;
			using base_table::resize;
			using base_table::get_hash_func;
			using base_table::get_allocator;

			using base_table::_iterator;

			using iterator = _iterator<basic_hash_table, false>;
			using const_iterator = _iterator<basic_hash_table, true>;

			// конструктор класса
			hash_multitable() : basic_hash_table() { };
			hash_multitable(size_type capasity) : basic_hash_table(capasity) { }
			//	hash_multitable(iterator begin, iterator end) : basic_hash_table(begin, end) { }
			hash_multitable(const hash_multitable & _other) :
				basic_hash_table(_other) {
				insert(_other.begin(), _other.end());
			}
			hash_multitable(hash_multitable && _other) : basic_hash_table(std::move(_other)) { }

			~hash_multitable()  {}

		protected:

			using base_table::node;
			using base_table::node_type;
			using base_table::data;

			bool valid_key(const key_type & key, size_type index) {
				return true;
			}
	};

};

#endif