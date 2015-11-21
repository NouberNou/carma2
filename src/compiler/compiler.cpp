#include "compiler.hpp"
#include <unordered_map>
#include <sstream>
#include <iostream>
#include "rules\object_creation.hpp"
#include "rules\script_commands.hpp"
#include "rules\function_declaration.hpp"
#include "rules\control_structures.hpp"
#include "rules\operator_handler.hpp"

namespace carma {
	namespace compiler {

        context::context(type context_type_, token_list& tokens) : parent(nullptr), context_type(context_type_), _tokens(tokens),
            _start_token(tokens.begin()), _end_token(tokens.end())
        {
            std::vector<context*> functions = std::vector<context*>();
        }

        context::context(context* parent_, type context_type_, token_list& tokens_) : parent(parent_), context_type(context_type_), _tokens(tokens_),
            _start_token(tokens_.begin()), _end_token(tokens_.end())
        {
            std::vector<context*> functions = std::vector<context*>();
        }

        context::context(type context_type_, token_list& tokens_, token_entry& start_entry_, token_entry& end_entry_) : parent(nullptr), context_type(context_type_), _tokens(tokens_),
            _start_token(start_entry_), _end_token(end_entry_)
        {
            std::vector<context*> functions = std::vector<context*>();
        }

        context::context(context* parent_, type context_type_, token_list& tokens_, token_entry& start_entry_, token_entry& end_entry_) : parent(parent_), context_type(context_type_), _tokens(tokens_),
            _start_token(start_entry_), _end_token(end_entry_)
        {
            std::vector<context*> functions = std::vector<context*>();
        }

        std::string context::compile() 
        {
            try {
                token_entry start_entry_ = _start_token;
                token_entry end_entry_ = _end_token;

                switch (context_type) {
                case type::FUNCTION_PARAMS:
                    compile_params(start_entry_, end_entry_);
                    break;
                case type::CONTROL_STRUCTURE:
                    compile_control_structure(start_entry_, end_entry_);;
                    break;
                case type::OBJECT:
                    compile_object(start_entry_, end_entry_);
                    break;
                default:
                    compile_scope(start_entry_, end_entry_);
                    break;
                }

                auto str = compiler::build_string(_tokens, _start_token, _end_token);
                return str;
            }
            catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                return "throw format['" + std::string(e.what()) +"'];";
            } catch (exception::invalid_context& e) {
                std::cout << e.message() << std::endl;
                return "throw format['" + e.message() + "'];";
            }
            catch (exception::invalid_operator& e) {
                std::cout << e.message() << std::endl;
                return "throw format['" + e.message() + "'];";
            }
            catch (exception::missing_bracket& e) {
                std::cout << e.message() << std::endl;
                return "throw format['" + e.message() + "'];";
            }
            catch (exception::missing_quotation& e) {
                std::cout << e.message() << std::endl;
                return "throw format['" + e.message() + "'];";
            }
            catch (exception::syntax_error& e) {
                std::cout << e.message() << std::endl;
                return "throw format['" + e.message() + "'];";
            }
            catch (exception::unknown& e) {
                std::cout << e.message() << std::endl;
                return "throw format['" + e.message() + "'];";
            }
        }

