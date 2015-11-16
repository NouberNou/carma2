#pragma once

#include <list>
#include <map>
#include <string>
#include "tokenizer.hpp"

using namespace carma::tokenizer;

namespace carma {
	namespace compiler {
		void process_accessors(token_list &tokens_, token_entry start_entry_, token_entry end_entry_);
		void process_spawn_keyword(token_list &tokens_, token_entry start_entry_, token_entry end_entry_);
		void process_new_keyword(token_list &tokens_, token_entry start_entry_, token_entry end_entry_);
		std::string build_string(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);
		std::string build_string_pretty(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);
	};
};