#pragma once
#include <string>
#include "Tree.h"

template<typename T>
struct Dupl
{
	T element;
	size_t count;

	Dupl() : count(1) {}
	Dupl(T element) : element(element), count(1) {}
};


struct Word;

struct User
{
	std::string id;
	std::string name;

	List< Tree<User>::Node > treePos;
	List< Tree<Dupl<User>>::Node > treeDuplPos;

	Tree<User*> follower;
	Tree<User*> following;
	Tree<Dupl<Word>> mentioned;

	User(std::string id, std::string name);

	bool mentioned_add(Word w);
	long long mentioned_count() const;
};


struct Word
{
	std::string content;

	
	Tree<Dupl<User>> mentioning;

	Word(std::string content);

	bool mentioning_add(User u);
	long long mentioning_count() const;
};
