#pragma once
#include <utility>
#include <functional>
#include <memory>

template<typename T>
class List
{
	template<typename U> using sptr = std::shared_ptr<U>;
public:
	struct Node
	{
		T value;
		Node *next, *pre;

		Node(T value) :
			value(value), next(nullptr), pre(nullptr) {}
	};

	class iterator
	{
		friend class List;
		Node* pNode;

	public:
		T operator*() {
			return pNode->value;
		}

		iterator& operator++() {
			pNode = pNode->next;
			return *this;
		}

		iterator const operator++(int) {
			iterator temp = *this;
			pNode = pNode->next;
			return temp;
		}

		bool operator==(iterator const& it) const {
			return pNode == it.pNode;
		}

		bool operator!=(iterator const& it) const {
			return pNode != it.pNode;
		}
	};

private:
	Node *first, *last;
	size_t count;

public:
	List() : first(nullptr), last(nullptr), count(0) {
	}

	~List() {
		clear();
	}

	void push_front(T value) {
		if (count == 0)
			first = last = new Node(value);
		else {
			Node* f = first;
			first = new Node(value);
			first->next = f;
			f->pre = first;
			if (count == 1)
				last->pre = first;
		}
		++count;
	}

	bool pop_front() {
		if (count == 0)
			return false;
		else if (count == 1) {
			delete first;
			first = last = nullptr;
		}
		else if (count == 2) {
			delete first;
			first = last;
			last->pre = nullptr;
		}
		else {
			Node* f = first->next;
			delete first;
			first = f;
			first->pre = nullptr;
		}
		--count;
		return true;
	}

	void push_back(T value) {
		if (count == 0)
			first = last = new Node(value);
		else {
			Node* l = new Node(value);
			last->next = l;
			l->pre = last;
			last = l;
			if (count == 1)
				first->next = last;
		}
		++count;
	}

	bool pop_back() {
		if (count == 0)
			return false;
		else if (count == 1) {
			delete last;
			first = last = nullptr;
		}
		else if (count == 2) {
			delete last;
			last = first;
			last->next = nullptr;
		}
		else {
			Node* l = last->pre;
			delete last;
			last = l;
			last->next = nullptr;
		}
		--count;
		return true;
	}

	bool append(List<T>& o) {
		for (T const& i : o)
			push_back(i);
	}

	Node* front() const {
		return first ? first : nullptr;
	}

	Node* find(std::function<bool(T)> find_expression) const {
		for (Node *c = first, *n; c; c = n) {
			n = c->next;
			if (find_expression(c->value))
				return c;
		}
		return nullptr;
	}

	void clear() {
		for (Node *c = first, *n; c; c = n) {
			n = c->next;
			delete c;
		}
		count = 0;
		first = nullptr;
	}

	iterator begin() const {
		iterator it;
		it.pNode = count > 0 ? first : nullptr;
		return it;
	}

	iterator end() const {
		iterator it;
		it.pNode = nullptr;
		return it;
	}

	size_t size() const {
		return count;
	}

	List(List& o) = delete;
	List& operator=(List&) = delete;
	List(List&& o) : List() {
		std::swap(first, o.first);
		std::swap(last, o.last);
		std::swap(count, o.count);
	}
}; 
