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
	token_list tokenize(std::string source_);
};