        void context::compile_scope(token_entry& start_entry_, token_entry& end_entry_) {
            for (token_entry current_token = start_entry_; current_token != end_entry_; ++current_token) {
                if (current_token->type == carma::type::EMPTY)
                    continue;

                if (current_token->val == "new" || current_token->val == "del") {
                    compile_object(current_token, end_entry_);
                    continue;
                }
                if (current_token->val == "function") {
                    compile_function(current_token, end_entry_);
                    continue;
                }
                if (current_token->val == "return") {
                    carma::rules::function_declaration::return_keyword(*this, _tokens, current_token, end_entry_);
                    continue;
                }
                if (current_token->val == "if" || current_token->val == "switch" || current_token->val == "while" || current_token->val == "waituntil") {
                    compile_control_structure(current_token, end_entry_);
                    continue;
                }

                if (std::next(current_token) != end_entry_ && (
                    std::next(current_token)->val == "." ||
                    std::next(current_token)->val == "::" ||
                    std::next(current_token)->val == "[" ||
                    std::next(current_token)->val == "(" ||
                    std::next(current_token)->val == "{"
                    )) {
                    // std::cout << "Process operator: " << current_token->val << " operator: " << std::next(current_token)->val << std::endl;
                    if (std::next(current_token)->val == "." || std::next(current_token)->val == "::") {
                        rules::operator_handler::dot_operator(*this, _tokens, current_token, end_entry_);
                    }
                    else if (std::next(current_token)->val == "[") {
                        rules::operator_handler::array_access_operator(*this, _tokens, current_token, end_entry_);
                    }
                    else if (std::next(current_token)->val == "{") {
                        rules::operator_handler::member_access_operator(*this, _tokens, current_token, end_entry_);
                    }
                    else if (std::next(current_token)->val == "(") {
                        rules::operator_handler::method_call_operator(*this, _tokens, current_token, end_entry_);
                    }
                }
            }
        }

        void context::compile_params(token_entry& start_entry_, token_entry& end_entry_) {
            // expected input: (value, value2) OR () OR (value)
            // should return: "params["_value", "_value2"];, "", "params ["_value"];"

            token_entry arg_token = start_entry_;
            typedef std::pair<std::string, std::string> arg_entry;
            std::list<arg_entry> args;

            for (arg_token; arg_token != end_entry_ && (arg_token->val != ")"); ++arg_token) {
                if (arg_token->type == carma::type::EMPTY)
                    continue;
                if (arg_token->type != carma::type::LITERAL && arg_token->val != "("  && arg_token->val != ")" && arg_token->val != ",")
                    throw exception::syntax_error("non-literal var name in function arguments");
               //if (arg_token->val.substr(0, 1) != "_")
                //    throw Carma_syntax_error_exception("non-local var name in function arguments");
                    // arg_token->val = "_"+arg_token->val;

                auto test_token = std::next(arg_token);
                if (test_token == _tokens.end())
                    continue;

                if (test_token->val == "," || test_token->val == ")") {
                    args.push_back(arg_entry(arg_token->val, ""));
                }
            }

            compiler::empty_tokens(_tokens, start_entry_, end_entry_);

            if (args.size() > 0) {
                std::stringstream params_string;
                for (auto param_entry = args.begin(); param_entry != args.end(); ++param_entry) {
                    if (param_entry->second != "") {
                        params_string << "[\"" << param_entry->first << "\", " << param_entry->second << "];";
                    }
                    else {
                        params_string << "\"" + param_entry->first + "\"";
                    }
                    if (std::next(param_entry) != args.end())
                        params_string << ", ";
                }
                start_entry_->val = "params[" + params_string.str() + "]; ";
                start_entry_->type = carma::type::ASSIGNMENTOPARRAY;
            }
        }

        void context::compile_object(token_entry& start_entry_, token_entry& end_entry_) { 
            if (start_entry_->val == "new") {
                // copy of prototype
                carma::rules::object_creation::new_object(*this, _tokens, start_entry_, end_entry_);
            }
            else if (start_entry_->val == "{") {
                // create anomyous js style prototype object
                // TODO implement
            }
            else if (start_entry_->val == "del") {
                carma::rules::object_creation::handle_del_keyword(*this, _tokens, start_entry_, end_entry_);
            }
        }

        void context::compile_function(token_entry& start_entry_, token_entry& end_entry_) {
            if (start_entry_->val == "function") {
                if (std::next(start_entry_)->val == "(") {
                    carma::rules::function_declaration::function_keyword(*this, _tokens, start_entry_, end_entry_);
                }
                else {
                    carma::rules::function_declaration::function_keyword(*this, _tokens, start_entry_, end_entry_);
                }
            }  
        }

