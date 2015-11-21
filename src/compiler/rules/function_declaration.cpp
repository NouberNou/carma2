#include "function_declaration.hpp"
#include "../compiler.hpp"

#include <sstream>

using namespace carma;

carma::rules::function_declaration::function_declaration()
{
}

carma::rules::function_declaration::~function_declaration()
{
}

void carma::rules::function_declaration::function_keyword(carma::compiler::context& a_scope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {
    auto curr_token = start_entry_;
    auto function_params_start = curr_token;
    auto function_params_end = function_params_start;
    bool has_encountered_start = false;
    int block_counter = 0;

    std::string assignment = "";
    if (std::next(curr_token) != end_entry_) {
        if (std::next(curr_token)->val != "(") {
            curr_token++;
            assignment = curr_token->val + "= ";
            if (curr_token->type == carma::type::SCALAR)
                throw carma::compiler::exception::syntax_error("Function name may not start with a number (" + curr_token->val + ")");
        }
    }
    // Move past the condition
    for (curr_token; curr_token != end_entry_ && (!has_encountered_start || block_counter > 0); ++curr_token) {
        if (curr_token->type == carma::type::EMPTY)
            continue;

        if (curr_token->val == "(") {
            if (!has_encountered_start) {
                function_params_start = curr_token;
            }
            has_encountered_start = true;
            block_counter++;
        }
        if (curr_token->val == ")")
            block_counter--;
    }
    if (!has_encountered_start) {
        throw std::exception("Syntax error!");
    }

    function_params_end = curr_token;
    function_params_end--; // skip to after the method parameters

    // Parse condition between function_params_start and function_params_end
    std::string parameters = "";
    if (std::next(function_params_start) != function_params_end)
        parameters = carma::compiler::context(&a_scope, compiler::context::type::FUNCTION_PARAMS, tokens_, function_params_start, function_params_end).compile();

    if (curr_token->val != "{")
        throw std::exception("Syntax error!");

    // collect content in the function block
    auto function_block_start = curr_token;

    has_encountered_start = false;
    block_counter = 0;
    for (curr_token; curr_token != end_entry_ && (!has_encountered_start || block_counter > 0); ++curr_token) {
        if (curr_token->type == carma::type::EMPTY)
            continue;

        if (curr_token->val == "{") {
            if (!has_encountered_start) {
                function_params_start = curr_token;
            }
            has_encountered_start = true;
            block_counter++;
        }
        if (curr_token->val == "}")
            block_counter--;
    }
    if (!has_encountered_start || block_counter > 0)
        throw std::exception("Syntax error");

    curr_token--; // go one back to }
    if (function_block_start == curr_token)
        throw std::exception("Syntax error, missing { or }");

    std::string block = "";
    if (std::next(function_block_start) != curr_token) {
        function_block_start++;
        // curr_token--;
        block = carma::compiler::context(&a_scope, compiler::context::type::FUNCTION, tokens_, function_block_start, curr_token).compile();
    }
    // parse content between function_block_start and curr_token (block end)
    auto content = assignment + "{ scopeName \"____carma2_main_scope\";" + parameters + block + "nil;}";

    // assign empty type from beginning until end
    compiler::empty_tokens(tokens_, start_entry_, curr_token);

    // Place content in our token list
    curr_token->type = carma::type::CODEBLOCK;
    curr_token->val = content;

}

void carma::rules::function_declaration::return_keyword(carma::compiler::context& a_scope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {
    auto return_token = std::next(start_entry_);
    if (return_token == tokens_.end())
        return;

    if (!a_scope.in_scope(compiler::context::type::FUNCTION))
        throw std::exception("Return statement must be in a function context");

    if (return_token->type == carma::type::LITERAL) {
        uint32_t block_counter = 0;
        auto return_token_end = return_token;
        if (return_token_end == end_entry_)
            throw std::exception("Syntax error!"); // @TODO: this should be a syntax error, hanging ( operator.

        auto return_token_start = return_token_end;

        // move until the end of the statement
        for (return_token_end; return_token_end != end_entry_ && (return_token_end->val != ";" || block_counter > 0); ++return_token_end) {
            if (return_token_end->type == carma::type::EMPTY)
                continue;
            if (return_token_end->val == "[" || return_token_end->val == "{" || return_token_end->val == "(")
                block_counter++;
            if (return_token_end->val == "]" || return_token_end->val == "}" || return_token_end->val == ")")
                block_counter--;
        }

        // compile the content of the statement
        auto return_string = carma::compiler::context(&a_scope, compiler::context::type::STATEMENT, tokens_, return_token_start, return_token_end).compile();
        
        // clear tokens we already processed
        compiler::empty_tokens(tokens_, return_token_start, return_token_end);      

        return_token->type = carma::type::EMPTY;
        start_entry_->type = carma::type::EMPTY;
        --return_token_end;

        // Set up our return string
        return_token_end->type = carma::type::UNKNOWN;
        return_token_end->val = "((" + return_string + ") breakOut \"____carma2_main_scope\")";
        start_entry_ = return_token_end;
    }
}