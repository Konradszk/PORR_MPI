#pragma once
#include <iostream>
#include <list>
#include <map>
#include "Text.h"

class Controller {
public:
	Controller();

private:
public:
	Controller( Text &words_to_check_text);
	//void check();
	std::map<std::string, int> minimum(std::map<std::string, int> a, std::map<std::string, int> b);
	std::vector<std::pair<std::string, int>>  repeats(std::list<std::string> text);
	int getSize(void) const { return(words.size()); };
	int sum_repeats(std::map<std::string, int> min_map);

private:
	Text words_to_check_text;
	std::list<std::string> words;
	
};

