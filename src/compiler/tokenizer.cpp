#include "tokenizer.hpp"
#include <regex>

namespace carma {
	namespace tokenizer {
		std::unordered_set<std::string> reserved_words;

		token_list tokenize(std::string source_) {
			std::list<token> token_stream;
			std::string token_str;
			bool in_string = false;
			for (std::string::size_type stream_pos = 0; stream_pos < source_.size(); ++stream_pos) {
				std::string cur_char = source_.substr(stream_pos, 1);

				if (!in_string) {
					if (cur_char == "\"" || cur_char == " " || cur_char == "\t" ||
						cur_char == "+" ||
						cur_char == "-" ||
						cur_char == "*" ||
						cur_char == "/" ||
						cur_char == ">" ||
						cur_char == "<" ||
						cur_char == "=" ||
						cur_char == "]" ||
						cur_char == "[" ||
						cur_char == "(" ||
						cur_char == ")" ||
						cur_char == "}" ||
						cur_char == "{" ||
						cur_char == "," ||
						cur_char == "!" ||
						cur_char == "." ||
						cur_char == ":" ||
						cur_char == ";"
						) {
						if (token_str != "") {
							token new_token;
							new_token.val = token_str;
							new_token.type = type::LITERAL;
							new_token.streamPos = stream_pos - token_str.length();
							token_stream.push_back(new_token);
						}
						if (cur_char == "+" ||
							cur_char == "-" ||
							cur_char == "*" ||
							cur_char == "/" ||
							cur_char == ">" ||
							cur_char == "<" ||
							cur_char == "=" ||
							cur_char == "]" ||
							cur_char == "[" ||
							cur_char == "(" ||
							cur_char == ")" ||
							cur_char == "}" ||
							cur_char == "{" ||
							cur_char == "," ||
							cur_char == "!" ||
							cur_char == "." ||
							cur_char == ":" ||
							cur_char == ";"
							) {
							bool skip = false;
							if (cur_char == ">" ||
								cur_char == "<" ||
								cur_char == "!" ||
								cur_char == "="
								) {
								if (source_.substr(stream_pos + 1, 1) == "=") {
									skip = true;
									cur_char += "=";
								}
							}
							if (cur_char == ":") {
								if (source_.substr(stream_pos + 1, 1) == ":") {
									skip = true;
									cur_char += ":";
								}
							}
							token operator_token;
							operator_token.val = cur_char;
							operator_token.streamPos = stream_pos;
							operator_token.type = type::OPERATOR;
							if (cur_char == ";") {
								operator_token.type = type::ENDOFSTATEMENT;
							}
							token_stream.push_back(operator_token);
							if (skip) {
								stream_pos++;
							}
						}
						token_str = "";
						if (cur_char == "\"") {
							token_str = "\"" + token_str;
						}
					}
					else {
						if (
							token_str == "<=" ||
							token_str == ">=" ||
							token_str == "!=" ||
							token_str == "::" ||
							token_str == "=="
							) {
							token new_token;
							new_token.val = token_str;
							new_token.streamPos = stream_pos - token_str.length();
							new_token.type = type::OPERATOR;
							token_stream.push_back(new_token);
							token_str = "";
						}
						else {
							if (cur_char != "\n") {
								token_str += cur_char;
							}
						}
					}
				}
				else {
					token_str += cur_char;
				}
				if (cur_char == "\"") {
					if (!in_string) {
						in_string = true;
					}
					else {
						if (source_.substr(stream_pos + 1, 1) == "\"") {
							token_str += "\"";
							stream_pos++;
						}
						else {
							in_string = false;
						}
					}
				}
			}
			return token_stream;
		}

		void add_reserved_word(std::string input_) {
			std::regex check_binary(".*b:.+?\\s(.+?)\\s.+");
			std::smatch matches;
			if (std::regex_match(input_, matches, check_binary)) {
				reserved_words.insert(matches[1]);
			}
			else {
				std::regex check_unary(".*u:(.+)?\\s.+");
				if (std::regex_match(input_, matches, check_unary)) {
					reserved_words.insert(matches[1]);
				}
				else {
					std::regex check_null(".*n:(.+)");
					if (std::regex_match(input_, matches, check_null)) {
						reserved_words.insert(matches[1]);
					}
				}
			}
		}

		bool is_reserved_word(std::string input_) {
			std::transform(input_.begin(), input_.end(), input_.begin(), ::tolower);
			if (reserved_words.count(input_) > 0) {
				return true;
			}
			return false;
		}
	}
}