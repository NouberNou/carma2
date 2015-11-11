#include <stdio.h>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>

#include "compiler\compiler.hpp"

namespace carma {
	void tokenize_simple_assigments(token_list &tokens_) {
		for (auto current_token = tokens_.begin(); current_token != tokens_.end(); ++current_token) {
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
}


int main(int argc, char **argv) {
	std::string input_str = "_fuck.obj.poop = { _test = _fuck.obj*_fuck.job(); player setDamage _test; };";
	carma::token_list tokens = carma::tokenize(input_str);
	carma::tokenize_simple_assigments(tokens);
	std::string output = "";
	
	for (auto current_token = tokens.begin(); current_token != tokens.end(); ++current_token) {
		if (current_token->type == carma::type::EMPTY)
			continue;
		output += current_token->val;
		if (std::next(current_token) != tokens.end() && current_token->type == carma::type::LITERAL && std::next(current_token)->type == carma::type::LITERAL) {
			output += " ";
		}
		else if (current_token->type == carma::type::ENDOFSTATEMENT) {
			output += " ";
		}
	};
	return 0;
}