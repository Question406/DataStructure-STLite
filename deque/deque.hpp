#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP
 
#include "exceptions.hpp"
 
#include <cstddef>
 
using namespace std;
 
namespace sjtu {
 
	template<class T>
	class deque {
	private:
		class node {
		public:
			T *data;
			node *next, *prev;
 
			node() {
				data = NULL; next = NULL; prev = NULL;
			}
			node(const T &_data, node * _prev = nullptr, node *_next = nullptr) {
				data = new T(_data);
				next = _next;
				prev = _prev;
			}
			node(node *_prev, node *_next) {
				data = nullptr;
				next = _next;
				prev = _prev;
			}
			~node() { delete data; }
		};
		class Node {
		public:
			size_t size, nodeSize;
			node *head, *tail;
			Node *prev, *next;
 
			Node(const Node &other, Node *_prev, Node *_next) {
				size = other.size;
				nodeSize = other.nodeSize;
				head = new node();
				node *p = head, *q = other.head->next;
				while (q != other.tail) {
					p->next = new node(*(q->data), p, NULL);
					q = q->next;
					p = p->next;
				}
				tail = new node(p, NULL);
				p->next = tail;
				prev = _prev; next = _next;
			}
 
			Node(Node *_prev = nullptr, Node *_next = nullptr) {
				nodeSize = 300;
				size = 0;  head = new node(); tail = new node(head, NULL); head->next = tail;
				prev = _prev; next = _next;
			}
			Node(node *_head, node *_tail, size_t s, Node *pre, Node *ne) {
				nodeSize = 300;	
				head = _head;	
				tail = _tail;
				size = s;
				prev = pre;
				next = ne;
			}
			~Node() {
				node *p = head->next, *q;
				while (p != NULL) {
					q = p->next;
					delete p;
					p = q;
				}
				delete head;
			}
 
			void getPos(size_t pos, node *&p) {
				if (pos < size / 2) {
					p = head->next;
					while (pos > 0) {
						pos--;
						p = p->next;
					}
				}
				else {
					pos = size - pos;
					p = tail;
					while (pos > 0) {
						pos--;
						p = p->prev;
					}
				}
			}
 
			void insert(size_t pos, const T &value) {
				/*if (pos < size / 2) {
					node *p;
					getPos(pos, p);
 
					node *q = p->next;
					p->next = new node(value, p, q);
					q->prev = p->next;
 
					++size;
				}
				else {*/
					node *p;
					getPos(pos, p);
 
					node *q = p->prev;
					p->prev = new node(value, q, p);
					q->next = p->prev;
					++size;
				//}
			}
 
			void push_back(const T &value) {
				++size;
				node *q = tail->prev;
				q->next = new node(value, q, tail);
				tail->prev = q->next;
 
				if (size > nodeSize) {
					split(size - 1);
					next->merge();
				}
			}
			void push_front(const T &value) {
				node *p = head->next;
				head->next = new node(value, head, p);
				p->prev = head->next;
				//insert(0, value);
				++size;
				if (size > nodeSize) {
					split(1);
					size = 1;
					prev->merge();
					return;
				}
			}
 
			void erase(size_t pos) {
				node *p;
				getPos(pos, p);
				--size;
				p->prev->next = p->next;
				p->next->prev = p->prev;
				delete p;
			}
 
			void pop_front() {
				erase(0);
			}
 
			void pop_back() {
				erase(size - 1);
			}
 
			void split(size_t pos) {
				node *p;
				getPos(pos, p);
				node *q = p->prev;
				node *head2, *tail2;
 
				tail2 = new node(q, NULL);
				q->next = tail2;
 
				head2 = new node();
				head2->prev = NULL;
				head2->next = p;
 
				p->prev = head2;
 
				next = new Node(head2, tail, size - pos, this, next);
				next->next->prev = next;
 
 
				tail = tail2;
				size = pos;
			}
 
			bool mergeable() {
				return !(this == NULL || size + next->getsize() > nodeSize || next->next == NULL);
			}
 
