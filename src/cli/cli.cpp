#include <stdio.h>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>

#include "compiler\compiler.hpp"

namespace carma {
	std::string build_string(token_list tokens_) {
		std::string output = "";
		for (auto current_token = tokens_.begin(); current_token != tokens_.end(); ++current_token) {
			if (current_token->type == carma::type::EMPTY)
				continue;
			output += current_token->val;
			if (std::next(current_token) != tokens_.end() && current_token->type == carma::type::LITERAL && std::next(current_token)->type == carma::type::LITERAL) {
				output += " ";
			}
			else if (current_token->type == carma::type::ENDOFSTATEMENT) {
				output += " ";
			}
		};
		return output;
	}
}


int main(int argc, char **argv) {
	std::string input_str = "(_test select 0).fuck;";
	carma::token_list tokens = carma::tokenize(input_str);
	
	carma::process_accessors(tokens);
	std::cout << carma::build_string(tokens) << "\n\n";
	uint32_t dummy_block_counter = 0;
	carma::process_simple_assigments(tokens, tokens.begin(), dummy_block_counter);
	std::cout << carma::build_string(tokens) << "\n\n";
	carma::process_method_calls(tokens, tokens.begin());
	std::cout << carma::build_string(tokens) << "\n\n";
	carma::process_new_keyword(tokens, tokens.begin());
	std::cout << carma::build_string(tokens) << "\n\n";
	carma::process_del_keyword(tokens, tokens.begin());
	std::cout << carma::build_string(tokens) << "\n\n";

	return 0;
}