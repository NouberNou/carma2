#include "object_creation.hpp"
#include "../compiler.hpp"
#include <sstream>

using namespace carma;

carma::rules::object_creation::object_creation()
{
}


carma::rules::object_creation::~object_creation()
{
}

void carma::rules::object_creation::new_object(carma::compiler::context& a_scope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {
    
    auto object_token = std::next(start_entry_);
    if (object_token == tokens_.end())
	    return; // nothing else to do

    if (object_token->type == carma::type::LITERAL) {
	    uint32_t block_counter = 0;
	    auto obj_token_end = object_token;
	    if (obj_token_end == end_entry_)
		    throw std::exception("Syntax error!"); // @TODO: this should be a syntax error, hanging ( operator.

	    auto obj_token_start = obj_token_end;

	    for (obj_token_end; obj_token_end != end_entry_ && (obj_token_end->val != "(" || block_counter > 0); ++obj_token_end) {
		    if (obj_token_end->type == carma::type::EMPTY)
			    continue;
		    if (obj_token_end->val == "[" || obj_token_end->val == "{" || obj_token_end->val == "(")
			    block_counter++;
		    if (obj_token_end->val == "]" || obj_token_end->val == "}" || obj_token_end->val == ")")
			    block_counter--;
	    }

        auto obj_string = carma::compiler::context(&a_scope, compiler::context::type::STATEMENT, tokens_, obj_token_start, obj_token_end).compile();
        
	    for (auto clear_token = obj_token_start; clear_token != obj_token_end; ++clear_token) {
		    clear_token->type = carma::type::EMPTY;
	    }

	    auto arg_token_end = std::next(obj_token_end);
	    if (arg_token_end == end_entry_)
		    throw std::exception("Syntax Error!"); // @TODO: this should be a syntax error, hanging ( operator.

	    auto arg_token_start = arg_token_end;

	    for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != ")" || block_counter > 0); ++arg_token_end) {
		    if (arg_token_end->type == carma::type::EMPTY)
			    continue;
		    if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
			    block_counter++;
		    if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
			    block_counter--;
	    }

        auto arg_string = carma::compiler::context(&a_scope, compiler::context::type::STATEMENT, tokens_, arg_token_start, arg_token_end).compile();

	    for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
		    clear_token->type = carma::type::EMPTY;
	    }
	    obj_token_end->type = carma::type::EMPTY;
	    arg_token_start->type = carma::type::EMPTY;
	    object_token->type = carma::type::EMPTY;
        start_entry_->type = carma::type::EMPTY;
	    arg_token_end->type = carma::type::FUNCTIONCALL;
	    arg_token_end->val = "([[" + arg_string + "], " + obj_string + ", _this_script] call carma2_fnc_new_object)";
        start_entry_ = --arg_token_end;
    }
}


