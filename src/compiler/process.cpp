#include "process.hpp"
#include "rules\object_creation.hpp"
#include "rules\script_commands.hpp"
#include "rules\function_declaration.hpp"
#include "rules\control_structures.hpp"
#include "rules\operator_handler.hpp"


namespace carma {
	namespace process {
		std::string process_input(std::string input_, bool do_pretty_, bool minimize_) {
			token_list tokens = tokenize(input_);
            
            compiler::context::rules.push_back(new rules::new_keyword_rule());
            compiler::context::rules.push_back(new rules::del_keyword_rule());
            compiler::context::rules.push_back(new rules::anon_object_rule());

            compiler::context::rules.push_back(new rules::function_keyword_rule());
            compiler::context::rules.push_back(new rules::return_keyword_rule());

            compiler::context::rules.push_back(new rules::if_statement_rule());
            compiler::context::rules.push_back(new rules::switch_statement_rule());
            compiler::context::rules.push_back(new rules::while_statement_rule());
            compiler::context::rules.push_back(new rules::foreach_statement_rule());
            compiler::context::rules.push_back(new rules::try_block_rule());
            compiler::context::rules.push_back(new rules::waituntil_statement_rule());

            compiler::context::rules.push_back(new rules::operator_dot_rule());
            compiler::context::rules.push_back(new rules::member_access_rule());
            compiler::context::rules.push_back(new rules::function_call_rule());
            compiler::context::rules.push_back(new rules::array_access_rule());

            return context(compiler::context::type::FILE, tokens, tokens.begin(), tokens.end()).compile();
		}
	}
}