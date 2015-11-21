#pragma once

#include <list>
#include <map>
#include <string>
#include <unordered_set>

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
			EMPTY,
			METHODCALL,
			MEMBERACCESSOR,
			ARRAYACCESSOR,
			ASSIGNMENTOPARRAY,
			ASSIGNMENTOPMEMBER,
			FUNCTIONCALL
		};
	};

	namespace tokenizer {
		struct token {
			std::string val;
			std::string::size_type stream_pos;
			unsigned int type;
			std::list<token> block_elements;
		};
		typedef std::list<token> token_list;
		typedef std::list<token, std::allocator<token>>::iterator token_entry;

		token_list tokenize(std::string source_);
		void add_reserved_word(std::string input_);
		bool is_reserved_word(std::string input_);

		void clean_token_list(token_list &tokens_);
        bool is_number(const std::string& input);
	}
};