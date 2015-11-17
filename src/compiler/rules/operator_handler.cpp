#include "operator_handler.hpp"
#include "script_commands.hpp"
#include <iostream>

carma::rules::operator_handler::operator_handler()
{
}

carma::rules::operator_handler::~operator_handler()
{
}

void carma::rules::operator_handler::handleDotOperator(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_) {
    auto object_token = start_entry_;
    auto dot_token = std::next(start_entry_);
    auto member_token = std::next(start_entry_, 2);
    if (member_token == end_entry_)
        return; // @TODO: this should be a syntax error, hanging . operator.
    auto following_token = std::next(start_entry_, 3);
    if ((following_token == end_entry_ || following_token->val != "(") && following_token->val != "=") {
        object_token->type = carma::type::EMPTY;
        dot_token->type = carma::type::EMPTY;
        member_token->type = carma::type::MEMBERACCESSOR;
        if (dot_token->val == ".") {
            member_token->val = "(" + object_token->val + " getVariable \"" + member_token->val + "\")";
        }
        else {
            member_token->val = "((" + object_token->val + " getVariable \"__prototype\") getVariable \"" + member_token->val + "\")";
        }
        start_entry_ = --member_token;
    }
    else if (following_token->val == "=") {
        auto val_token_end = std::next(following_token);
        if (val_token_end == end_entry_)
            return; // @TODO: this should be a syntax error, hanging = operator.

        auto val_token_start = val_token_end;
        uint32_t block_counter = 0;
        for (val_token_end; val_token_end != end_entry_ && (val_token_end->val != ";" || block_counter > 0); ++val_token_end) {
            if (val_token_end->type == carma::type::EMPTY)
                return;
            if (val_token_end->val == "[" || val_token_end->val == "{" || val_token_end->val == "(")
                block_counter++;
            if (val_token_end->val == "]" || val_token_end->val == "}" || val_token_end->val == ")")
                block_counter--;
        }

        auto val_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, val_token_start, val_token_end).Compile();
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
        start_entry_ = val_token_end;
    }
    else if (following_token->val == "(") {
        uint32_t block_counter = 0;

        auto arg_token_end = std::next(following_token);
        if (arg_token_end == end_entry_)
            return; // @TODO: this should be a syntax error, hanging [ operator.

        auto arg_token_start = arg_token_end;

        for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != ")" || block_counter > 0); ++arg_token_end) {
            if (arg_token_end->type == carma::type::EMPTY)
                return;
            if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
                block_counter++;
            if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
                block_counter--;
        }

        auto val_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, arg_token_start, arg_token_end).Compile();
        std::string arg_string = carma::compiler::build_string(tokens_, arg_token_start, arg_token_end);
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
        start_entry_ = --arg_token_end;
    }
}

