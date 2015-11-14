#pragma once

#include <list>
#include <map>
#include <string>
#include "tokenizer.hpp"

using namespace carma::tokenizer;

namespace carma {
	namespace compiler {
		void process_accessors(token_list &tokens_, token_entry start_entry_);
		void process_simple_assigments(token_list &tokens_, token_entry start_entry_);
		void process_simple_assigments(token_list &tokens_, token_entry start_entry_, uint32_t &outer_block_counter);
		void process_method_calls(token_list &tokens_, token_entry start_entry_);
		void process_spawn_keyword(token_list &tokens_, token_entry start_entry_);
		void process_array_accessors(token_list &tokens_, token_entry start_entry_);
		void process_new_keyword(token_list &tokens_, token_entry start_entry_);
		void process_del_keyword(token_list &tokens_, token_entry start_entry_);
		std::string build_string(const token_list &tokens_);
	};
};