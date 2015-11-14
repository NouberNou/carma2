#include <stdio.h>
#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>

#include "compiler\compiler.hpp"

namespace carma {
	namespace cli {
		enum arg_options {
			output_file
		};


		uint32_t find_arg_code(std::string arg_) {
			if (arg_ == "-o") return arg_options::output_file;
			return -1;
		}
	}
}


int main(int argc, char **argv) {
	uint32_t arg_index = 1;
	if (argc < 2) {
		std::cout << "Usage: cli.exe [-o output.sqf] input.sqf\n";
		return 0;
	}
	std::string input_filename(argv[argc - 1]);
	std::string output_filename = "carma2_sqf_.sqf";
	for (arg_index; arg_index < argc-1; ++arg_index) {
		std::string arg(argv[arg_index]);
		switch (carma::cli::find_arg_code(arg)) {
		case carma::cli::arg_options::output_file:
			output_filename = std::string(argv[++arg_index]);
			break;
		};
	}

	std::ifstream input_file(input_filename);
	if (!input_file.is_open()) {
		std::cout << "Error: Cannot find input file \"" << input_filename << "\n";
		return 1;
	}
	std::string input_str((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

	carma::token_list tokens = carma::tokenize(input_str);

	carma::process_accessors(tokens);
	uint32_t dummy_block_counter = 0;
	carma::process_simple_assigments(tokens, tokens.begin(), dummy_block_counter);
	carma::process_method_calls(tokens, tokens.begin());
	tokens = carma::tokenize(carma::build_string(tokens));
	carma::process_new_keyword(tokens, tokens.begin());
	tokens = carma::tokenize(carma::build_string(tokens));
	carma::process_del_keyword(tokens, tokens.begin());
	std::ofstream compile_file(output_filename);
	if (!input_file.is_open()) {
		std::cout << "Error: Cannot open output file \"" << input_filename << "\n";
		return 1;
	}
	compile_file << carma::build_string(tokens);

	return 0;
}