void carma::rules::operator_handler::handleMethodCallOperator(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_) {

    if (start_entry_->type != carma::type::LITERAL &&
        start_entry_->type != carma::type::ARRAYACCESSOR &&
        start_entry_->type != carma::type::MEMBERACCESSOR &&
        start_entry_->type != carma::type::METHODCALL && 
        start_entry_->type != carma::type::CODEBLOCK)
        return;
    uint32_t block_counter = 0;
    auto function_token = start_entry_;

    if (is_reserved_word(function_token->val)) { // handle script commands method invoke

        std::string commandName = function_token->val;

        // If the command name is a control structure key word, skip
        if (commandName == "if"
            || commandName == "while"
            || commandName == "then"
            || commandName == "else"
            || commandName == "do"
            || commandName == "exitwith"
            || commandName == "switch"
            || commandName == "do"
            || commandName == "from"
            || commandName == "to"
            || commandName == "case"
            || commandName == "default"
            ) return;

        auto paren_token = std::next(start_entry_);
        auto arg_token_end = std::next(start_entry_, 2);

        if (arg_token_end == end_entry_)
            throw CarmaSyntaxErrorException("hanging ( operator"); 

        // Check if we use our command as a method
        if (paren_token->val == "(") { // we got a method invoke
            if (arg_token_end->val == ")") { // No parameters given
                if (script_commandParser::isscript_command(commandName, script_command::Type::NONE)) {
                    // If this is a valid script command, we can just strip away our () tokens here
                    std::string outputCommand = script_commandParser::getscript_command(commandName, script_command::Type::NONE).format();
                    arg_token_end->val = outputCommand;
                    arg_token_end->type = carma::type::METHODCALL;
                    paren_token->type = carma::type::EMPTY;
                    start_entry_->type = carma::type::EMPTY;
                }
                else {
                    throw CarmaSyntaxErrorException("Script command (" + commandName + ") with no parameters does not exist");
                }
            }
            else { // Method invoke with parameters
                auto arg_token_start = arg_token_end;

                // Collect all parameters within ( .. ).
                for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != ")" || block_counter > 0); ++arg_token_end) {
                    if (arg_token_end->type == carma::type::EMPTY)
                        return;
                    if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
                        block_counter++;
                    if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
                        block_counter--;
                }
                // Recursively process the parameters inside the ( .. )
                auto val_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, arg_token_start, arg_token_end).Compile();

                // Convert our parameters into a vector
                auto parameters = script_commandParser::getParameters(val_string);

                // Clear the parameters from our output by emptying the tokens
                for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
                    clear_token->type = carma::type::EMPTY;
                }

                if (parameters.size() == 1) { // If there is one parameter, try to use our command as an unary argument
                    if (script_commandParser::isscript_command(commandName, script_command::Type::RIGHT)) {
                        std::string outputCommand = script_commandParser::getscript_command(commandName, script_command::Type::RIGHT).format(parameters[0]);
                        paren_token->type = carma::type::EMPTY;
                        start_entry_->type = carma::type::EMPTY;
                        arg_token_end->type = carma::type::METHODCALL;
                        arg_token_end->val = outputCommand;
                        start_entry_ = --arg_token_end;
                    }
                    else {
                        throw CarmaSyntaxErrorException("Script command (" + commandName + ") with one parameter does not exist");
                    }
                }
                else if (parameters.size() >= 2) { // If there is one parameter, try to use our command as a binary argument
                    if (script_commandParser::isscript_command(commandName, script_command::Type::LEFT_RIGHT)) {
                        std::string outputCommand = script_commandParser::getscript_command(commandName, script_command::Type::LEFT_RIGHT).format(parameters[0], parameters[1]);
                        paren_token->type = carma::type::EMPTY;
                        start_entry_->type = carma::type::EMPTY;
                        arg_token_end->type = carma::type::METHODCALL;
                        arg_token_end->val = outputCommand;
                        start_entry_ = --arg_token_end;
                    }
                    else {
                        // TODO syntax error
                        throw CarmaSyntaxErrorException("Script command (" + commandName + ") with two parameters does not exist");
                    }
                }
                else { // Single parameter command - no parameters found. Just output the command name and we are done
                    paren_token->type = carma::type::EMPTY;
                    start_entry_->type = carma::type::EMPTY;
                    arg_token_end->type = carma::type::METHODCALL;
                    arg_token_end->val = commandName;
                    start_entry_ = --arg_token_end;
                }
            }
        }
        return;
    }
    auto paren_token = std::next(start_entry_);
    auto arg_token_end = std::next(start_entry_, 2);
    if (arg_token_end == end_entry_)
        throw CarmaSyntaxErrorException("Hanging [ operator"); // @TODO: this should be a syntax error, hanging [ operator.

    auto arg_token_start = arg_token_end;

    for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != ")" || block_counter > 0); ++arg_token_end) {
        if (arg_token_end->type == carma::type::EMPTY)
            return;
        if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
            block_counter++;
        if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
            block_counter--;
    }

    auto arg_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, arg_token_start, arg_token_end).Compile();
    for (auto clear_token = arg_token_start; clear_token != arg_token_end; ++clear_token) {
        clear_token->type = carma::type::EMPTY;
    }
    function_token->type = carma::type::EMPTY;
    paren_token->type = carma::type::EMPTY;
    arg_token_end->type = carma::type::FUNCTIONCALL;
    arg_token_end->val = "([" + arg_string + "] call " + function_token->val + ")";
    start_entry_ = --arg_token_end;
}

