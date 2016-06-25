#include "Data.h"

User::User(std::string id, std::string name) :
	id(id), name(name),
	follower([](sptr<User> a, sptr<User> b) { return a->id .compare(b->id); }),
	following([](sptr<User> a, sptr<User> b) { return a->id .compare(b->id); }),
	mentioned([](sptr<Word> a, sptr<Word> b) { return a.element->content.compare(b.element->content); }) {
}

bool User::mentioned_add(sptr<Word> w) {
	auto pos = mentioned.find(sptr<Word>(w));
	if (pos) {
		++(pos->value.count);
		return false;
	}
	else {
		pos = mentioned.insert(sptr<Word>(w));
		w->treeDuplPos.push_back(pos);
		return true;
	}
}

long long User::mentioned_count() const {
	long long res = 0;
	for (auto i : mentioned.get_top(0))
		res += i->value.count;
	return res;
}

Word::Word(std::string content) : content(content),
mentioning([](sptr<User> a, sptr<User> b) { return a.element->id.compare(b.element->id); }) {
}

bool Word::mentioning_add(User  u) {
	auto pos = mentioning.find(sptr<User>(u));
	if (pos) {
		++(pos->value.count);
		return false;
	}
	else {
		pos = mentioning.insert(sptr<User>(u));
		u->treeDuplPos.push_back(pos);
		return true;
	}
}

long long Word::mentioning_count() const {
	long long res = 0;
	for (auto i : mentioning.get_top(0))
		res += i->value.count;
	return res;
}