			void merge() {
				if (this == NULL || size + next->getsize() > nodeSize || next->next == NULL) return;
 
				if (next->getsize() == 0) {
					Node *rNode = next;
					next = rNode->next;
					rNode->next->prev = this;
					delete rNode;
					return;
				}
 
				Node *rNode = next;
				node *llast = tail->prev, *rfirst = rNode->head->next, *rlast = rNode->tail->prev;
				size += next->getsize();
 
				llast->next = rfirst;
				rfirst->prev = llast;
 
				rNode->head->next = rNode->tail;
				rNode->tail->prev = rNode->head;
				next = rNode->next;
				rNode->next->prev = this;
				delete rNode;
 
				tail->prev = rlast;
				rlast->next = tail;
 
			}
 
			T &get(size_t pos) {
				node *p;
				getPos(pos, p);
 
				return *p->data;
			}
 
			T &operator[](const size_t &pos) {
				return get(pos);
			}
 
			size_t getsize() {
				return size;
			}
		};
		class block {
		public:
			size_t nodeSize, len;
			Node *head, *tail;
 
		public:
			block() {
				len = 0;
				nodeSize = 300;
				head = new Node();
				head->next = new Node(head, NULL);
				tail = new Node(head->next, NULL);
				head->next->next = tail;
				head->size = tail->size = nodeSize;
			}
			block(const block &other) {
				len = other.len;
				nodeSize = other.nodeSize;
				head = new Node(*other.head, NULL, NULL);
				Node *p = head, *q = other.head->next;
				while (q != other.tail) {
					p->next = new Node(*q, p, NULL);
					p = p->next;
					q = q->next;
				}
				tail = new Node(p, NULL); tail->size = nodeSize;
				p->next = tail;
			}
			~block() {
				clear();
				delete head->next;
				delete head;
				delete tail;
			}
 
			size_t size() { return len; }
			void clear() {
				Node *p = head->next, *q;
				while (p != tail && p != NULL) {
					q = p->next;
					delete p;
					p = q;
				}
				head->next = new Node(head, tail);
				tail->prev = head->next;
				len = 0;
			}
			void push_back(const T &value) {
				tail->prev->push_back(value);
				++len;
			}
			void pop_back() {
				tail->prev->pop_back();
				--len;
			}
			void push_front(const T &value) {
				head->next->push_front(value);
				++len;
			}
			void pop_front() {
				head->next->pop_front();
				--len;
			}
			bool empty() {
				return len == 0;
 
			}
 
			void findPos(size_t start, Node *&p, size_t &pos) {
				if (start == 0) {
					pos = 0;
					p = head->next;
					return;
				}
				size_t hcount = 0;
 
				p = head->next;
				while (hcount < start) {
					if (p == tail) throw(invalid_iterator());
					if (hcount + p->size < start) {
						hcount += p->size;
						p = p->next;
					}
					else {
						if (hcount + p->size == start) {
							pos = start - hcount - p->size;
							p = p->next;
							if (p == tail) throw(invalid_iterator());
							return;
						}
						pos = start - hcount;
						return;
					}
				}
			}
 
			T &get(size_t pos) {
				Node *p;
				size_t t;
				findPos(pos, p, t);
				return (*p)[t];
			}
 
			void traverse() {
				Node *p = head->next;
				while (p != tail) {
					for (size_t i = 0; i < p->getsize(); ++i)
					//	cout << (*p)[i] << ' ';
					p = p->next;
				}
			//	cout << endl;
			}
 
		};
 
		block *blk;
 
	public:
		class const_iterator;
		class iterator {
			friend class deque;
		private:
			Node *itr;
			size_t pos;
			block *me;
 
		public:
			iterator() { itr = NULL; }
			iterator(Node *_itr, size_t _pos, block *_me) {
				itr = _itr;
				pos = _pos;				
				me = _me;
			}
			iterator &operator=(const iterator &other) {
				itr = other.itr;
				pos = other.pos;
				me = other.me;
				return *this;
			}
			iterator(const iterator &other) {
				itr = other.itr;
				pos = other.pos;
				me = other.me;
			}
 
