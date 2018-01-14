#include "stdafx.h"
#include "Controller.h"



Controller::Controller(Text &words_to_check_text) :  words_to_check_text(words_to_check_text) {
	words = words_to_check_text.get_list_of_words_from_text();

}

std::vector<std::pair<std::string, int>> Controller::repeats(std::list<std::string> text) {
	std::vector<std::pair<std::string, int>> count_vector;

	int count;
	for (auto w = words.begin(); w != words.end(); ++w) {
		count = 0;
		for (auto s = text.begin(); s != text.end(); ++s)
			if (*w == *s)
				count++;

		count_vector.push_back(std::make_pair(*w, count));
	}
	return count_vector;
}

std::map<std::string, int> Controller::minimum(std::map<std::string, int> a, std::map<std::string, int> b) {

	std::map<std::string, int> min_map;
	for (auto it = a.begin(); it != a.end(); ++it) {
		if (it->second< b.at(it->first))
			min_map[it->first] = it->second;
		else {
			min_map[it->first] = b.at(it->first);
		}
	}
	return min_map;
}

int Controller::sum_repeats(std::map<std::string, int> min_map) {
	int min = 0;

	for (const auto& it : min_map)
		min += it.second;
	return min;
}
/*
void Controller::check() {
	int sum = sum_repeats(
		minimum(
			repeats(base_text.get_list_of_words_from_text()),
			repeats(examined_text.get_list_of_words_from_text())
		)
	);
	std::cout << "Ilosc slow w textBase: " << base_text.get_list_of_words_from_text().size() << std::endl;
	std::cout << "Ilosc slow w textExamined: " << examined_text.get_list_of_words_from_text().size() << std::endl;
	std::cout << "Poziom podobienstwa: " << (double)sum / base_text.get_list_of_words_from_text().size() * 100 << "%" << std::endl;


}*/
