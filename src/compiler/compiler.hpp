#pragma once

#include <list>
#include <map>
#include <string>


namespace carma {
	namespace type {
		enum {
			UNKNOWN,
			OPERATOR,
			SCALAR,
			STRING,
			LITERAL,
			ARRAYSTART,
			ARRAYEND,
			CODESTART,
			CODEEND,
			ENDOFSTATEMENT,
			CODEBLOCK,
			ARRAY,
			EMPTY
		};
	};

	struct token {
		std::string val;
		std::string::size_type streamPos;
		unsigned int type;
		std::list<token> blockElements;
	};
	typedef std::list<token> token_list;
	typedef std::list<carma::token, std::allocator<carma::token>>::iterator token_entry;

	token_list tokenize(std::string source_);
	void process_accessors(token_list &tokens_);
	void process_simple_assigments(token_list &tokens_, token_entry start_entry_, uint32_t &outer_block_counter);
	void process_method_calls(token_list &tokens_, token_entry start_entry_);
	void process_new_keyword(token_list &tokens_, token_entry start_entry_);
	void process_del_keyword(token_list &tokens_, token_entry start_entry_);
	std::string build_string(const token_list &tokens_);

};