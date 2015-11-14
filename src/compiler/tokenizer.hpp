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
			METHODCALL
		};
	};

	namespace tokenizer {
		struct token {
			std::string val;
			std::string::size_type streamPos;
			unsigned int type;
			std::list<token> blockElements;
		};
		typedef std::list<token> token_list;
		typedef std::list<token, std::allocator<token>>::iterator token_entry;

		token_list tokenize(std::string source_);
		void add_reserved_word(std::string input_);
		bool is_reserved_word(std::string input_);
	}
};