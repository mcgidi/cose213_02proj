#pragma once
#include "List.h"
#include <cstdio>
#include <memory>

template<typename T>
class Tree
{
	template<typename U> using sptr = std::shared_ptr<U>;
	template<typename U> using wptr = std::weak_ptr<U>;
public:
	struct Node
	{
		friend class Tree;

		Tree* tree;
		T value;
		Node *parent, *left, *right;
		enum class Color { Black, Red } color;

		Node(Tree* tree, T value) :
			tree(tree), value(value), color(Color::Red),
			parent(nullptr), left(nullptr), right(nullptr) {}

		bool erase() {
			return tree->erase(this);
		}

		bool operator==(Node const& n) const {
			return compare(value, n.value) == 0;
		}

		bool operator!=(Node const& n) const {
			return compare(value, n.value) != 0;
		}
	};

private:
	Node *root;
	size_t count;
	std::function<int(T, T)> const compare;

public:
	Tree(std::function<int(T, T)> compare_func) :
		root(nullptr), count(0), compare(compare_func) {
	}

	void clear() {
		List<Node*> all = get_top(0);
		for (Node* i : all)
			delete i;
		count = 0;
	}

	~Tree() {
		clear();
	}

private:
	Node* grandparent(Node* node) const {
		return node->parent ? node->parent->parent : nullptr;
	}

	Node* uncle(Node* node) const {
		Node* grand = grandparent(node);
		if (grand == nullptr)
			return nullptr;
		else
			return node->parent == grand->left ? grand->right : grand->left;
	}

	Node* sibling(Node* node) const {
		return node->parent == nullptr ? nullptr
			: (node == node->parent->left ? node->parent->right : node->parent->left);
	}

	void rotate_left(Node* node) {
		//   t            c
		// 1   c   ->   t   3
		//    2 3      1 2
		Node *t = node;
		Node *c = node->right;
		Node *p = node->parent;

		if (c->left)
			c->left->parent = t;

		t->right = c->left;
		t->parent = c;
		c->left = t;
		c->parent = p;

		if (p == nullptr)
			root = c;
		else if (p->left == t)
			p->left = c;
		else
			p->right = c;
	}

	void rotate_right(Node* node) {
		//    t          c
		//  c   3  ->  1   t
		// 1 2            2 3
		Node *t = node;
		Node *c = node->left;
		Node *p = node->parent;

		if (c->right)
			c->right->parent = t;

		t->left = c->right;
		t->parent = c;
		c->right = t;
		c->parent = p;

		if (p == nullptr)
			root = c;
		else if (p->right == t)
			p->right = c;
		else
			p->left = c;
	}

	bool isRed(Node* node) const {
		return node && node->color == Node::Color::Red;
	}

	bool isBlack(Node* node) const {
		return node == nullptr || node->color == Node::Color::Black;
	}

	void setRed(Node* node) {
		if (node)
			node->color = Node::Color::Red;
	}

	void setBlack(Node* node) {
		if (node)
			node->color = Node::Color::Black;
	}

	void swapNode(Node* a, Node *b) {
		Node *a_p = a->parent;
		Node *a_r = a->right;
		Node *a_l = a->left;
		Node::Color a_c = a->color;
		Node *b_p = b->parent;
		Node *b_r = b->right;
		Node *b_l = b->left;
		Node::Color b_c = b->color;

		if (a_p != b) {
			if (a_p == nullptr)
				root = b;
			else if (a_p->left == a)
				a_p->left = b;
			else
				a_p->right = b;
		}

		if (a_r && a_r != b)
			a_r->parent = b;
		if (a_l && a_l != b)
			a_l->parent = b;

		if (b_p != a) {
			if (b_p == nullptr)
				root = a;
			else if (b_p->left == b)
				b_p->left = a;
			else
				b_p->right = a;
		}

		if (b_r && b_r != a)
			b_r->parent = a;
		if (b_l && b_l != a)
			b_l->parent = a;

		a->parent = b_p == a ? b : b_p;
		a->right = b_r == a ? b : b_r;
		a->left = b_l == a ? b : b_l;
		a->color = b_c;
		b->parent = a_p == b ? a : a_p;
		b->right = a_r == b ? a : a_r;
		b->left = a_l == b ? a : a_l;
		b->color = a_c;
	}

