#include "process.hpp"

namespace carma {
	namespace process {
		std::string process_input(std::string input_, bool do_pretty_, bool minimize_) {
			token_list tokens = tokenize(input_);

			//process_new_keyword(tokens, tokens.begin(), tokens.end());
			//clean_token_list(tokens);
			process_accessors(tokens, tokens.begin(), tokens.end());
			clean_token_list(tokens);
			//process_method_calls(tokens, tokens.begin());
			//clean_token_list(tokens);
			//process_simple_assigments(tokens, tokens.begin());
			//clean_token_list(tokens);
			//tokens = tokenize(build_string(tokens));
			//process_array_accessors(tokens, tokens.begin());
			

			//tokens = tokenize(build_string(tokens));
			//process_spawn_keyword(tokens, tokens.begin());
			//process_new_keyword(tokens, tokens.begin());
			//tokens = tokenize(build_string(tokens));
			//process_del_keyword(tokens, tokens.begin());
			//tokens = minimize(tokens, tokens.begin(), tokens.end());



			if(do_pretty_)
				return build_string_pretty(tokens, tokens.begin(), tokens.end());
			else
				return build_string(tokens, tokens.begin(), tokens.end());
			
		}
	}
}