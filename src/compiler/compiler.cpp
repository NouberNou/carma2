#include "compiler.hpp"

namespace carma {
	namespace compiler {

		void process_accessors(token_list &tokens_, token_entry start_entry_, token_entry end_entry_) {
			for (token_entry current_token = start_entry_; current_token != end_entry_; ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				if (current_token->val == "new") {
					auto object_token = std::next(current_token);
					if (object_token == tokens_.end())
						continue;
					if (object_token->type == carma::type::LITERAL) {
						uint32_t block_counter = 0;
						auto obj_token_end = object_token;
						if (obj_token_end == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging ( operator.

						auto obj_token_start = obj_token_end;

						for (obj_token_end; obj_token_end != end_entry_ && (obj_token_end->val != "(" || block_counter > 0); ++obj_token_end) {
							if (obj_token_end->type == carma::type::EMPTY)
								continue;
							if (obj_token_end->val == "[" || obj_token_end->val == "{" || obj_token_end->val == "(")
								block_counter++;
							if (obj_token_end->val == "]" || obj_token_end->val == "}" || obj_token_end->val == ")")
								block_counter--;
						}

						process_accessors(tokens_, obj_token_start, obj_token_end);
						std::string obj_string = build_string(tokens_, obj_token_start, obj_token_end);
						for (auto clear_token = obj_token_start; clear_token != obj_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}

						auto arg_token_end = std::next(obj_token_end);
						if (arg_token_end == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging ( operator.

						auto arg_token_start = arg_token_end;

						for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != ")" || block_counter > 0); ++arg_token_end) {
							if (arg_token_end->type == carma::type::EMPTY)
								continue;
							if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
								block_counter++;
							if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
								block_counter--;
						}

						process_accessors(tokens_, arg_token_start, arg_token_end);
						std::string arg_string = build_string(tokens_, arg_token_start, arg_token_end);
						for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}
						obj_token_end->type = carma::type::EMPTY;
						arg_token_start->type = carma::type::EMPTY;
						object_token->type = carma::type::EMPTY;
						current_token->type = carma::type::EMPTY;
						arg_token_end->type = carma::type::FUNCTIONCALL;
						arg_token_end->val = "([[" + arg_string + "], " + obj_string + ", _thisScript] call carma2_fnc_newObject)";
						current_token = --arg_token_end;
					}
					continue;
				}

				if (current_token->val == "del") {
					auto object_token = std::next(current_token);
					if (object_token == tokens_.end())
						continue;
					if (object_token->type == carma::type::LITERAL) {
						uint32_t block_counter = 0;
						auto obj_token_end = object_token;
						if (obj_token_end == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging ( operator.

						auto obj_token_start = obj_token_end;

						for (obj_token_end; obj_token_end != end_entry_ && (obj_token_end->val != ";" || block_counter > 0); ++obj_token_end) {
							if (obj_token_end->type == carma::type::EMPTY)
								continue;
							if (obj_token_end->val == "[" || obj_token_end->val == "{" || obj_token_end->val == "(")
								block_counter++;
							if (obj_token_end->val == "]" || obj_token_end->val == "}" || obj_token_end->val == ")")
								block_counter--;
						}

						process_accessors(tokens_, obj_token_start, obj_token_end);
						std::string obj_string = build_string(tokens_, obj_token_start, obj_token_end);
						for (auto clear_token = obj_token_start; clear_token != obj_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}

						object_token->type = carma::type::EMPTY;
						current_token->type = carma::type::EMPTY;
						--obj_token_end;
						obj_token_end->type = carma::type::UNKNOWN;
						obj_token_end->val = "([" + obj_string + "] call carma2_fnc_delObject)";
						current_token = obj_token_end;
					}
					continue;
				}

				if (std::next(current_token) != end_entry_ && (
					std::next(current_token)->val == "." ||
					std::next(current_token)->val == "::" ||
					std::next(current_token)->val == "[" || 
					std::next(current_token)->val == "(" ||
					std::next(current_token)->val == "{" 
					)) {
					if (std::next(current_token)->val == "." || std::next(current_token)->val == "::") {
						auto object_token = current_token;
						auto dot_token = std::next(current_token);
						auto member_token = std::next(current_token, 2);
						if (member_token == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging . operator.
						auto following_token = std::next(current_token, 3);
						if((following_token == end_entry_ || following_token->val != "(") && following_token->val != "=") {
							object_token->type = carma::type::EMPTY;
							dot_token->type = carma::type::EMPTY;
							member_token->type = carma::type::MEMBERACCESSOR;
							if (dot_token->val == ".") {
								member_token->val = "(" + object_token->val + " getVariable \"" + member_token->val + "\")";
							}
							else {
								member_token->val = "((" + object_token->val + " getVariable \"__prototype\") getVariable \"" + member_token->val + "\")";
							}
							current_token = --member_token;
						}
						else if (following_token->val == "=") {
							auto val_token_end = std::next(following_token);
							if (val_token_end == end_entry_)
								continue; // @TODO: this should be a syntax error, hanging = operator.

							auto val_token_start = val_token_end;
							uint32_t block_counter = 0;
							for (val_token_end; val_token_end != end_entry_ && (val_token_end->val != ";" || block_counter > 0); ++val_token_end) {
								if (val_token_end->type == carma::type::EMPTY)
									continue;
								if (val_token_end->val == "[" || val_token_end->val == "{" || val_token_end->val == "(")
									block_counter++;
								if (val_token_end->val == "]" || val_token_end->val == "}" || val_token_end->val == ")")
									block_counter--;
							}
							process_accessors(tokens_, val_token_start, val_token_end);
							std::string val_string = build_string(tokens_, val_token_start, val_token_end);
							for (auto clear_token = val_token_start; clear_token != val_token_end; ++clear_token) {
								clear_token->type = carma::type::EMPTY;
							}
							object_token->type = carma::type::EMPTY;
							dot_token->type = carma::type::EMPTY;
							member_token->type = carma::type::EMPTY;
							following_token->type = carma::type::EMPTY;
							--val_token_end;
							val_token_end->type = carma::type::ARRAYACCESSOR;
							if (dot_token->val == ".") {
								val_token_end->val = "(" + object_token->val + " setVariable [\"" + member_token->val + "\", " + val_string + "])";
							}
							else {
								val_token_end->val = "((" + object_token->val + " getVariable \"__prototype\") setVariable [\"" + member_token->val + "\", " + val_string + "])";
							}
							current_token = val_token_end;
						}
						else if (following_token->val == "(") {
							uint32_t block_counter = 0;

							auto arg_token_end = std::next(following_token);
							if (arg_token_end == end_entry_)
								continue; // @TODO: this should be a syntax error, hanging [ operator.

							auto arg_token_start = arg_token_end;

							for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != ")" || block_counter > 0); ++arg_token_end) {
								if (arg_token_end->type == carma::type::EMPTY)
									continue;
								if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
									block_counter++;
								if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
									block_counter--;
							}


							process_accessors(tokens_, arg_token_start, arg_token_end);
							std::string arg_string = build_string(tokens_, arg_token_start, arg_token_end);
							for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
								clear_token->type = carma::type::EMPTY;
							}
							object_token->type = carma::type::EMPTY;
							dot_token->type = carma::type::EMPTY;
							member_token->type = carma::type::EMPTY;
							following_token->type = carma::type::EMPTY;
							arg_token_end->type = carma::type::METHODCALL;
							/*
							This could be made more simple probably, but eh for now.

							@TODO: In the future make these definable in SQF via some sort of registry and dynamically called/overridable.
							*/
							if (member_token->val == "__call") {
								if (dot_token->val == ".") {
									arg_token_end->val = "([" + object_token->val + ", [" + arg_string + "]] call carma2_fnc_methodInvokeContext)";
								}
								else {
									arg_token_end->val = "([(" + object_token->val + " getVariable \"__prototype\"), [" + arg_string + "]] call carma2_fnc_methodInvokeContext)";
								}
							} 
							else if (member_token->val == "__apply") {
								if (dot_token->val == ".") {
									arg_token_end->val = "([" + object_token->val + ", [" + arg_string + "]] call carma2_fnc_methodApplyContext)";
								}
								else {
									arg_token_end->val = "([(" + object_token->val + " getVariable \"__prototype\"), [" + arg_string + "]] call carma2_fnc_methodApplyContext)";
								}
							}
							else {
								if (dot_token->val == ".") {
									arg_token_end->val = "([" + object_token->val + ", \"" + member_token->val + "\", [" + arg_string + "]] call carma2_fnc_methodInvoke)";
								}
								else {
									arg_token_end->val = "([(" + object_token->val + " getVariable \"__prototype\"), \"" + member_token->val + "\", [" + arg_string + "]] call carma2_fnc_methodInvoke)";
								}
							}
							current_token = --arg_token_end;
						}
					}
					else if (std::next(current_token)->val == "[") {
						if (current_token->type != carma::type::LITERAL &&
							current_token->type != carma::type::ARRAYACCESSOR &&
							current_token->type != carma::type::MEMBERACCESSOR &&
							current_token->type != carma::type::METHODCALL)
							continue;
						if (is_reserved_word(current_token->val))
							continue;
						auto object_token = current_token;
						auto bracket_token = std::next(current_token);
						uint32_t block_counter = 0;

						auto arg_token_end = std::next(current_token, 2);
						if (arg_token_end == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging [ operator.

						auto arg_token_start = arg_token_end;

						for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != "]" || block_counter > 0); ++arg_token_end) {
							if (arg_token_end->type == carma::type::EMPTY)
								continue;
							if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
								block_counter++;
							if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
								block_counter--;
						}
						
						
						process_accessors(tokens_, arg_token_start, arg_token_end);
						std::string arg_string = build_string(tokens_, arg_token_start, arg_token_end);
						for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}
						object_token->type = carma::type::EMPTY;
						bracket_token->type = carma::type::EMPTY;
						

						auto following_token = std::next(arg_token_end);
						bool is_assignment = false;
						if (following_token != end_entry_ && following_token->val == "=") {
							is_assignment = true;
						}

						if (!is_assignment) {
							arg_token_end->type = carma::type::ARRAYACCESSOR;
							arg_token_end->val = "(" + object_token->val + " select " + arg_string + ")";
							current_token = --arg_token_end;
						}
						else {
							auto val_token_end = std::next(following_token);
							if (val_token_end == end_entry_)
								continue; // @TODO: this should be a syntax error, hanging = operator.

							auto val_token_start = val_token_end;
							block_counter = 0;
							for (val_token_end; val_token_end != end_entry_ && (val_token_end->val != ";" || block_counter > 0); ++val_token_end) {
								if (val_token_end->type == carma::type::EMPTY)
									continue;
								if (val_token_end->val == "[" || val_token_end->val == "{" || val_token_end->val == "(")
									block_counter++;
								if (val_token_end->val == "]" || val_token_end->val == "}" || val_token_end->val == ")")
									block_counter--;
							}
							process_accessors(tokens_, val_token_start, val_token_end);
							std::string val_string = build_string(tokens_, val_token_start, val_token_end);
							for (auto clear_token = val_token_start; clear_token != val_token_end; ++clear_token) {
								clear_token->type = carma::type::EMPTY;
							}
							arg_token_end->type = carma::type::EMPTY;
							following_token->type = carma::type::EMPTY;
							--val_token_end;
							val_token_end->type = carma::type::ARRAYACCESSOR;
							val_token_end->val = "(" + object_token->val + " set [" + arg_string + ", " + val_string + "])";
							current_token = val_token_end;
						}
					}
					else if (std::next(current_token)->val == "{") {
						if (current_token->type != carma::type::LITERAL && 
							current_token->type != carma::type::ARRAYACCESSOR && 
							current_token->type != carma::type::MEMBERACCESSOR &&
							current_token->type != carma::type::METHODCALL)
							continue;
						if (is_reserved_word(current_token->val))
							continue;
						auto object_token = current_token;
						auto bracket_token = std::next(current_token);
						uint32_t block_counter = 0;

						auto arg_token_end = std::next(current_token, 2);
						if (arg_token_end == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging [ operator.

						auto arg_token_start = arg_token_end;

						for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != "}" || block_counter > 0); ++arg_token_end) {
							if (arg_token_end->type == carma::type::EMPTY)
								continue;
							if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
								block_counter++;
							if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
								block_counter--;
						}


						process_accessors(tokens_, arg_token_start, arg_token_end);
						std::string arg_string = build_string(tokens_, arg_token_start, arg_token_end);
						for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}
						object_token->type = carma::type::EMPTY;
						bracket_token->type = carma::type::EMPTY;


						auto following_token = std::next(arg_token_end);
						bool is_assignment = false;
						if (following_token != end_entry_ && following_token->val == "=") {
							is_assignment = true;
						}

						if (!is_assignment) {
							arg_token_end->type = carma::type::ARRAYACCESSOR;
							arg_token_end->val = "(" + object_token->val + " getVariable " + arg_string + ")";
							current_token = --arg_token_end;
						}
						else {
							auto val_token_end = std::next(following_token);
							if (val_token_end == end_entry_)
								continue; // @TODO: this should be a syntax error, hanging = operator.

							auto val_token_start = val_token_end;
							block_counter = 0;
							for (val_token_end; val_token_end != end_entry_ && (val_token_end->val != ";" || block_counter > 0); ++val_token_end) {
								if (val_token_end->type == carma::type::EMPTY)
									continue;
								if (val_token_end->val == "[" || val_token_end->val == "{" || val_token_end->val == "(")
									block_counter++;
								if (val_token_end->val == "]" || val_token_end->val == "}" || val_token_end->val == ")")
									block_counter--;
							}
							process_accessors(tokens_, val_token_start, val_token_end);
							std::string val_string = build_string(tokens_, val_token_start, val_token_end);
							for (auto clear_token = val_token_start; clear_token != val_token_end; ++clear_token) {
								clear_token->type = carma::type::EMPTY;
							}
							arg_token_end->type = carma::type::EMPTY;
							following_token->type = carma::type::EMPTY;
							--val_token_end;
							val_token_end->type = carma::type::ARRAYACCESSOR;
							val_token_end->val = "(" + object_token->val + " setVariable [" + arg_string + ", " + val_string + "])";
							current_token = val_token_end;
						}
					}
					else if (std::next(current_token)->val == "(") {
						if (current_token->type != carma::type::LITERAL &&
							current_token->type != carma::type::ARRAYACCESSOR &&
							current_token->type != carma::type::MEMBERACCESSOR &&
							current_token->type != carma::type::METHODCALL)
							continue;
						uint32_t block_counter = 0;
						auto function_token = current_token;
						if (is_reserved_word(function_token->val))
							continue;
						auto paren_token = std::next(current_token);
						auto arg_token_end = std::next(current_token,2);
						if (arg_token_end == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging [ operator.

						auto arg_token_start = arg_token_end;

						for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != ")" || block_counter > 0); ++arg_token_end) {
							if (arg_token_end->type == carma::type::EMPTY)
								continue;
							if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
								block_counter++;
							if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
								block_counter--;
						}


						process_accessors(tokens_, arg_token_start, arg_token_end);
						std::string arg_string = build_string(tokens_, arg_token_start, arg_token_end);
						for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}
						function_token->type = carma::type::EMPTY;
						paren_token->type = carma::type::EMPTY;
						arg_token_end->type = carma::type::FUNCTIONCALL;
						arg_token_end->val = "([" + arg_string + "] call " + function_token->val + ")";
						current_token = --arg_token_end;
					}
				}
			}
		}

		void process_new_keyword(token_list &tokens_, token_entry start_entry_, token_entry end_entry_) {
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
						uint32_t block_counter = 0;
						auto arg_token_end = std::next(next_token);
						if (arg_token_end == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging ( operator.

						auto arg_token_start = arg_token_end;

						for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != ")" || block_counter > 0); ++arg_token_end) {
							if (arg_token_end->type == carma::type::EMPTY)
								continue;
							if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
								block_counter++;
							if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
								block_counter--;
						}


						process_new_keyword(tokens_, arg_token_start, arg_token_end);
						std::string arg_string = build_string(tokens_, arg_token_start, arg_token_end);
						for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}
						object_token->type = carma::type::EMPTY;
						next_token->type = carma::type::EMPTY;
						current_token->type = carma::type::EMPTY;
						arg_token_end->type = carma::type::FUNCTIONCALL;
						arg_token_end->val = "([[" + arg_string + "], " + object_token->val + ", _thisScript] call carma2_fnc_newObject)";
						current_token = --arg_token_end;
					}
				}
			}
		}

		void process_spawn_keyword(token_list &tokens_, token_entry start_entry_, token_entry end_entry_) {
			/*for (token_entry current_token = start_entry_; current_token != tokens_.end(); ++current_token) {
				if (current_token->type == carma::type::EMPTY)
					continue;
				if (current_token->val == "spawn") {
					token_entry value_token = std::next(current_token);
					if (value_token == tokens_.end())
						continue;
					uint32_t block_counter = 0;
					token_list value_tokens;
					for (value_token; value_token != tokens_.end() && (value_token->val != ";" || block_counter > 0); ++value_token) {
						if (value_token->type == carma::type::EMPTY)
							continue;
						if (value_token->val == "[" || value_token->val == "{" || value_token->val == "(")
							block_counter++;
						if (value_token->val == "]" || value_token->val == "}" || value_token->val == ")")
							block_counter--;
						if (value_token->val == "spawn") {
							process_spawn_keyword(tokens_, value_token);
						}
						value_tokens.push_back(*value_token);
						value_token->type = carma::type::EMPTY;
					}
					std::string value_string = build_string(value_tokens);
					current_token->type = carma::type::EMPTY;
					std::prev(value_token)->type = carma::type::UNKNOWN;	
					std::prev(value_token)->val = " call { [_this, " + value_string + "] call carma2_fnc_spawnWrapper; }";
				}
			}*/
		}

		void process_del_keyword(token_list &tokens_, token_entry start_entry_, token_entry end_entry_) {
			/*
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
			*/
		}

		std::string build_string(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_) {
			std::string output = "";
			for (auto current_token = start_entry_; current_token != end_entry_; ++current_token) {
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