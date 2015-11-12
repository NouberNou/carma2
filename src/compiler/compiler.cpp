#include "compiler.hpp"

namespace carma {
	token_list tokenize(std::string source_) {
		std::list<token> token_stream;
		std::string token_str;
		bool in_string = false;
		for (std::string::size_type stream_pos = 0; stream_pos < source_.size(); ++stream_pos) {
			std::string cur_char = source_.substr(stream_pos, 1);

			if (!in_string) {
				if (cur_char == "\"" || cur_char == " " || cur_char == "\t" ||
					cur_char == "+" ||
					cur_char == "-" ||
					cur_char == "*" ||
					cur_char == "/" ||
					cur_char == ">" ||
					cur_char == "<" ||
					cur_char == "=" ||
					cur_char == "]" ||
					cur_char == "[" ||
					cur_char == "(" ||
					cur_char == ")" ||
					cur_char == "}" ||
					cur_char == "{" ||
					cur_char == "," ||
					cur_char == "!" ||
					cur_char == "." ||
					cur_char == ":" ||
					cur_char == ";"
					) {
					if (token_str != "") {
						token new_token;
						new_token.val = token_str;
						new_token.type = type::LITERAL;
						new_token.streamPos = stream_pos - token_str.length();
						token_stream.push_back(new_token);
					}
					if (cur_char == "+" ||
						cur_char == "-" ||
						cur_char == "*" ||
						cur_char == "/" ||
						cur_char == ">" ||
						cur_char == "<" ||
						cur_char == "=" ||
						cur_char == "]" ||
						cur_char == "[" ||
						cur_char == "(" ||
						cur_char == ")" ||
						cur_char == "}" ||
						cur_char == "{" ||
						cur_char == "," ||
						cur_char == "!" ||
						cur_char == "." ||
						cur_char == ":" ||
						cur_char == ";"
						) {
						bool skip = false;
						if (cur_char == ">" ||
							cur_char == "<" ||
							cur_char == "!" ||
							cur_char == "="
							) {
							if (source_.substr(stream_pos + 1, 1) == "=") {
								skip = true;
								cur_char += "=";
							}
						}
						if (cur_char == ":") {
							if (source_.substr(stream_pos + 1, 1) == ":") {
								skip = true;
								cur_char += ":";
							}
						}
						token operator_token;
						operator_token.val = cur_char;
						operator_token.streamPos = stream_pos;
						operator_token.type = type::OPERATOR;
						if (cur_char == ";") {
							operator_token.type = type::ENDOFSTATEMENT;
						}
						token_stream.push_back(operator_token);
						if (skip) {
							stream_pos++;
						}
					}
					token_str = "";
					if (cur_char == "\"") {
						token_str = "\"" + token_str;
					}
				}
				else {
					if (
						token_str == "<=" ||
						token_str == ">=" ||
						token_str == "!=" ||
						token_str == "::" ||
						token_str == "=="
						) {
						token new_token;
						new_token.val = token_str;
						new_token.streamPos = stream_pos - token_str.length();
						new_token.type = type::OPERATOR;
						token_stream.push_back(new_token);
						token_str = "";
					}
					else {
						if (cur_char != "\n") {
							token_str += cur_char;
						}
					}
				}
			}
			else {
				token_str += cur_char;
			}
			if (cur_char == "\"") {
				if (!in_string) {
					in_string = true;
				}
				else {
					if (source_.substr(stream_pos + 1, 1) == "\"") {
						token_str += "\"";
						stream_pos++;
					}
					else {
						in_string = false;
					}
				}
			}
		}
		return token_stream;
	}

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
					}

					next_token->val = "([" + object_token->val + ",\"" + member_token->val + "\",[";
					object_token->type = carma::type::EMPTY;
					dot_token->type = carma::type::EMPTY;
					member_token->type = carma::type::EMPTY;
					token end_token;
					end_token.type = carma::type::UNKNOWN;
					end_token.val = "]] call carma2_fnc_methodInvoke";
					tokens_.insert(value_token, end_token);
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
					}

					next_token->val = "([[";
					object_token->type = carma::type::EMPTY;
					current_token->type = carma::type::EMPTY;
					token end_token;
					end_token.type = carma::type::UNKNOWN;
					end_token.val = "], " + object_token->val + ", __FILE__, __LINE__] call carma2_fnc_newObject";
					tokens_.insert(value_token, end_token);
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
};