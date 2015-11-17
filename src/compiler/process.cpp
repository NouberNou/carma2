#include "process.hpp"

namespace carma {
	namespace process {
		std::string process_input(std::string input_, bool do_pretty_, bool minimize_) {
			token_list tokens = tokenize(input_);

			//process_new_keyword(tokens, tokens.begin(), tokens.end());
			//clean_token_list(tokens);
            return CarmaScope(compiler::context::Type::FILE, tokens, tokens.begin(), tokens.end()).Compile();
            
			/*if(do_pretty_)
				return build_string_pretty(tokens, tokens.begin(), tokens.end());
			else
				return build_string(tokens, tokens.begin(), tokens.end());
			*/
		}
	}
}