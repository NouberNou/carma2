#include "object_creation.hpp"
#include "../compiler.hpp"

using namespace carma;

carma::rules::object_creation::object_creation()
{
}


carma::rules::object_creation::~object_creation()
{
}

void carma::rules::object_creation::newObject(carma::compiler::context& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {
    
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

        auto obj_string = carma::compiler::context(&aScope, compiler::context::type::STATEMENT, tokens_, obj_token_start, obj_token_end).compile();
        
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

        auto arg_string = carma::compiler::context(&aScope, compiler::context::type::STATEMENT, tokens_, arg_token_start, arg_token_end).compile();

	    for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
		    clear_token->type = carma::type::EMPTY;
	    }
	    obj_token_end->type = carma::type::EMPTY;
	    arg_token_start->type = carma::type::EMPTY;
	    object_token->type = carma::type::EMPTY;
        start_entry_->type = carma::type::EMPTY;
	    arg_token_end->type = carma::type::FUNCTIONCALL;
	    arg_token_end->val = "([[" + arg_string + "], " + obj_string + ", _thisScript] call carma2_fnc_newObject)";
        start_entry_ = --arg_token_end;
    }
}


void carma::rules::object_creation::handleAnonObject(carma::compiler::context& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {

}

void carma::rules::object_creation::handleDelKeyword(carma::compiler::context& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {

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

        auto obj_string = carma::compiler::context(&aScope, compiler::context::type::STATEMENT, tokens_, obj_token_start, obj_token_end).compile();

        for (auto clear_token = obj_token_start; clear_token != obj_token_end; ++clear_token) {
            clear_token->type = carma::type::EMPTY;
        }

        object_token->type = carma::type::EMPTY;
        start_entry_->type = carma::type::EMPTY;
        --obj_token_end;
        obj_token_end->type = carma::type::UNKNOWN;
        obj_token_end->val = "([" + obj_string + "] call carma2_fnc_delObject)";

        start_entry_ = obj_token_end;
    }
}