        void context::compile_control_structure(token_entry& start_entry_, token_entry& end_entry_) {

            if (start_entry_->val == "if") {
                rules::control_structures::if_statement(*this, _tokens, start_entry_, end_entry_);
            }
            else if (start_entry_->val == "switch") {
                rules::control_structures::switch_statement(*this, _tokens, start_entry_, end_entry_);

            }
            else if (start_entry_->val == "while") {
                rules::control_structures::while_statement(*this, _tokens, start_entry_, end_entry_);
            }
            else if (start_entry_->val == "waituntil") {
                rules::control_structures::waituntil_statement(*this, _tokens, start_entry_, end_entry_);
            }
            else {
                throw exception::syntax_error("Not a control structure"); // TODO proper exception here
            }
        }
        
        bool context::in_scope(type context_type_) {
            if (context_type == context_type_)
                return true;
            if (parent == nullptr)
                return false;
            return parent->in_scope(context_type_);
        }

        void empty_tokens(token_list& tokens_, token_entry& start_entry_, token_entry& end_entry_) {
            auto start = start_entry_;
            for (start; start != end_entry_; ++start) {
                start->type = carma::type::EMPTY;
            }
        }

		std::string build_string(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_) {
            try {
                std::string output = "";
                for (auto current_token = start_entry_; current_token != end_entry_; ++current_token) {
                    if (current_token->type == carma::type::EMPTY)
                        continue;
                    output += current_token->val;

                    if (std::next(current_token) != end_entry_ && current_token->type == carma::type::LITERAL && std::next(current_token)->type == carma::type::LITERAL) {
                        output += " ";
                    }
                    else if (current_token->type == carma::type::ENDOFSTATEMENT) {
                        output += " ";
                    }
                };
                return output;
            }
            catch (std::exception e) {
                std::cout << e.what() << std::endl;
            }
		}

		std::string build_string_pretty(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_) {
			token_list tokens = tokenize(build_string(tokens_, start_entry_, end_entry_));
			std::string output = "";
			int block_indent = 0;
			for (auto current_token = tokens.begin(); current_token != tokens.end() && current_token != end_entry_; ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				output += current_token->val;
				if (std::next(current_token) != tokens.end() && std::next(current_token)->type == carma::type::LITERAL) {
					if(current_token->val != "}" || current_token->val != "{")
							output += " ";
				}
				if (std::next(current_token) != tokens.end() && (
					std::next(current_token)->val == "="
					)) {
					output += " ";
				}
				if (current_token->val == "="
					) {
					output += " ";
				}
				if (current_token->val == "{")
					block_indent++;
				if (current_token->val == "}")
					block_indent--;
				if (current_token->val == ";" || current_token->val == "{") {
					output += "\n";
					int b = block_indent;
					if (std::next(current_token) != tokens.end() && std::next(current_token)->val == "}")
						b--;
					for (int i = 0; i < b; ++i)
						output += "    ";
				}
				//if (current_token->val == "{" && (std::next(current_token) != tokens.end() && std::next(current_token)->val != ";")) {
				//	output += "\n";
				//}
			};
			return output;
		}

		token_list minimize(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_) {
			token_list tokens = tokenize(build_string(tokens_, start_entry_, end_entry_));
			std::unordered_set<std::string> reserved_variables;
			reserved_variables.insert("_this");
			reserved_variables.insert("_thisObj");
			reserved_variables.insert("_thisScript");
			reserved_variables.insert("_x");
			reserved_variables.insert("_forEachIndex");

			std::string output = "";
			token_list minimized_tokens;
			std::unordered_map<std::string, std::string> unique_vars;
			uint32_t id = 0;
			for (auto current_token = tokens.begin(); current_token != tokens.end(); ++current_token) {
				if (current_token->type == carma::type::LITERAL) {
					if (!is_reserved_word(current_token->val) && current_token->val.substr(0, 1) == "_") {
						if (reserved_variables.find(current_token->val) == reserved_variables.end()) {
							if (unique_vars.find(current_token->val) == unique_vars.end()) {
								std::stringstream var_name;
								var_name << "_" << id++;
								unique_vars[current_token->val] = var_name.str();
							}
							current_token->val = unique_vars[current_token->val];
						}
					}
				}
				minimized_tokens.push_back(*current_token);
			}

			for (auto entry : unique_vars) {
				std::cout << entry.first << ": " << entry.second << "\n";
			}

			return minimized_tokens;
		}
	}
};