	void transplant(Node* dest, Node *node) {
		if (dest->parent == nullptr)
			root = node;
		else if (dest->parent->left == dest)
			dest->parent->left = node;
		else
			dest->parent->right = node;
		if (node)
			node->parent = dest->parent;
	}

public:
	Node* insert(T value) {
		Node *const elem = new Node(this, value);

		// binary tree insert
		if (root == nullptr) {
			// case 1. root is inserted -> done. pass
			root = elem;
			setBlack(root);
			++count;
			return root;
		}
		else {
			Node *current = root, *post = root;
			while (current) {
				post = current;
				int res = compare(current->value, value);
				if (res == 0)      // duplicated 
					return nullptr;
				else if (res > 0)  // value > current
					current = current->left;
				else                 // value < current
					current = current->right;
			}
			elem->parent = post;
			if (compare(post->value, value) > 0)  // value > post
				post->left = elem;
			else                                    // value < post
				post->right = elem;
		}

		// RB fix
		// case 2. parent is Black -> done. pass
		Node *current = elem;
		while (isRed(current->parent)) {
			Node *parent = current->parent;
			Node *grand = grandparent(current);
			Node *uncle = this->uncle(current);
			// case 3. parent and uncle are both Red
			if (isRed(uncle)) {
				setBlack(parent);
				setBlack(uncle);
				setRed(grand);
				current = grand;
			}
			// parent is Red, uncle is Black
			else {
				// case 4. current and parent are diffrent direction -> rotate parent
				if (current == parent->right && parent == grand->left) {
					current = parent;
					rotate_left(current);
				}
				else if (current == parent->left && parent == grand->right) {
					current = parent;
					rotate_right(current);
				}
				parent = current->parent;
				grand = grandparent(current);
				uncle = this->uncle(current);
				// case 5. current and parent are same direction -> rotate grand
				setBlack(parent);
				setRed(grand);
				if (current == parent->left)
					rotate_right(grand);
				else
					rotate_left(grand);
			}
			setBlack(root);
		}

		++count;
		return elem;
	}

private:
	void erase_fixup(Node* node, Node* parent) {
		// case 1. node is new root -> pass
		while (node != root && isBlack(node)) {
			if (node == parent->left) {
				Node *sible = parent->right;
				// case 2. sibling is red 
				if (isRed(sible)) {
					setBlack(sible);
					setRed(parent);
					rotate_left(parent);
					sible = parent->right;
				}
				// case 3. sibling, child of sibling is Black 
				if (isBlack(sible->left) && isBlack(sible->right)) {
					setRed(sible);
					node = parent;
					parent = node->parent;
				}
				else {
					// case 4. sibling is Black, node-direction child of sibling is Red, other child is Black
					if (isBlack(sible->right)) {
						setRed(sible);
						setBlack(sible->left);
						rotate_right(sible);
						sible = parent->right;
					}
					// case 6. sibling is Black, node-opposite-direction child of sibling is Red
					if (sible)
						sible->color = parent->color;
					setBlack(parent);
					setBlack(sible->right);
					rotate_left(parent);
					node = root;
				}
			}
			else {
				Node *sible = parent->left;
				// case 2. sibling is red 
				if (isRed(sible)) {
					setBlack(sible);
					setRed(parent);
					rotate_right(parent);
					sible = parent->left;
				}
				// case 3. sibling, child of sibling is Black 
				if (isBlack(sible->right) && isBlack(sible->left)) {
					setRed(sible);
					node = parent;
					parent = node->parent;
				}
				else {
					// case 4. sibling is Black, node-direction child of sibling is Red, other child is Black
					if (isBlack(sible->left)) {
						setRed(sible);
						setBlack(sible->right);
						rotate_left(sible);
						sible = parent->left;
					}
					// case 6. sibling is Black, node-opposite-direction child of sibling is Red
					if (sible)
						sible->color = parent->color;
					setBlack(parent);
					setBlack(sible->left);
					rotate_right(parent);
					node = root;
				}
			}
		}
		setBlack(node);
	}