			/**
			 * return a new iterator which pointer n-next elements
			 *   even if there are not enough elements, the behaviour is **undefined**.
			 * as well as operator-
			 */
			iterator operator+(const int &n) const {
				iterator temp(*this);
				if (n < 0) {
					temp = temp - (-n);
					return temp;
				}
				if (n == 0) return temp;
				++temp;
				size_t count = 0;
				while (count < n && temp.itr != NULL) {
					if (count + temp.itr->size - temp.pos >= n) {
						temp.pos += n - count - 1;
						return temp;
					}
					else {
						count += temp.itr->size - temp.pos;
						temp.itr = temp.itr->next;
						temp.pos = 0;
					}
				}
			}
 
			iterator operator-(const int &n) const {
				iterator temp(*this);
				if (n < 0) {
					temp = temp + (-n);
					return temp;
				}
				if (n == 0) return temp;
				--temp;
				size_t count = 0;
				while (count < n && temp.itr != NULL) {
					if (count + temp.pos + 1 >= n) {
						temp.pos -= (n - count) - 1;
						return temp;
					}
					else {
						count += temp.pos + 1;
						temp.itr = temp.itr->prev;
						temp.pos = temp.itr->size - 1;
					}
				}
				return temp;
			}
			// return th distance between two iterator,
			// if these two iterators points to different vectors, throw invaild_iterator.
 
			size_t getpos() {
				Node *p = itr;
				size_t t = pos;
				size_t count = t;
				while (p->prev != NULL) {
					count += p->size;
					p = p->prev;
				}
				return count;
			}
 
			int operator-(const iterator &rhs) const {
				if (me != rhs.me) throw(invalid_iterator());
 
				Node *p = itr;
				size_t t = pos;
				size_t count1 = t;
				if (p->prev != NULL) p = p->prev;
				while (p->prev != NULL) {
					count1 += p->size;
					p = p->prev;
				}
 
				p = rhs.itr;
				t = rhs.pos;
				size_t count2 = t;
				if (p->prev != NULL) p = p->prev;
				while (p->prev != NULL) {
					count2 += p->size;
					p = p->prev;
				}
				return count1 - count2;
			}
			iterator operator+=(const int &n) {
				*this = (*this) + n;
				return *this;
			}
			iterator operator-=(const int &n) {
				*this = (*this) - n;
				return *this;
			}
			/**
			 * TODO iter++
			 */
			iterator operator++(int) {
				iterator temp(*this);
				if (pos == itr->getsize() - 1) {
					pos = 0;
					itr = itr->next;
				}
				else pos++;
				return temp;
			}
			/**
			 * TODO ++iter
			 */
			iterator& operator++() {
				if (pos == itr->getsize() - 1) {
					pos = 0;
					itr = itr->next;
				}
				else pos++;
				return *this;
			}
			/**
			 * TODO iter--
			 */
			iterator operator--(int) {
				iterator temp(*this);
				if (pos == 0) {
					itr = itr->prev;
					pos = itr->getsize() - 1;
				}
				else pos--;
				return temp;
			}
			/**
			 * TODO --iter
			 */
			iterator& operator--() {
				if (pos == 0) {
					itr = itr->prev;
					pos = itr->getsize() - 1;
				}
				else pos--;
				return *this;
			}
			/**
			 * TODO *it
			 */
			T& operator*() const {
				if (itr->prev == NULL || itr->next == NULL) throw(invalid_iterator());
				return (*itr)[pos];
			}
			/**
			 * TODO it->field
			 */
			T* operator->() const noexcept {
				node *p;
				itr->getPos(pos, p);
				return p->data;
			}
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator &rhs) const {
				return (itr == rhs.itr && pos == rhs.pos);
			}
			bool operator==(const const_iterator &rhs) const {
				return (itr == rhs.itr && pos == rhs.pos);
			}
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const {
				return !(*this == rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this == rhs);
			}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
	private:
		iterator itr;
 
