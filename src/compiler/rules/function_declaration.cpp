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

void carma::rules::function_declaration::functionKeyWord(CarmaScope& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {
    auto curr_token = start_entry_;
    auto function_params_start = curr_token;
    auto function_params_end = function_params_start;
    bool hasEncounteredStart = false;
    int block_counter = 0;

    std::string assignment = "";
    if (std::next(curr_token) != end_entry_) {
        if (std::next(curr_token)->val != "(") {
            curr_token++;
            assignment = curr_token->val + "= ";
            if (curr_token->type == carma::type::SCALAR)
                throw CarmaSyntaxErrorException("Function name may not start with a number (" + curr_token->val + ")");
        }
    }
    // Move past the condition
    for (curr_token; curr_token != end_entry_ && (!hasEncounteredStart || block_counter > 0); ++curr_token) {
        if (curr_token->type == carma::type::EMPTY)
            continue;

        if (curr_token->val == "(") {
            if (!hasEncounteredStart) {
                function_params_start = curr_token;
            }
            hasEncounteredStart = true;
            block_counter++;
        }
        if (curr_token->val == ")")
            block_counter--;
    }
    if (!hasEncounteredStart) {
        throw std::exception("Syntax error!");
    }

    function_params_end = curr_token;
    function_params_end--; // skip to after the method parameters

    // Parse condition between function_params_start and function_params_end
    std::string parameters = "";
    if (std::next(function_params_start) != function_params_end)
        parameters = CarmaScope(&aScope, compiler::context::Type::FUNCTION_PARAMS, tokens_, function_params_start, function_params_end).Compile();

    if (curr_token->val != "{")
        throw std::exception("Syntax error!");

    // collect content in the function block
    auto function_block_start = curr_token;

    hasEncounteredStart = false;
    block_counter = 0;
    for (curr_token; curr_token != end_entry_ && (!hasEncounteredStart || block_counter > 0); ++curr_token) {
        if (curr_token->type == carma::type::EMPTY)
            continue;

        if (curr_token->val == "{") {
            if (!hasEncounteredStart) {
                function_params_start = curr_token;
            }
            hasEncounteredStart = true;
            block_counter++;
        }
        if (curr_token->val == "}")
            block_counter--;
    }
    if (!hasEncounteredStart || block_counter > 0)
        throw std::exception("Syntax error");

    curr_token--; // go one back to }
    if (function_block_start == curr_token)
        throw std::exception("Syntax error, missing { or }");

    std::string block = "";
    if (std::next(function_block_start) != curr_token) {
        function_block_start++;
        // curr_token--;
        block = CarmaScope(&aScope, compiler::context::Type::FUNCTION, tokens_, function_block_start, curr_token).Compile();
    }
    // parse content between function_block_start and curr_token (block end)
    auto content = assignment + "{ scopeName \"____carma2_main_scope\";" + parameters + block + "nil;}";

    // assign empty type from beginning until end
    compiler::empty_tokens(tokens_, start_entry_, curr_token);

    // Place content in our token list
    curr_token->type = carma::type::CODEBLOCK;
    curr_token->val = content;

}

void carma::rules::function_declaration::returnKeyWord(CarmaScope& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_) {
    auto return_token = std::next(start_entry_);
    if (return_token == tokens_.end())
        return;

    if (!aScope.InScopeType(compiler::context::Type::FUNCTION))
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
        auto return_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, return_token_start, return_token_end).Compile();
        
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