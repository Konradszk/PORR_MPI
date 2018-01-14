#include "stdafx.h"

#include "Text.h"



Text::Text(std::string file_path) : file_path(file_path) {
	Text::get_words_from_file();
}

std::list<char> Text::list_of_characters_to_change = { '.', ',', ';', ':', ')', '(', '-', '[', ']', '?', '!' };

std::list<std::string>  Text::get_list_of_words_from_text() {
	return list_of_words_from_text;
}

void Text::get_words_from_file() {
	std::string str;

	std::ifstream fin(file_path); // Open it up!
	while (fin >> str) // Will read up to eof() and stop at every
	{                  // whitespace it hits. (like spaces!)
		std::string temp_str = str;
		std::string result_str = Text::remove_all_chars(temp_str);
		list_of_words_from_text.push_back(result_str);
	}
	fin.close(); // Close that file!
}

void Text::read_text() {
	std::cout << "start reading" << std::endl;
	for (std::list<std::string>::iterator it = list_of_words_from_text.begin(); it != list_of_words_from_text.end(); it++)
		std::cout << *it << std::endl;

}

std::string Text::remove_char(std::string str, char c)
{
	std::string result;

	for (std::string::iterator it = str.begin(); it != str.end(); it++)
	{
		char current_char = *it;
		if (current_char != c)
			result += current_char;
	}

	return result;
}

std::string Text::remove_all_chars(std::string str) {
	std::string str2 = str;
	for (std::list<char>::iterator it = Text::list_of_characters_to_change.begin(); it != Text::list_of_characters_to_change.end(); it++)
	{
		str2 = Text::remove_char(str2, *it);
	}
	return str2;
}