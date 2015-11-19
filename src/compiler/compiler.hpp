#pragma once

#include <list>
#include <map>
#include <string>
#include "tokenizer.hpp"

using namespace carma::tokenizer;

namespace carma {
	namespace compiler {

		struct block {
			std::string type;
			block *parent;
		};

		void process_pass_1(token_list &tokens_, token_entry start_entry_, token_entry end_entry_);
		void process_pass_2(token_list &tokens_, token_entry start_entry_, token_entry end_entry_, block &parent_);

		std::string build_string(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);
		std::string build_string_pretty(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);
		token_list minimize(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);
	};
};