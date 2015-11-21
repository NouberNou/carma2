#include "script_commands.hpp"
#include <regex>

std::vector<script_command> script_command_parser::commands = std::vector<script_command>();

script_command_parser::script_command_parser()
{    
}

script_command_parser::~script_command_parser()
{
}


std::vector<std::string> script_command_parser::get_parameters(const std::string& source_) {
    std::vector<std::string> parameters = std::vector<std::string>();

    std::string token_str;

    int block_counter = 0;

    for (std::string::size_type stream_pos = 0; stream_pos < source_.size(); ++stream_pos) {
        std::string cur_char = source_.substr(stream_pos, 1);

        if (block_counter == 0 && cur_char == ",") {
            parameters.push_back(token_str);
            token_str = "";
            continue;
        }
        if (cur_char == "[" || cur_char == "{" || cur_char == "(")
            block_counter++;
        if (cur_char == "]" || cur_char == "}" || cur_char == ")")
            block_counter--;

        token_str += cur_char;   
    }
    if (token_str != "") {
        parameters.push_back(token_str);
    }

    return parameters;
}

void script_command_parser::add_script_command(std::string input) {
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);

    std::regex check_binary(".*b:.+?\\s(.+?)\\s.+");
    std::smatch matches;
    if (std::regex_match(input, matches, check_binary)) {
        if (isscript_command(matches[1], script_command::Type::LEFT_RIGHT)) { return; }
        script_command command = script_command(matches[1], script_command::Type::LEFT_RIGHT);
        commands.push_back(command);
    }
    else {
        std::regex check_unary(".*u:(.+)?\\s.+");
        if (std::regex_match(input, matches, check_unary)) {
            if (isscript_command(matches[1], script_command::Type::RIGHT)) { return; }
            script_command command = script_command(matches[1], script_command::Type::RIGHT);
            commands.push_back(command);
        } 
        else {
            std::regex check_null(".*n:(.+)");
            if (std::regex_match(input, matches, check_null)) {
                if (isscript_command(matches[1], script_command::Type::NONE)) { return; }
                script_command command = script_command(matches[1], script_command::Type::NONE);
                commands.push_back(command);
            }
        }
    }
}

script_command script_command_parser::getscript_command(std::string cmd, script_command::Type type) {
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    for each (auto command in script_command_parser::commands)
    {
        if (command.name == cmd && command.type == type)
            return command;
    };
    script_command *comm = NULL;
    return *comm;
};

bool script_command_parser::isscript_command(std::string cmd, script_command::Type type) {
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    for each (auto command in script_command_parser::commands)
    {
        if (command.name == cmd && command.type == type)
            return true;
    };
    return false;
};

script_command::script_command(std::string name, script_command::Type type): name(name), type(type)
{

};

script_command::~script_command() {
};


std::string script_command::format() {
    if (type == script_command::Type::NONE) {
        return name;
    }
    throw carma::compiler::exception::syntax_error("Formatting script command (" + name + ") does not exists with no parameters");
}

std::string script_command::format(const std::string& first) {
    if (type == script_command::Type::LEFT) {
        return "(" + first + " " + name + ")";
    } else if (type == script_command::Type::RIGHT) {
        return "(" + name + " " + first + ")";
    }
    throw carma::compiler::exception::syntax_error("Formatting script command (" + name + ") does not exists with one parameter");
};

std::string script_command::format(const std::string& first, const std::string& second) {
    if (type == script_command::Type::LEFT) {
        return "(" + first + " " + name + ")";
    }
    else if (type == script_command::Type::RIGHT) {
        return "(" + name + " " + first + ")";
    }
    else if (type == script_command::Type::LEFT_RIGHT) {
        return "(" + first + " " + name + " " + second + ")";
    }
    throw carma::compiler::exception::syntax_error("Formatting script command (" + name + ") does not exists with parameters");
};
