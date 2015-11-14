#include "compiler.hpp"

namespace carma {
	namespace compiler {

		void process_accessors(token_list &tokens_) {
			for (token_entry current_token = tokens_.begin(); current_token != tokens_.end(); ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				if (std::next(current_token) != tokens_.end() && std::next(current_token)->val == ".") {
					auto dot_token = std::next(current_token);
					auto member_token = std::next(current_token, 2);
					if (member_token == tokens_.end())
						continue;
					auto next_token = std::next(current_token, 3);
					if (next_token == tokens_.end())
						continue;
					if ((next_token->type == carma::type::OPERATOR || next_token->type == carma::type::ENDOFSTATEMENT) && next_token->val != "=" && next_token->val != "(") {
						std::string simple_assignment = "(" + current_token->val + " getVariable \"" + member_token->val + "\")";
						if (next_token->val != ".") {
							current_token->type = carma::type::EMPTY;
							dot_token->type = carma::type::EMPTY;
							member_token->val = simple_assignment;
							current_token = member_token;
						}
						else {
							member_token->val = simple_assignment;
							dot_token->type = carma::type::EMPTY;
							current_token->type = carma::type::EMPTY;
							current_token = dot_token;
						}
					}
				}
			}
		}

		void process_simple_assigments(token_list &tokens_, token_entry start_entry_) {
			uint32_t dummy_block_counter = 0;
			process_simple_assigments(tokens_, start_entry_, dummy_block_counter);
		}


		void process_simple_assigments(token_list &tokens_, token_entry start_entry_, uint32_t &outer_block_counter) {
			for (token_entry current_token = start_entry_; current_token != tokens_.end(); ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				if (std::next(current_token) != tokens_.end() && std::next(current_token)->val == ".") {
					auto object_token = current_token;
					auto dot_token = std::next(current_token);
					auto member_token = std::next(current_token, 2);
					if (member_token == tokens_.end())
						continue;
					auto next_token = std::next(current_token, 3);
					if (next_token == tokens_.end())
						continue;
					if (next_token->val == "=") {
						token_entry value_token = std::next(next_token);
						uint32_t block_counter = 0;
						if (value_token == tokens_.end())
							continue;
						token_list value_tokens;
						for (value_token; value_token != tokens_.end() && (value_token->val != ";" || block_counter > 0); ++value_token) {
							if (value_token->type == carma::type::EMPTY)
								continue;
							if (value_token->val == "[" || value_token->val == "{" || value_token->val == "(")
								block_counter++;
							if (value_token->val == "]" || value_token->val == "}" || value_token->val == ")")
								block_counter--;
							if (value_token->val == ".") {
								process_simple_assigments(tokens_, std::prev(value_token), block_counter);
							}
							value_tokens.push_back(*value_token);
						}

						next_token->val = object_token->val + " setVariable [\"" + member_token->val + "\", ";
						object_token->type = carma::type::EMPTY;
						dot_token->type = carma::type::EMPTY;
						member_token->type = carma::type::EMPTY;
						token end_token;
						end_token.type = carma::type::UNKNOWN;
						end_token.val = "]";
						outer_block_counter++;
						tokens_.insert(value_token, end_token);
					}
				}
			}
		}

		void process_method_calls(token_list &tokens_, token_entry start_entry_) {
			for (token_entry current_token = start_entry_; current_token != tokens_.end(); ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				if (std::next(current_token) != tokens_.end() && std::next(current_token)->val == ".") {
					auto object_token = current_token;
					auto dot_token = std::next(current_token);
					auto member_token = std::next(current_token, 2);
					if (member_token == tokens_.end())
						continue;
					auto next_token = std::next(current_token, 3);
					if (next_token == tokens_.end())
						continue;
					if (next_token->val == "(") {
						token_entry value_token = std::next(next_token);
						uint32_t block_counter = 0;
						if (value_token == tokens_.end())
							continue;
						token_list value_tokens;
						for (value_token; value_token != tokens_.end() && (value_token->val != ")" || block_counter > 0); ++value_token) {
							if (value_token->type == carma::type::EMPTY)
								continue;
							if (value_token->val == "[" || value_token->val == "{" || value_token->val == "(")
								block_counter++;
							if (value_token->val == "]" || value_token->val == "}" || value_token->val == ")")
								block_counter--;
							if (value_token->val == ".") {
								process_method_calls(tokens_, std::prev(value_token));
							}
							value_tokens.push_back(*value_token);
							value_token->type = carma::type::EMPTY;
						}
						std::string value_string = build_string(value_tokens);
						next_token->type = carma::type::EMPTY;
						object_token->type = carma::type::EMPTY;
						dot_token->type = carma::type::EMPTY;
						member_token->type = carma::type::EMPTY;
						value_token->val = "([" + object_token->val + ",\"" + member_token->val + "\",[" + value_string + "]] call carma2_fnc_methodInvoke)";
					}
				}
			}
		}

		void process_new_keyword(token_list &tokens_, token_entry start_entry_) {
			for (token_entry current_token = start_entry_; current_token != tokens_.end(); ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				if (current_token->val == "new") {
					auto object_token = std::next(current_token);
					if (object_token == tokens_.end())
						continue;
					auto next_token = std::next(current_token, 2);
					if (next_token == tokens_.end())
						continue;
					if (next_token->val == "(") {
						token_entry value_token = std::next(next_token);
						uint32_t block_counter = 0;
						if (value_token == tokens_.end())
							continue;
						token_list value_tokens;
						for (value_token; value_token != tokens_.end() && (value_token->val != ")" || block_counter > 0); ++value_token) {
							if (value_token->type == carma::type::EMPTY)
								continue;
							if (value_token->val == "[" || value_token->val == "{" || value_token->val == "(")
								block_counter++;
							if (value_token->val == "]" || value_token->val == "}" || value_token->val == ")")
								block_counter--;
							if (value_token->val == ".") {
								process_new_keyword(tokens_, std::prev(value_token));
							}
							value_tokens.push_back(*value_token);
							value_token->type = carma::type::EMPTY;
						}
						std::string value_string = build_string(value_tokens);
						next_token->type = carma::type::EMPTY;
						object_token->type = carma::type::EMPTY;
						current_token->type = carma::type::EMPTY;
						value_token->val = "([[" + value_string + "], " + object_token->val + ", __FILE__, __LINE__] call carma2_fnc_newObject)";
					}
				}
			}
		}

		void process_del_keyword(token_list &tokens_, token_entry start_entry_) {
			for (token_entry current_token = start_entry_; current_token != tokens_.end(); ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				if (current_token->val == "del") {
					auto object_token = std::next(current_token);
					if (object_token == tokens_.end())
						continue;
					auto next_token = std::next(current_token, 2);
					if (next_token == tokens_.end())
						continue;
					if (next_token->val == ";") {
						object_token->val = "[" + object_token->val + "] call carma2_fnc_delObject";
						current_token->type = carma::type::EMPTY;
					}
				}
			}
		}

		std::string build_string(const token_list &tokens_) {
			std::string output = "";
			for (auto current_token = tokens_.begin(); current_token != tokens_.end(); ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				output += current_token->val;
				if (std::next(current_token) != tokens_.end() && current_token->type == carma::type::LITERAL && std::next(current_token)->type == carma::type::LITERAL) {
					output += " ";
				}
				else if (current_token->type == carma::type::ENDOFSTATEMENT) {
					output += " ";
				}
			};
			return output;
		}
	}
};