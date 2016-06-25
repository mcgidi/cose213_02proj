#pragma warning(disable:4996)
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "Tree.h"
#include "Data.h"


// input method
char buffer[1001];

std::string input(std::string message) {
	printf("%s", message.c_str());
	scanf("%1000s", buffer);
	return std::string(buffer);
};

char* finput(FILE* f) {
	char* r = fgets(buffer, sizeof(buffer), f);
	int i;
	for (i = 0; i < sizeof(buffer) && buffer[i]; ++i);
	for (--i; i >= 0 && (buffer[i] == '\n' || buffer[i] == '\r' || buffer[i] == ' '); --i)
		buffer[i] = '\0';
	return r;
}


// data structure to solve the problem
List<sptr<User>> resultUserHolder;

Tree<sptr<User>> userIdTree([](sptr<User> *a, sptr<User>& b) { return b->id.compare(a->id); });
Tree<sptr<User>> userNameTree([](sptr<User> *a, sptr<User>& b) { return a->name.compare(b->name); });
Tree<sptr<Word>> wordContentTree([](sptr<Word> *a, sptr<User>& b) { return a->content.compare(b->content); });

// option 0.
void load_data() {
	// clear data
	userIdTree.clear();
	userNameTree.clear();
	wordContentTree.clear();
	long long total_friends = 0, total_tweets = 0;

	// read user file
	FILE *file = fopen("user.txt", "r");
	while (finput(file)) {
		std::string id(buffer);

		finput(file);  // date, skip

		finput(file);
		std::string name(buffer);

		finput(file);  // blank, skip

		User *u = new User(id, name);
		auto t = userIdTree.insert(u);
		u->treePos.push_back(t);
		t = userNameTree.insert(u);
		u->treePos.push_back(t);
	}
	fclose(file);

	// read word file
	file = fopen("word.txt", "r");
	while (finput(file)) {
		std::string id(buffer);

		finput(file);  // date, skip

		finput(file);
		std::string content(buffer);

		finput(file);  // blank, skip

		Word *w = new Word(content);

		User *f = new User(id, "");
		auto nu = userIdTree.find(f);
		User *u = nu->value;
		delete f;

		if (u == nullptr) {
			continue;
		}

		++total_tweets;

		auto wpos = wordContentTree.find(w);
		if (wpos == nullptr) {
			wpos = wordContentTree.insert(w);
			w->treePos.push_back(wpos);
		}

		wpos->value->mentioning_add(u);
		nu->value->mentioned_add(w);
	}
	fclose(file);

	// read friend file
	file = fopen("friend.txt", "r");
	while (finput(file)) {
		std::string fromid(buffer);

		finput(file);
		std::string toid(buffer);

		finput(file);  // blank, skip

		User* from = new User(fromid, "");
		User* to = new User(toid, "");

		auto fwing = userIdTree.find(from);
		auto fwer = userIdTree.find(to);

		delete from;
		delete to;

		if (fwing && fwer) {
			auto t = fwing->value->following.insert(fwer->value);
			if (t)
				fwer->value->treePos.push_back(t);
			t = fwer->value->follower.insert(fwing->value);
			if (t)
				fwing->value->treePos.push_back(t);
			++total_friends;
		}
	}
	fclose(file);

	// print some info
	printf(" Total users: %d\n", userIdTree.size());
	printf(" Total friendship records: %lld\n", total_friends);
	printf(" Total tweets: %lld\n", total_tweets);
}


// option 1.
void display_statistics() {
	long long all_friends = 0;
	long long min_friends = 0x7fffffff;
	long long max_friends = 0;
	long long all_tweets = 0;
	long long min_tweets = 0x7fffffff;
	long long max_tweets = 0;

	for (auto i : userIdTree.get_top(0)) {
		long long friends = i->value->follower.size();
		long long tweets = i->value->mentioned_count();
		all_friends += friends;
		if (min_friends > friends)
			min_friends = friends;
		if (max_friends < friends)
			max_friends = friends;
		all_tweets += tweets;
		if (min_tweets > tweets)
			min_tweets = tweets;
		if (max_tweets < tweets)
			max_tweets = tweets;
	}
	printf(" Total   number of friends: %lld\n", all_friends);
	printf(" Average number of friends: %lf\n", double(all_friends) / userIdTree.size());
	printf(" Minimum number of friends: %lld\n", min_friends);
	printf(" Maximum number of friends: %lld\n", max_friends);
	printf("\n");
	printf(" Total   tweets per user: %lld\n", all_tweets);
	printf(" Average tweets per user: %lf\n", double(all_tweets) / userIdTree.size());
	printf(" Minimum tweets per user: %lld\n", min_tweets);
	printf(" Maximum tweets per user: %lld\n", max_tweets);
}


