#include <stdio.h>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>

#include "compiler\compiler.hpp"

namespace carma {
	
}


int main(int argc, char **argv) {
	std::string input_str = "_fuck.ok();";
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