	public:
		const_iterator() {}
		const_iterator(const const_iterator &other) {
			itr = other.itr;
		}
		const_iterator(const iterator &other) {
			itr = other;
		}
		const_iterator(Node *_itr, size_t _pos, block *_me) {
			itr = iterator(_itr, _pos, _me);
		}
		const_iterator operator+(const int &n) const {
			const_iterator temp(*this);
			temp.itr += n;
			return temp;
		}
		const_iterator operator-(const int &n) const {
			const_iterator temp(*this);
			temp.itr -= n;
			return temp;
		}
		int operator-(const const_iterator &rhs) const {
			return (itr - rhs.itr);
		}
		const_iterator operator+=(const int &n) {
			itr += n;
			return *this;
		}
		const_iterator operator-=(const int &n) {
			itr -= n;
			return *this;
		}
		const_iterator operator++(int) {
			const_iterator temp(*this);
			++temp.itr;
			return temp;
		}
		/**
		 * TODO ++iter
		 */
		const_iterator& operator++() {
			++itr;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		const_iterator operator--(int) {
			const_iterator temp(*this);
			--temp.itr;
			return temp;
		}
		/**
		 * TODO --iter
		 */
		const_iterator& operator--() {
			--itr;
			return *this;
		}
		/**
		 * TODO *it
		 */
		const T& operator*() const {
			return *itr;
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
			node *p;
			size_t pos = itr.pos;
			itr.itr->getPos(pos, p);
			return p->data;
		}
		bool operator==(const iterator &rhs) const {
			return (itr == rhs);
		}
		bool operator==(const const_iterator &rhs) const {
			return (itr == rhs.itr);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
 
	};
	/**
	 * TODO Constructors
	 */
	deque() { blk = new block(); }
	deque(const deque &other) {
		blk = new block((*other.blk));
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() { if (blk) delete blk; }
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {
		if (this == &other) return *this;
		delete blk;
		blk = new block(*(other.blk));
		return *this;
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
		return blk->get(pos);
	}
	const T & at(const size_t &pos) const {
		return blk->get(pos);
	}
	T & operator[](const size_t &pos) {
		return blk->get(pos);
	}
	const T & operator[](const size_t &pos) const {
		return blk->get(pos);
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		if (blk->empty()) throw(container_is_empty());
		return blk->get(0);
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		if (blk->empty()) throw(container_is_empty());
		return blk->get(blk->size() - 1);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		return iterator(blk->head->next, 0, blk);
	}
	const_iterator cbegin() const {
		return const_iterator(blk->head->next, 0, blk);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		if (blk->len != 0) return iterator(blk->tail, 0, blk);
		else return iterator(blk->head->next, 0, blk);
	}
	const_iterator cend() const {
		if (blk->len != 0) return const_iterator(blk->tail, 0, blk);
		else return const_iterator(blk->head->next, 0, blk);
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
		return blk->empty();
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return blk->size();
	}
	/**
	 * clears the contents
	 */
	void clear() {
		blk->clear();
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
		Node *p = pos.itr;
		size_t t = pos.pos;
		if (blk != pos.me || p == blk->head || (p == blk->tail && t != 0) || t > p->size || t < 0) throw(invalid_iterator());
 
		if (t == 0 && p == blk->tail) {
			blk->push_back(value);
			p = blk->tail->prev;
			t = blk->tail->prev->size - 1;
			return iterator(p, t, blk);
		}//push_back
 
		p->split(t);
		p->push_back(value);
		++blk->len;
		p->next->merge();
 
		t = p->size - 1;
		p->merge();
 
		return iterator(p, t, blk);
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
		Node *p = pos.itr;
		size_t t = pos.pos;
		if (blk->len == 0 || t < 0 || t >= p->size) throw(invalid_iterator());
		if (t == p->getsize() - 1) {
			p->erase(t);
			Node *q = p->next;
			p->prev->merge();
			p = q;
			t = 0;
		}
		else {
			p->erase(t);
			if (p->prev->mergeable()) {
				Node *q = p->prev;
				t = q->size + t;
				p->prev->merge();
				p = q;
			}
		}
		--blk->len;
		return iterator(p, t, blk);
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
		blk->push_back(value);
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
		if (empty()) throw(container_is_empty());
		blk->pop_back();
		if (blk->tail->prev->size == 0) {
			Node *p = blk->tail->prev;
			blk->tail->prev = p->prev;
			p->prev->next = blk->tail;
			delete p;
		}
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
		blk->push_front(value);
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
		if (empty()) throw(container_is_empty());
		blk->pop_front();
		blk->head->next->merge();
	}
};
 
}
 
#endif