// option 2.
void display_top5_words() {
	struct Res {
		Word* word;
		long long count;
		Res() : word(nullptr), count(0) {}
	} top5[5];
	for (auto i : wordContentTree.get_top(0)) {
		auto iment = i->value->mentioning_count();
		for (int j = 0; j < 5; ++j)
			if (top5[j].word == nullptr || top5[j].count < iment) {
				for (int k = 4; k > j; --k)
					top5[k] = top5[k - 1];
				top5[j].word = i->value;
				top5[j].count = iment;
				break;
			}
	}
	if (top5[0].word) printf(" 1st: %-32s\t(%lld time tweeted)\n", top5[0].word->content.c_str(), top5[0].count);
	if (top5[1].word) printf(" 2nd: %-32s\t(%lld time tweeted)\n", top5[1].word->content.c_str(), top5[1].count);
	if (top5[2].word) printf(" 3rd: %-32s\t(%lld time tweeted)\n", top5[2].word->content.c_str(), top5[2].count);
	if (top5[3].word) printf(" 4th: %-32s\t(%lld time tweeted)\n", top5[3].word->content.c_str(), top5[3].count);
	if (top5[4].word) printf(" 5th: %-32s\t(%lld time tweeted)\n", top5[4].word->content.c_str(), top5[4].count);
}


// option 3.
void display_top5_users() {
	struct Res {
		User* user;
		long long count;
		Res() : user(nullptr), count(0) {}
	} top5[5];
	for (auto i : userIdTree.get_top(0)) {
		auto iment = i->value->mentioned_count();
		for (int j = 0; j < 5; ++j)
			if (top5[j].user == nullptr || top5[j].count < iment) {
				for (int k = 4; k > j; --k)
					top5[k] = top5[k - 1];
				top5[j].user = i->value;
				top5[j].count = iment;
				break;
			}
	}
	if (top5[0].user) printf(" 1st: %-9s: %-32s\t(%lld time tweeted)\n", top5[0].user->id.c_str(), top5[0].user->name.c_str(), top5[0].count);
	if (top5[1].user) printf(" 2nd: %-9s: %-32s\t(%lld time tweeted)\n", top5[1].user->id.c_str(), top5[1].user->name.c_str(), top5[1].count);
	if (top5[2].user) printf(" 3rd: %-9s: %-32s\t(%lld time tweeted)\n", top5[2].user->id.c_str(), top5[2].user->name.c_str(), top5[2].count);
	if (top5[3].user) printf(" 4th: %-9s: %-32s\t(%lld time tweeted)\n", top5[3].user->id.c_str(), top5[3].user->name.c_str(), top5[3].count);
	if (top5[4].user) printf(" 5th: %-9s: %-32s\t(%lld time tweeted)\n", top5[4].user->id.c_str(), top5[4].user->name.c_str(), top5[4].count);
	for (int i = 0; i < 5; ++i)
		resultUserHolder.push_back(top5[i].user);
}


// option 4.
void display_users_tweeted(std::string word) {
	Word* u = new Word(word);
	auto pos = wordContentTree.find(u);
	delete u;
	if (pos) {
		for (auto i : pos->value->mentioning.get_top(0)) {
			printf(" %-9s: %s\n", i->value.element->id.c_str(), i->value.element->name.c_str());
			resultUserHolder.push_back(i->value.element);
		}
	}
	else
		printf(" tweet \"%s\" does not exist.\n", word.c_str());
}


// option 5.
void display_friend_of_above() {
	if (resultUserHolder.size() <= 0) {
		puts("None of users above.");
		return;
	}

	struct Res
	{
		User* user;
		User* follower;
		Res(User* u, User* f) : user(u), follower(f) {}
	};

	Tree<Res> resultTree([](Res a, Res b) { return b.user->id.compare(a.user->id); });
	for (auto i : resultUserHolder)
		for (auto j : i->follower.get_top(0))
			resultTree.insert(Res(j->value, i));

	if (resultTree.size() <= 0)
		puts("No follower exists.");
	else
		for (auto i : resultTree.get_top(0))
			printf(" %-9s: %-32s\t(following %-9s: %-16s)\n",
				i->value.user->id.c_str(), i->value.user->name.c_str(),
				i->value.follower->id.c_str(), i->value.follower->name.c_str());

	resultUserHolder.clear();
}


