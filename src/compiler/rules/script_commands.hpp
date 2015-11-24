#pragma once
#include <string>
#include <vector>
#include "../compiler.hpp"

class script_command
{
public:

    /**
    * Possible locations of argument placement relative to the script command
    * Example:
    *   LEFT COMMAND
    *   COMMAND RIGHT
    *   LEFT COMMAND RIGHT
    */
    enum class Type { NONE, LEFT, RIGHT, LEFT_RIGHT };

    script_command(std::string name, script_command::Type type);
    ~script_command();

    /**
     * Format command without any parameters
     */
    std::string format();
    
    /**
    * Format command with a single parameter
    * @param first Parameter placed on the right side of the command
    */
    std::string format(const std::string& first);

    /**
    * Format command with two parameters
    * @param first Parameter placed on the left side of the command
    * @param second Parameter placed on the right side of the command
    */
    std::string format(const std::string& first, const std::string& second);

    /**
     * Script command name.
     */
    std::string name;

    /**
     * Script command type.
     */
    script_command::Type type;
};

static class script_command_parser
{
public:
    static void add_script_command(std::string input);

    /**
    * Convert a string into a vector of parameters based on comma's and brackets
    * Example input string:
    *   value getvariable "variable", [0,43,32,"hello"]
    * Would return a vector with two elements, (value getvariable "variable") and ([0,43,32,"hello"])
    *
    * @param parameters Input string containing the parameters
    */
    static std::vector<std::string> get_parameters(const std::string& parameters);

    /**
    * Get the script command if it exists.
    * @param command Name of the command
    * @param type Type of the command
    */
    static script_command getscript_command(std::string cmd, script_command::Type type);

    /**
    * Check if the script command if it exists.
    * @param command Name of the command
    * @param type Type of the command
    */
    static bool isscript_command(std::string cmd, script_command::Type type);

    /**
    * All known script commands.
    */
    static std::vector<script_command> commands;
    
private:
    script_command_parser();
    ~script_command_parser();
};