void carma::rules::object_creation::handle_anon_object(carma::compiler::context& a_scope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {
    if (start_entry_->val == "{" && start_entry_ != tokens_.begin() && (
        std::prev(start_entry_)->val == "&&" ||
        std::prev(start_entry_)->val == "||" ||
        std::prev(start_entry_)->val == "and" ||
        std::prev(start_entry_)->val == "or"))
        return; // check for lazy eval expression

    auto current_token = std::next(start_entry_);
    if (current_token == tokens_.end())
        return;

    if (current_token->val == "}") { // Create an empty object
        compiler::empty_tokens(tokens_, start_entry_, std::next(current_token));
        current_token->type = carma::type::METHODCALL;
        current_token->val = "([[], carma2_AnonObject, _thisScript] call carma2_fnc_newObject)";
        return;
    }

    std::vector<std::string> newObject_arguments;
    for (current_token; current_token != end_entry_ && current_token->val != "}"; current_token++) {

        if (std::next(current_token) == end_entry_ || std::next(current_token, 2) == end_entry_)
            break;
        if (current_token->val == ")" || current_token->val == "]" || current_token->val == "}")
            break;

        if (std::next(current_token)->val == ":") {
            // The name of our value
            auto name_token = current_token;

            // Find the value
            auto value_start_token = std::next(current_token, 2);
            auto value_end_token = std::next(current_token, 2);
            int block_counter = 0;

            // loop until either end of this value (,) or end of the object ( } ). This will collect our value contents
            for (current_token; current_token != end_entry_ && ((current_token->val != "}" && current_token->val != ",") || block_counter > 0); current_token++) {
                if (current_token->type == carma::type::EMPTY)
                    continue;
                if (current_token->val == "{" || current_token->val == "[" || current_token->val == "(") {
                    block_counter++;
                }
                if (current_token->val == "}" || current_token->val == "]" || current_token->val == ")") {
                    block_counter--;
                }
            }
            // Recursively compile our value
            std::string value = carma::compiler::context(&a_scope, compiler::context::type::STATEMENT, tokens_, value_start_token, current_token).compile();
            
            // Create our key value pair for object creation
            newObject_arguments.push_back("[\"" + name_token->val + "\", " + value + "]");

            // Move past our value end.
            if (std::next(current_token)->val == ",")
                current_token++;
        }
        else { // This means there was a space between our key and value or an invalid operator was used
            if (std::next(current_token) != end_entry_)
                throw carma::compiler::exception::syntax_error("Invalid syntax for object creation. Expected \":\". Found: \"" + std::next(current_token)->val + "\"" + " with name: " + current_token->val);
            else
                throw carma::compiler::exception::syntax_error("Invalid syntax for object creation. Possible missing \"}\"?");
        }
    }
    auto end = current_token;
    if (end->val == "}")
        end++;

    compiler::empty_tokens(tokens_, start_entry_, end);

    std::stringstream stream;
    stream << "[";
    for (int i = 0; i < newObject_arguments.size(); ++i) {
        stream << newObject_arguments.at(i);
        if (i + 1 < newObject_arguments.size()) {
            stream << ",";
        }
    }
    stream << "]";
    // start_entry_ = --current_token;
    start_entry_->type = carma::type::METHODCALL;
    start_entry_->val = "([" + stream.str() + ", carma2_AnonObject, _thisScript] call carma2_fnc_newObject)";
}

void carma::rules::object_creation::handle_del_keyword(carma::compiler::context& a_scope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {

    auto object_token = std::next(start_entry_);
    if (object_token == tokens_.end())
        return;

    if (object_token->type == carma::type::LITERAL) {
        uint32_t block_counter = 0;
        auto obj_token_end = object_token;
        if (obj_token_end == end_entry_)
            throw std::exception("Syntax error!"); // @TODO: this should be a syntax error, hanging ( operator.

        auto obj_token_start = obj_token_end;

        for (obj_token_end; obj_token_end != end_entry_ && (obj_token_end->val != ";" || block_counter > 0); ++obj_token_end) {
            if (obj_token_end->type == carma::type::EMPTY)
                continue;
            if (obj_token_end->val == "[" || obj_token_end->val == "{" || obj_token_end->val == "(")
                block_counter++;
            if (obj_token_end->val == "]" || obj_token_end->val == "}" || obj_token_end->val == ")")
                block_counter--;
        }

        auto obj_string = carma::compiler::context(&a_scope, compiler::context::type::STATEMENT, tokens_, obj_token_start, obj_token_end).compile();

        for (auto clear_token = obj_token_start; clear_token != obj_token_end; ++clear_token) {
            clear_token->type = carma::type::EMPTY;
        }

        object_token->type = carma::type::EMPTY;
        start_entry_->type = carma::type::EMPTY;
        --obj_token_end;
        obj_token_end->type = carma::type::UNKNOWN;
        obj_token_end->val = "([" + obj_string + "] call carma2_fnc_del_object)";

        start_entry_ = obj_token_end;
    }
}