// option 6.
void delete_word(std::string word) {
	Word *tw = new Word(word);
	auto nw = wordContentTree.find(tw);
	delete tw;

	if (nw) {
		auto w = nw->value;

		for (auto i : w->treePos)
			i->erase();
		for (auto i : w->treeDuplPos)
			i->erase();

		delete w;
	}
	else
		printf(" word \"%s\" does not exist.\n", word.c_str());
}


// option 7.
void delete_users_censored(std::string word) {
	Word *tw = new Word(word);
	auto nw = wordContentTree.find(tw);
	delete tw;

	if (nw) {
		Tree<User*> user_delete_list([](User* a, User* b) { return a->id.compare(b->id); });
		Tree<Word*> word_delete_list([](Word* a, Word* b) { return a->content.compare(b->content); });

		for (auto i : nw->value->mentioning.get_top(0))
			if (i->value.element)
				user_delete_list.insert(i->value.element);

		for (auto i : user_delete_list.get_top(0))
			if (i->value)
				for (auto j : i->value->mentioned.get_top(0))
					if (j->value.element)
						word_delete_list.insert(j->value.element);

		for (auto i : user_delete_list.get_top(0)) {
			if (i->value) {
				for (auto j : i->value->treePos)
					if (j)
						j->erase();
				for (auto j : i->value->treeDuplPos)
					if (j)
						j->erase();
				delete i->value;
			}
		}

		for (auto i : word_delete_list.get_top(0)) {
			if (i->value && i->value->mentioning_count() == 0) {
				auto pos = wordContentTree.find(i->value);
				if (pos)
					pos->erase();
				delete i->value;
			}
		}
	}
	else
		printf(" word \"%s\" does not exist.\n", word.c_str());
}


// option 8.
void display_scc_top5() {}


// option 9.
void display_shortest_path(std::string user) {}


int main(void) {
	while (true) {
		// print interface
		puts("0. Read data files");
		puts("1. display statistics");
		puts("2. Top 5 most tweeted words");
		puts("3. Top 5 most tweeted users (and hold)");
		puts("4. Find users who tweeted a word (and hold)");
		puts("5. Find all people who are friends of the above users");
		puts("6. Delete all mentions of a word");
		puts("7. Delete all users who mentioned a word");
		puts("8. Find strongly connected components");
		puts("9. Find shortest path from a given user");
		puts("99. Quit");
		puts("");
		if (resultUserHolder.size() > 0) {
			printf("Holding users: [");
			int i = 0;
			for (auto c : resultUserHolder) {
				printf("%s%s", (i++ > 0 ? ", " : ""), c->name.c_str());
			}
			puts("]\n");
		}

		// get options input
		int arg;
		while (true) {
			printf("Select Menu: ");
			if (scanf("%d", &arg) != 1) {
				puts("Option should be integer, retype");
				scanf("%1000s", buffer);
			}
			else if (0 <= arg && arg <= 9 || arg == 99)
				break;
			else {
				puts("Invalid, retype");
			}
		}
		puts("");

		// branch options
		if (arg != 5)
			resultUserHolder.clear();

		switch (arg) {
		case 0: load_data();										break;
		case 1: display_statistics();								break;
		case 2: display_top5_words();								break;
		case 3: display_top5_users();								break;
		case 4: display_users_tweeted(input("Enter word: "));		break;
		case 5: display_friend_of_above();							break;
		case 6: delete_word(input("Enter word: "));					break;
		case 7: delete_users_censored(input("Enter word: "));		break;
		case 8: display_scc_top5();									break;
		case 9: display_shortest_path(input("Enter user name: "));	break;
		case 99: return 0;
		}

		// press to continue and clear screen
		puts("");
		puts("------------------------------------------------------------");
		puts("");
		puts("Press ENTER to continue...");

		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cin.get();
#ifdef _WIN32
		system("cls");
#else
		system("clear");
#endif
	}

	return 0;
}
