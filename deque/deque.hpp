/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP
 
 // only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"
 
namespace sjtu {
 
	template<
		class Key,
		class T,
		class Compare = std::less<Key>
	> class map {
	public:
		/**
		 * the internal type of data.
		 * it should have a default constructor, a copy constructor.
		 * You can use sjtu::map as value_type by typedef.
		 */
		typedef pair<const Key, T> value_type;
 
		enum colourT { RED, BLACK };
		struct Node {
			value_type *data;
			colourT colour;
			Node *father, *left, *right;	// keep a tree
 
			Node *prev, *next;				// keep a List for iterator
 
			Node() { data = NULL; colour = RED; father = left = right = NULL; prev = next = NULL; }
			Node(const value_type &_data, Node *_father, Node *_left, Node *_right, colourT _colour = RED) {
				data = new value_type(_data);
				father = _father;
				left = _left;
				right = _right;
				colour = _colour;
			}
			~Node() {
				if (data) delete data;
			}
		};
 
		Node *root, *nil;
		Compare comparator;
		size_t curSize;
 
		class const_iterator;
		class iterator {
		private:
			friend const_iterator;
			friend map;
		private:
			Node *itr, *root, *nil;
 
		public:
			iterator() {
				root = nil = itr = NULL;
			}
			iterator(const iterator &other) {
				itr = other.itr;
				root = other.root;
				nil = other.nil;
			}
			iterator(Node *t, Node *_root, Node *_nil) {
				itr = t;
				root = _root;
				nil = _nil;
			}
			iterator operator=(const iterator &other) {
				itr = other.itr;
				root = other.root;
				nil = other.nil;
				return *this;
			}
			iterator operator++(int) {
				iterator temp(*this);
				++(*this);
				return temp;
			}
			iterator & operator++() {
				if (itr == nil) throw(index_out_of_bound());
				itr = itr->next;
				return *this;
			}
			iterator operator--(int) {
				iterator temp(*this);
				--(*this);
				return temp;
			}
			iterator & operator--() {
				if (itr == nil) {
					itr = root;
					if (itr == NULL) throw(index_out_of_bound());
					while (itr->right != nil) itr = itr->right;
				}
				else itr = itr->prev;
				if (itr == nil) throw(index_out_of_bound());				
 
				return *this;
			}
			value_type & operator*() const {
				return *(itr->data);
			}
			bool operator==(const iterator &rhs) const { return itr == rhs.itr; }
			bool operator==(const const_iterator &rhs) const { return *this == rhs.itr; }
			bool operator!=(const iterator &rhs) const {
				return !(*this == rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this == rhs);
			}
 
			value_type* operator->() const noexcept {
				return itr->data;
			}
		};
		class const_iterator {
			friend iterator;
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
			
			const_iterator operator++(int) {
				const_iterator temp(*this);
				++(*this);
				return temp;
			}
			const_iterator & operator++() {
				itr++;
				return *this;
			}
			const_iterator operator--(int) {
				const_iterator temp(*this);
				--(*this);
				return temp;
			}
			const_iterator & operator--() {
				--itr;
				return *this;
			}
			value_type & operator*() const {
				return *(itr.itr->data);
			}
			bool operator==(const iterator &rhs) const { return itr == rhs; }
			bool operator==(const const_iterator &rhs) const { return itr == rhs.itr; }
			bool operator!=(const iterator &rhs) const {
				return !(itr == rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this == rhs);
			}
 
			value_type* operator->() const noexcept {
				return itr.itr->data;
			}
		};
		/**
		 * TODO two constructors
		 */
		map() {
			root = NULL;
			nil = new Node();
			nil->colour = BLACK;
			nil->data = NULL;
			curSize = 0;
		}
		map(const map &other) {
			root = NULL;
			nil = new Node();
			nil->colour = BLACK;
			nil->data = NULL;
			curSize = 0;
 
			auto it = other.cbegin();
			while (it != other.cend()) {
				value_type temp = *it;
				Node *t = NULL;
				insert(temp, t);
				++it;
			}
		}
		/**
		 * TODO assignment operator
		*/
		map & operator=(const map &other) {
			if (this == &other) return *this;
			if (root) clear(root);
			root = NULL;
			curSize = 0;
 
			auto it = other.cbegin();
			while (it != other.cend()) {
				value_type temp = *it;
				Node *t = NULL;
				insert(temp, t);
				++it;
			}
 
			return *this;
		}
		/**
		* TODO Destructors
		*/
		~map() {
			clear();
			delete nil;
		}
		T & at(const Key &key) {
			Node *t = findNode(key);
			if (t == nil) throw(index_out_of_bound());
			return t->data->second;
		}
		const T & at(const Key &key) const {
			Node *t = findNode(key);
			if (t == nil) throw(index_out_of_bound());
			return t->data->second;
		}
		T & operator[](const Key &key) {
			Node *t = findNode(key);
			if (t == nil) {
				value_type p = pair<Key, T> (key, T());
				insert(p, t);
				return t->data->second;
			}
			else return at(key);
		}
		const T & operator[](const Key &key) const {
			return at(key);
		}
		iterator begin() {
			if (empty()) return end();
			Node *it = root;
			while (it->left != nil)
				it = it->left;
			return iterator(it, root, nil);
		}
		const_iterator cbegin() const {
			if (empty()) return cend();
			Node *it = root;
			while (it->left != nil)
				it = it->left;
			iterator temp(it, root, nil);
			return const_iterator(temp);
		}
		iterator end() {
			return iterator(nil, root, nil);
		}
		const_iterator cend() const {
			iterator temp(nil, root, nil);
			return const_iterator(temp);
		}
		bool empty() const {
			return curSize == 0;
		}
		size_t size() const {
			return curSize;
		}
		void clear(Node *&t) {
			if (t != nil) {
				clear(t->left);
				clear(t->right);
				delete t;
				t = NULL;
				return;
			}
		}
 
		void clear() {
			if (root) clear(root);
			curSize = 0;
		}
		/**
		 * insert an element.
		 * return a pair, the first of the pair is
		 *   the iterator to the new element (or the element that prevented the insertion),
		 *   the second one is true if insert successfully, or false.
		 */
		pair<iterator, bool> insert(const value_type &value) {
			Node *t = NULL, *now = this->findNode(value.first);
			if (now != nil) {
				iterator temp(now, root, nil);
				return pair<iterator, bool>(temp, false);
			}
			else {
				insert(value, t);
				iterator temp(t, root, nil);
				return pair<iterator, bool>(temp, true);
			}
		}
 
		Node *successor(Node *now) {
			if (now->right != nil) {
				Node *t = now->right;
				while (t->left != nil) t = t->left;
				return t;
			}
			Node *t = now->father;
			while (t != nil && now == t->right) {
				now = t;
				t = t->father;
			}
			return t;
		}
 
		Node *former(Node *now) {
			if (now->left != nil) {
				Node *t = now->left;
				while (t->right != nil) t = t->right;
				return t;
			}
			Node *t = now->father;
			while (t != nil && now == t->left) {
				now = t;
				t = t->father;
			}
			return t;
		}
 
		void left_rotate(Node *now) {
			Node *rchild = now->right;
			now->right = rchild->left;
			if (rchild->left != nil)
				rchild->left->father = now;
 
			rchild->father = now->father;
			if (now->father == nil) root = rchild;
			else
				if (now == now->father->left)
					now->father->left = rchild;
				else
					now->father->right = rchild;
 
			rchild->left = now;
			now->father = rchild;
		}
 
		void right_rotate(Node *now) {
			Node *lchild = now->left;
			now->left = lchild->right;
			if (lchild->right != nil)
				lchild->right->father = now;
 
			lchild->father = now->father;
			if (now->father == nil)	root = lchild;
			else
				if (now == now->father->left)
					now->father->left = lchild;
				else
					now->father->right = lchild;
 
			lchild->right = now;
			now->father = lchild;
		}
 
		void insert_fixup(Node *now) {
			while (now->father->colour == RED) { // while there is two consequent RED nodes
				Node *uncle = NULL, *gfather = now->father->father;
				// LL, LR
				if (now->father == gfather->left) {
					uncle = gfather->right;
					if (uncle->colour == RED) {
						now->father->colour = BLACK;
						uncle->colour = BLACK;
						gfather->colour = RED;
						now = gfather;
					}
					else {
						if (now == now->father->right) { //LL, LR
							now = now->father;
							left_rotate(now);
						}
						now->father->colour = BLACK;
						gfather->colour = RED;
						right_rotate(gfather);
					}
				}
				else {
					uncle = gfather->left;
					if (uncle->colour == RED) {
						now->father->colour = BLACK;
						uncle->colour = BLACK;
						gfather->colour = RED;
						now = gfather;
					}
					else {
						if (now == now->father->left) { //LR
							now = now->father;
							right_rotate(now);
						}
						now->father->colour = BLACK;
						gfather->colour = RED;
						left_rotate(gfather);
					}
				}
			}
			root->colour = BLACK;
		}
 
		void insert(const value_type &value, Node *&t) {
			if (curSize == 0) {
				++curSize;
				root = new Node(value, nil, nil, nil);
				root->colour = BLACK;
				root->next = successor(root);
				root->prev = former(root);
				t = root;
				return;
			}
 
			++curSize;
			Node *y = nil, *x = root;
			while (x != nil) {
				y = x;
				if (comparator(value.first, x->data->first)) x = x->left;
				else x = x->right;
			}
			Node *z = new Node(value, y, nil, nil);
 
			if (y == nil) root = z;
			else if (comparator(value.first, y->data->first)) y->left = z;
			else y->right = z;
 
			Node *prev = former(z), *next = successor(z);
			z->next = next;
			z->prev = prev;
			next->prev = z;
			prev->next = z;
 
			insert_fixup(z);
			t = z;
		}
 
		void delete_fixup(Node *now) {
			while (now != root && now->colour == BLACK) {
				if (now == now->father->left) {
					Node *sibling = now->father->right;
					if (sibling->colour == RED) {
						sibling->colour = BLACK;
						now->father->colour = RED;
						left_rotate(now->father);
						sibling = now->father->right;
					}
					if (sibling->left->colour == BLACK && sibling->right->colour == BLACK) {
						sibling->colour = RED;
						now = now->father;
					}
					else {
						if (sibling->right->colour == BLACK) {
							sibling->left->colour = BLACK;
							sibling->colour = RED;
							right_rotate(sibling);
							sibling = now->father->right;
						}
						sibling->colour = now->father->colour;
						now->father->colour = BLACK;
						sibling->right->colour = BLACK;
						left_rotate(now->father);
						now = root;
					}
				}
				else {
					Node *sibling = now->father->left;
					if (sibling->colour == RED) {
						sibling->colour = BLACK;
						now->father->colour = RED;
						right_rotate(now->father);
						sibling = now->father->left;
					}
					if (sibling->left->colour == BLACK && sibling->right->colour == BLACK) {
						sibling->colour = RED;
						now = now->father;
					}
					else {
						if (sibling->left->colour == BLACK) {
							sibling->right->colour = BLACK;
							sibling->colour = RED;
							left_rotate(sibling);
							sibling = now->father->left;
						}
						sibling->colour = now->father->colour;
						now->father->colour = BLACK;
						sibling->left->colour = BLACK;
						right_rotate(now->father);
						now = root;
					}
				}
			}
			now->colour = BLACK;
		}
 
		void erasenode(Node *now) {
			//find the Real deleted Node
			Node *temp = NULL, *son_temp = NULL;
			if (now->left == nil || now->right == nil) {
				temp = now;
				temp->next->prev = temp->prev;
				temp->prev->next = temp->next;
			}
			else {
				temp = successor(now);
 
				if (now == root) root = temp;
				
				if (now->father->left == now) now->father->left = temp;
				else now->father->right = temp;
				if (temp->father->left == temp) temp->father->left = now;
				else temp->father->right = now;
 
				Node *t = temp->father;
				temp->father = now->father;
				now->father = t;
 
				t = now->left;
				t->father = temp;
				now->left = temp->left;
				temp->left = t;
 
				t = now->right;
				t->father = temp;
				now->right = temp->right;
				temp->right = t;
 
				temp->prev = now->prev;
				now->prev->next = temp;
 
				colourT tempc = now->colour;
				now->colour = temp->colour;
				temp->colour = tempc;
 
				temp = now;
			}
			
			if (temp->left != nil) son_temp = temp->left;
			else son_temp = temp->right;
 
			son_temp->father = temp->father;
 
			if (temp->father == nil) root = son_temp;
			else
				if (temp == temp->father->left) temp->father->left = son_temp;
				else temp->father->right = son_temp;
 
			delete temp->data;
			temp->data = NULL;
 
			if (temp->colour == BLACK) delete_fixup(son_temp);
			nil->colour = BLACK;
			nil->left = nil->right = nil->prev = nil->next = nil->father = nil;
 
			delete temp;
 
		}
 
 
		void erasenode(const Key &key) {
			erasenode(findNode(key));
		}
		/**
		 * erase the element at pos.
		 *
		 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
		 */
		void erase(iterator pos) {
			if (pos == end() || pos.root != root || pos.nil != nil) throw(index_out_of_bound());
			Node *t = pos.itr;
			erasenode(t);
			--curSize;
		}
		/**
		 * Returns the number of elements with key
		 *   that compares equivalent to the specified argument,
		 *   which is either 1 or 0
		 *     since this container does not allow duplicates.
		 * The default method of check the equivalence is !(a < b || b > a)
		 */
		size_t count(const Key &key) const {
			size_t i;
			if (findNode(key) == nil) i = 0;
			else i = 1;
			return i;
		}
		iterator find(const Key &key) {
			Node *temp = findNode(key);
			return iterator(temp, root, nil);
		}
		const_iterator find(const Key &key) const {
			iterator temp(findNode(key), root, nil);
			return const_iterator(temp);
		}
 
		int compare(const Key &a, const Key &b) const {
			return comparator(a, b) ? -1 : (comparator(b, a) ? 1 : 0);
		}
		Node *findNode(const Key &key) const {
			if (empty()) return nil;
 
			Node *t = root;
 
			while (t != nil) {
				if (compare(key, t->data->first) == 0) break;
 
				if (compare(key, t->data->first) < 0) t = t->left;
				else t = t->right;
			}
 
			return t;
		}
	};
 
}
 
#endif