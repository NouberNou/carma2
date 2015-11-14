#include "process.hpp"

namespace carma {
	namespace process {
		std::string process_input(std::string input_) {
			token_list tokens = tokenize(input_);

			process_accessors(tokens, tokens.begin());
			process_simple_assigments(tokens, tokens.begin());
			process_array_accessors(tokens, tokens.begin());
			process_method_calls(tokens, tokens.begin());
			process_array_accessors(tokens, tokens.begin());
			tokens = tokenize(build_string(tokens));
			process_new_keyword(tokens, tokens.begin());
			tokens = tokenize(build_string(tokens));
			process_del_keyword(tokens, tokens.begin());

			return build_string(tokens);
			
		}
	}
}