	bool erase(Node* node) {
		if (node == nullptr)
			return false;

		if (node->left && node->right) {
			Node *repl = node->right;
			while (repl->left)
				repl = repl->left;
			swapNode(node, repl);
		}

		// node has at least one null child
		Node *child = node->left ? node->left : node->right;
		Node *childParent = node->parent;

		// replace node to child
		transplant(node, child);

		// RB fix
		if (isBlack(node)) {
			if (isRed(child))
				child->color = Node::Color::Black;
			else
				erase_fixup(child, childParent);
		}

		delete node;
		--count;
		return true;
	}
public:
	bool erase(T value) {
		return erase(find(value));
	}

	Node* find(T value) const {
		Node* current = root;
		while (current) {
			int res = compare(current->value, value);
			if (res == 0)
				return current;
			else if (res > 0)  // value > current
				current = current->left;
			else                 // value < current
				current = current->right;
		}
		return nullptr;
	}

	List<Node*> get_top(int n) const {
		if (root == nullptr)
			return List<Node*>();

		if (n <= 0)
			n = 0x7fffffff;

		struct Check
		{
			Node* value;
			int check;
			Check(Node* value) : value(value), check(0) {}
		};

		List<Node*> result;
		List<Check> stack;

		stack.push_front(Check(root));

		while (n > 0 && stack.size() > 0) {
			Check& t = stack.front()->value;
			if (t.check == 0) {
				++(t.check);
				if (t.value->left) {
					stack.push_front(Check(t.value->left));
					continue;
				}
			}
			if (t.check == 1) {
				result.push_back(t.value);
				--n;
				++(t.check);
				if (t.value->right) {
					stack.push_front(Check(t.value->right));
					continue;
				}
			}
			else {
				stack.pop_front();
			}
		}

		return result;
	}

	List<Node*> get_bottom(int n) const {
		if (root == nullptr)
			return List<Node*>();

		if (n <= 0)
			n = 0x7fffffff;

		struct Check
		{
			Node* value;
			int check;
			Check(Node* value) : value(value), check(0) {}
		};

		List<Node*> result;
		List<Check> stack;

		stack.push_front(Check(root));

		while (n > 0 && stack.size() > 0) {
			Check& t = stack.front()->value;
			if (t.check == 0) {
				++(t.check);
				if (t.value->right) {
					stack.push_front(Check(t.value->right));
					continue;
				}
			}
			if (t.check == 1) {
				result.push_back(t.value);
				--n;
				++(t.check);
				if (t.value->left) {
					stack.push_front(Check(t.value->left));
					continue;
				}
			}
			else {
				stack.pop_front();
			}
		}

		return result;
	}

	void debug_print(std::function<void(T)> print_func) const {
		if (root == nullptr) {
			printf("null\n");
			return;
		}

		struct Check
		{
			Node* value;
			int check;
			bool right;
			Check(Node* value, bool right) : value(value), check(0), right(right) {}
		};

		List<Check> stack;

		stack.push_front(Check(root, false));

		while (stack.size() > 0) {
			Check& t = stack.front()->value;
			if (t.check == 0) {
				++(t.check);
				if (t.value->right) {
					stack.push_front(Check(t.value->right, true));
					continue;
				}
			}
			if (t.check == 1) {
				for (int i = 1; i < stack.size(); ++i)
					printf("    ");
				if (stack.size() > 1)
					printf("%s", t.right ? "¦£" : "¦¦");
				printf("%c:", isRed(t.value) ? 'R' : 'B');
				print_func(t.value->value);
				++(t.check);
				if (t.value->left) {
					stack.push_front(Check(t.value->left, false));
					continue;
				}
			}
			else {
				stack.pop_front();
			}
		}
	}

	size_t size() const {
		return count;
	}

	Tree(Tree& o) = delete;
	Tree& operator=(Tree&) = delete;
	Tree(Tree&& o) : Tree(o.compare) {
		std::swap(root, o.root);
		std::swap(count, o.count);
	}
};