void carma::rules::operator_handler::handleArrayAccessOperator(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_) {
    if (start_entry_->type != carma::type::LITERAL &&
        start_entry_->type != carma::type::ARRAYACCESSOR &&
        start_entry_->type != carma::type::MEMBERACCESSOR &&
        start_entry_->type != carma::type::METHODCALL)
        return;
    if (is_reserved_word(start_entry_->val))
    {
        std::string commandName = start_entry_->val;
        // If the command name is a control structure key word, skip
        if (commandName == "if"
            || commandName == "while"
            || commandName == "then"
            || commandName == "else"
            || commandName == "do"
            || commandName == "exitwith"
            || commandName == "switch"
            || commandName == "do"
            || commandName == "from"
            || commandName == "to"
            || commandName == "case"
            || commandName == "default"
            ) return;

        // Support Arma3 script commands with array accessors -> May need to add support for type filtering on return type value instead
        if (!script_commandParser::isscript_command(commandName, script_command::Type::NONE)) {
            return;
        }
    }
    auto object_token = start_entry_;
    auto bracket_token = std::next(start_entry_);
    uint32_t block_counter = 0;

    auto arg_token_end = std::next(start_entry_, 2);
    if (arg_token_end == end_entry_)
        throw CarmaSyntaxErrorException("Hanging [ operator"); // @TODO: this should be a syntax error, hanging [ operator.

    auto arg_token_start = arg_token_end;

    for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != "]" || block_counter > 0); ++arg_token_end) {
        if (arg_token_end->type == carma::type::EMPTY)
            return;
        if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
            block_counter++;
        if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
            block_counter--;
    }

    auto arg_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, arg_token_start, arg_token_end).Compile();

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
        start_entry_ = --arg_token_end;
    }
    else {
        auto val_token_end = std::next(following_token);
        if (val_token_end == end_entry_)
            throw CarmaSyntaxErrorException("Hanging = operator"); // @TODO: this should be a syntax error, hanging = operator.

        auto val_token_start = val_token_end;
        block_counter = 0;
        for (val_token_end; val_token_end != end_entry_ && (val_token_end->val != ";" || block_counter > 0); ++val_token_end) {
            if (val_token_end->type == carma::type::EMPTY)
                return;
            if (val_token_end->val == "[" || val_token_end->val == "{" || val_token_end->val == "(")
                block_counter++;
            if (val_token_end->val == "]" || val_token_end->val == "}" || val_token_end->val == ")")
                block_counter--;
        }

        auto val_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, val_token_start, val_token_end).Compile();
        for (auto clear_token = val_token_start; clear_token != val_token_end; ++clear_token) {
            clear_token->type = carma::type::EMPTY;
        }
        arg_token_end->type = carma::type::EMPTY;
        following_token->type = carma::type::EMPTY;
        --val_token_end;
        val_token_end->type = carma::type::ARRAYACCESSOR;
        val_token_end->val = "(" + object_token->val + " set [" + arg_string + ", " + val_string + "])";
        start_entry_ = val_token_end;
    }
}

void carma::rules::operator_handler::handleMemberAccessOperator(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_) {
    if (start_entry_->type != carma::type::LITERAL &&
        start_entry_->type != carma::type::ARRAYACCESSOR &&
        start_entry_->type != carma::type::MEMBERACCESSOR &&
        start_entry_->type != carma::type::METHODCALL)
        return;
    if (is_reserved_word(start_entry_->val))
        return;
    auto object_token = start_entry_;
    auto bracket_token = std::next(start_entry_);
    uint32_t block_counter = 0;

    auto arg_token_end = std::next(start_entry_, 2);
    if (arg_token_end == end_entry_)
        throw CarmaSyntaxErrorException("Hanging [ operator"); // @TODO: this should be a syntax error, hanging [ operator.

    auto arg_token_start = arg_token_end;

    for (arg_token_end; arg_token_end != end_entry_ && (arg_token_end->val != "}" || block_counter > 0); ++arg_token_end) {
        if (arg_token_end->type == carma::type::EMPTY)
            return;
        if (arg_token_end->val == "[" || arg_token_end->val == "{" || arg_token_end->val == "(")
            block_counter++;
        if (arg_token_end->val == "]" || arg_token_end->val == "}" || arg_token_end->val == ")")
            block_counter--;
    }

    auto arg_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, arg_token_start, arg_token_end).Compile();
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
        start_entry_ = --arg_token_end;
    }
    else {
        auto val_token_end = std::next(following_token);
        if (val_token_end == end_entry_)
            return; // @TODO: this should be a syntax error, hanging = operator.

        auto val_token_start = val_token_end;
        block_counter = 0;
        for (val_token_end; val_token_end != end_entry_ && (val_token_end->val != ";" || block_counter > 0); ++val_token_end) {
            if (val_token_end->type == carma::type::EMPTY)
                return;
            if (val_token_end->val == "[" || val_token_end->val == "{" || val_token_end->val == "(")
                block_counter++;
            if (val_token_end->val == "]" || val_token_end->val == "}" || val_token_end->val == ")")
                block_counter--;
        }

        auto val_string = CarmaScope(&aScope, compiler::context::Type::STATEMENT, tokens_, val_token_start, val_token_end).Compile();
        for (auto clear_token = val_token_start; clear_token != val_token_end; ++clear_token) {
            clear_token->type = carma::type::EMPTY;
        }
        arg_token_end->type = carma::type::EMPTY;
        following_token->type = carma::type::EMPTY;
        --val_token_end;
        val_token_end->type = carma::type::ARRAYACCESSOR;
        val_token_end->val = "(" + object_token->val + " setVariable [" + arg_string + ", " + val_string + "])";
        start_entry_ = val_token_end;
    }
}
