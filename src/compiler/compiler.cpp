#include "compiler.hpp"

#include <unordered_map>
#include <sstream>
#include <iostream>

namespace carma {
	namespace compiler {

		int return_ok_flag = 0;
		int block_depth = 0;


		void process_pass_1(token_list &tokens_, token_entry start_entry_, token_entry end_entry_) {
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

						process_pass_1(tokens_, obj_token_start, obj_token_end);
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

						process_pass_1(tokens_, arg_token_start, arg_token_end);
						std::string arg_string = build_string(tokens_, arg_token_start, arg_token_end);
						for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}
						obj_token_end->type = carma::type::EMPTY;
						arg_token_start->type = carma::type::EMPTY;
						object_token->type = carma::type::EMPTY;
						current_token->type = carma::type::EMPTY;
						arg_token_end->type = carma::type::FUNCTIONCALL;
						//arg_token_end->val = "([[" + arg_string + "], " + obj_string + ", _thisScript] call carma2_fnc_newObject)";
						if (arg_string != "") {
							arg_token_end->val = "@CARMA_NEW_OBJECT(" + obj_string + "," + arg_string + ")";
						}
						else {
							arg_token_end->val = "@CARMA_NEW_OBJECT(" + obj_string + ")";
						}
						current_token = --arg_token_end;
					}
					continue;
				}

				if (current_token->val == "function") {
					auto arg_start_token = std::next(current_token);
					if (arg_start_token == tokens_.end())
						continue;
					if (arg_start_token->val != "(")
						continue; // @TODO: throw a syntax error
					arg_start_token->type = carma::type::EMPTY;
					arg_start_token = std::next(arg_start_token);
					if (arg_start_token == tokens_.end())
						continue;

					token_entry arg_token = arg_start_token;
					typedef std::pair<std::string, std::string> arg_entry;
					std::list<arg_entry> args;

					for (arg_token; arg_token != end_entry_ && (arg_token->val != ")"); ++arg_token) {
						if (arg_token->type == carma::type::EMPTY)
							continue;
						if (arg_token->type != carma::type::LITERAL)
							continue; // @TODO: throw an error, non-literal var name in function arguments
						if(arg_token->val.substr(0,1) != "_")
							continue; // @TODO: throw an error, non-local var name in function arguments

						auto test_token = std::next(arg_token);
						if (test_token == tokens_.end())
							continue;
						if (test_token->val == "," || test_token->val == ")") {
							args.push_back(arg_entry(arg_token->val, ""));
						}
						else if (test_token->val == "=") {
							uint32_t block_counter = 0;
							token_entry val_token_start = std::next(test_token);
							if (val_token_start == tokens_.end())
								continue;
							token_entry val_token_end = val_token_start;;
							for (val_token_end; val_token_end != end_entry_ && ((val_token_end->val != "," && val_token_end->val != ")") || block_counter > 0); ++val_token_end) {
								if (val_token_end->val == "[" || val_token_end->val == "{" || val_token_end->val == "(")
									block_counter++;
								if (val_token_end->val == "]" || val_token_end->val == "}" || val_token_end->val == ")")
									block_counter--;
							}
							process_pass_1(tokens_, val_token_start, val_token_end);
							std::string val_string = build_string(tokens_, val_token_start, val_token_end);
							args.push_back(arg_entry(arg_token->val, val_string));
						}

					}
					++arg_token;
					for (auto clear_token = arg_start_token; clear_token != arg_token; ++clear_token) {
						clear_token->type = carma::type::EMPTY;
					}

					token_entry definition_start_token = arg_token;
					if (definition_start_token->val != "{")
						continue;
					definition_start_token = ++arg_token;
					if (definition_start_token == tokens_.end())
						continue;

					uint32_t block_counter = 0;
					token_entry definition_end_token = definition_start_token;
					for (definition_end_token; definition_end_token != end_entry_ && (definition_end_token->val != "}" || block_counter > 0); ++definition_end_token) {
						if (definition_end_token->val == "[" || definition_end_token->val == "{" || definition_end_token->val == "(")
							block_counter++;
						if (definition_end_token->val == "]" || definition_end_token->val == "}" || definition_end_token->val == ")")
							block_counter--;
					}
					return_ok_flag++;
					process_pass_1(tokens_, definition_start_token, definition_end_token);
					return_ok_flag--;
					std::string definition_string = build_string(tokens_, definition_start_token, definition_end_token);
					for (auto clear_token = --definition_start_token; clear_token != definition_end_token; ++clear_token) {
						clear_token->type = carma::type::EMPTY;
					}
					current_token->type = carma::type::EMPTY;
					current_token = definition_end_token;
					if (args.size() > 0) {
						std::stringstream params_string;
						for (auto param_entry = args.begin(); param_entry != args.end(); ++param_entry) {
							if (param_entry->second != "") {
								params_string << "[\"" << param_entry->first << "\", " << param_entry->second << "]";
							}
							else {
								params_string << "\"" + param_entry->first + "\"";
							}
							if (std::next(param_entry) != args.end())
								params_string << ", ";
						}

						//current_token->val = "{ scopeName \"____carma2_main_scope\"; params [" + params_string.str() + "]; " + definition_string + " }";
						current_token->val = "@CARMA_FUNCTION_DEFINITION(" + definition_string + "," + params_string.str() + ")";
					}
					else {
						//current_token->val = "{ scopeName \"____carma2_main_scope\"; " + definition_string + " }";
						current_token->val = "@CARMA_FUNCTION_DEFINITION(" + definition_string + ")";
					}
					current_token->type = carma::type::LITERAL;
					current_token--;
					continue;
				}

				if (current_token->val == "return") {
					if (return_ok_flag <= 0)
						continue; // @TODO: throw an error here because return statements are only allowed inside function() defined methods.
					auto return_token = std::next(current_token);
					if (return_token == tokens_.end())
						continue;
					if (return_token->type == carma::type::LITERAL) {
						uint32_t block_counter = 0;
						auto return_token_end = return_token;
						if (return_token_end == end_entry_)
							continue; // @TODO: this should be a syntax error, hanging ( operator.

						auto return_token_start = return_token_end;

						for (return_token_end; return_token_end != end_entry_ && (return_token_end->val != ";" || block_counter > 0); ++return_token_end) {
							if (return_token_end->type == carma::type::EMPTY)
								continue;
							if (return_token_end->val == "[" || return_token_end->val == "{" || return_token_end->val == "(")
								block_counter++;
							if (return_token_end->val == "]" || return_token_end->val == "}" || return_token_end->val == ")")
								block_counter--;
						}

						process_pass_1(tokens_, return_token_start, return_token_end);
						std::string return_string = build_string(tokens_, return_token_start, return_token_end);
						for (auto clear_token = return_token_start; clear_token != return_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}

						return_token->type = carma::type::EMPTY;
						current_token->type = carma::type::EMPTY;
						--return_token_end;
						return_token_end->type = carma::type::UNKNOWN;
						//return_token_end->val = "((" + return_string + ") breakOut \"____carma2_main_scope\")";
						return_token_end->val = "@CARMA_RETURN(" + return_string + ")";
						current_token = return_token_end;
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

						process_pass_1(tokens_, obj_token_start, obj_token_end);
						std::string obj_string = build_string(tokens_, obj_token_start, obj_token_end);
						for (auto clear_token = obj_token_start; clear_token != obj_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}

						object_token->type = carma::type::EMPTY;
						current_token->type = carma::type::EMPTY;
						--obj_token_end;
						obj_token_end->type = carma::type::UNKNOWN;
						//obj_token_end->val = "([" + obj_string + "] call carma2_fnc_delObject)";
						obj_token_end->val = "@CARMA_DEL(" + obj_string + ")";
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
								//member_token->val = "(" + object_token->val + " getVariable \"" + member_token->val + "\")";
								member_token->val = "@CARMA_DOT_OPERATOR(" + object_token->val + "," + member_token->val + ")";
							}
							else {
								//member_token->val = "((" + object_token->val + " getVariable \"__prototype\") getVariable \"" + member_token->val + "\")";
								member_token->val = "@CARMA_STATIC_OPERATOR(" + object_token->val + "," + member_token->val + ")";
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
							process_pass_1(tokens_, val_token_start, val_token_end);
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
								//val_token_end->val = "(" + object_token->val + " setVariable [\"" + member_token->val + "\", " + val_string + "])";
								val_token_end->val = "@CARMA_DOT_ASSIGNMENT(" + object_token->val + "," + member_token->val + "," + val_string + ")";
							}
							else {
								//val_token_end->val = "((" + object_token->val + " getVariable \"__prototype\") setVariable [\"" + member_token->val + "\", " + val_string + "])";
								val_token_end->val = "@CARMA_STATIC_ASSIGNMENT(" + object_token->val + "," + member_token->val + "," + val_string + ")";
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


							process_pass_1(tokens_, arg_token_start, arg_token_end);
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
							@TODO: In the future make these definable in SQF via some sort of registry and dynamically called/overridable.
							*/
							if (arg_string != "") {
								if (dot_token->val == ".") {
									//arg_token_end->val = "([" + object_token->val + ", \"" + member_token->val + "\", [" + arg_string + "]] call carma2_fnc_methodInvoke)";
									arg_token_end->val = "@CARMA_DOT_INVOKE(" + object_token->val + "," + member_token->val + "," + arg_string + ")";
								}
								else {
									//arg_token_end->val = "([(" + object_token->val + " getVariable \"__prototype\"), \"" + member_token->val + "\", [" + arg_string + "]] call carma2_fnc_methodInvoke)";
									arg_token_end->val = "@CARMA_STATIC_INVOKE(" + object_token->val + "," + member_token->val + "," + arg_string + ")";
								}
							}
							else {
								if (dot_token->val == ".") {
									//arg_token_end->val = "([" + object_token->val + ", \"" + member_token->val + "\", [" + arg_string + "]] call carma2_fnc_methodInvoke)";
									arg_token_end->val = "@CARMA_DOT_INVOKE(" + object_token->val + "," + member_token->val + ")";
								}
								else {
									//arg_token_end->val = "([(" + object_token->val + " getVariable \"__prototype\"), \"" + member_token->val + "\", [" + arg_string + "]] call carma2_fnc_methodInvoke)";
									arg_token_end->val = "@CARMA_STATIC_INVOKE(" + object_token->val + "," + member_token->val + ")";
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
						
						
						process_pass_1(tokens_, arg_token_start, arg_token_end);
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
							//arg_token_end->val = "(" + object_token->val + " select " + arg_string + ")";
							arg_token_end->val = "@CARMA_ARRAY_ACCESSOR(" + object_token->val + "," + arg_string + ")";
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
							process_pass_1(tokens_, val_token_start, val_token_end);
							std::string val_string = build_string(tokens_, val_token_start, val_token_end);
							for (auto clear_token = val_token_start; clear_token != val_token_end; ++clear_token) {
								clear_token->type = carma::type::EMPTY;
							}
							arg_token_end->type = carma::type::EMPTY;
							following_token->type = carma::type::EMPTY;
							--val_token_end;
							val_token_end->type = carma::type::ARRAYACCESSOR;
							//val_token_end->val = "(" + object_token->val + " set [" + arg_string + ", " + val_string + "])";
							val_token_end->val = "@CARMA_ARRAY_ASSIGNMENT(" + object_token->val + "," + val_string + "," + arg_string + ")";
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


						process_pass_1(tokens_, arg_token_start, arg_token_end);
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
							//arg_token_end->val = "(" + object_token->val + " getVariable " + arg_string + ")";
							arg_token_end->val = "@CARMA_HASHMAP_ACCESSOR(" + object_token->val + "," + arg_string + ")";
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
							process_pass_1(tokens_, val_token_start, val_token_end);
							std::string val_string = build_string(tokens_, val_token_start, val_token_end);
							for (auto clear_token = val_token_start; clear_token != val_token_end; ++clear_token) {
								clear_token->type = carma::type::EMPTY;
							}
							arg_token_end->type = carma::type::EMPTY;
							following_token->type = carma::type::EMPTY;
							--val_token_end;
							val_token_end->type = carma::type::ARRAYACCESSOR;
							//val_token_end->val = "(" + object_token->val + " setVariable [" + arg_string + ", " + val_string + "])";
							val_token_end->val = "@CARMA_HASHMAP_ASSIGNMENT(" + object_token->val + "," + arg_string + "," + val_string + ")";
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


						process_pass_1(tokens_, arg_token_start, arg_token_end);
						std::string arg_string = build_string(tokens_, arg_token_start, arg_token_end);
						for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
							clear_token->type = carma::type::EMPTY;
						}
						function_token->type = carma::type::EMPTY;
						paren_token->type = carma::type::EMPTY;
						arg_token_end->type = carma::type::FUNCTIONCALL;
						//arg_token_end->val = "([" + arg_string + "] call " + function_token->val + ")";
						if (arg_string != "") {
							arg_token_end->val = "@CARMA_FUNCTION_CALL(" + function_token->val + "," + arg_string + ")";
						}
						else {
							arg_token_end->val = "@CARMA_FUNCTION_CALL(" + function_token->val + ")";
						}
						current_token = --arg_token_end;
					}
				}
			}
		}

		void process_pass_2(token_list &tokens_, token_entry start_entry_, token_entry end_entry_, block &parent_) {
			for (token_entry current_token = start_entry_; current_token != end_entry_; ++current_token) {
				if (current_token->val.substr(0, 6) == "@CARMA") {
					block child_block;
					child_block.type = current_token->val.substr(7);
				}
			}
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

		std::string build_string_pretty(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_) {
			token_list tokens = tokenize(build_string(tokens_, start_entry_, end_entry_));
			std::string output = "";
			int block_indent = 0;
			for (auto current_token = tokens.begin(); current_token != tokens.end(); ++current_token) {
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