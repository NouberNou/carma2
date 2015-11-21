#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>
#include "tokenizer.hpp"

using namespace carma::tokenizer;

namespace carma {
	namespace compiler {

        /**
        * Convert tokens into a string
        * @param tokens The tokens
        * @param start_entry_ Starting point from which the string will be compiled
        * @param end_entry_ End point till which everything will be converted into a string
        * @return A string from start_entry_ until the token before end_entry_
        */
		std::string build_string(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);

        /**
        * Convert tokens into a string
        * @param tokens The tokens
        * @param start_entry_ Starting point from which the string will be compiled
        * @param end_entry_ End point till which everything will be converted into a string
        * @return A string from start_entry_ until the token before end_entry_
        */
		std::string build_string_pretty(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);

        /**
        * Convert tokens into a mimimalized version, without empty tokens in between
        * @param tokens The tokens
        * @param start_entry_ Starting point
        * @param end_entry_ End point
        * @return A mimized version of the token list
        */
		token_list minimize(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);

        /**
        * Convert tokens to empty tokens
        * @param tokens The tokens
        * @param start_entry_ Starting point
        * @param end_entry_ End point
        */
        void empty_tokens(token_list& tokens_, token_entry& start_entry_, token_entry& end_entry_);

        class context {
        public:
            /**
            * Possible context types
            */
            enum class Type { FILE, SCOPE, OBJECT, FUNCTION, FUNCTION_PARAMS, CONTROL_STRUCTURE, STATEMENT };

            /**
            * Parent context. Null_ptr if this is a root.
            * This is used to check certain keywords such as return, that are only allowed within certain scopes
            */
            context* parent;

            /**
            * All scopes that this context contains
            */
            std::vector<context*> scopes;

            /**
            * The context type
            */
            Type type;

            std::vector<std::string> variables;
            std::vector<context*> functions;
            
            /**
            * Create a new parent context
            * @param aType the type of context
            * @param tokens list of tokens that make up the context
            */
            context(Type aType, token_list& tokens);

            /**
            * Create a new sub context
            * @param aType the type of context
            * @param tokens list of tokens that make up the context
            */
            context(context* parentContext, Type aType, token_list& tokens);

            /**
            * Create a new parent context
            * @param aType the type of context
            * @param tokens list of tokens that make up the context
            * @param start_entry_ Starting point in the tokens list
            * @param end_entry|_ End point in the tokens list
            */
            context(Type aType, token_list& tokens, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Create a new sub context
            * @param aType the type of context
            * @param tokens list of tokens that make up the context
            * @param start_entry_ Starting point in the tokens list
            * @param end_entry|_ End point in the tokens list
            */
            context(context* parentContext, Type aType, token_list& tokens, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Compile the context
            * @returns Compiled contents as a string
            */
            std::string Compile();

            /**
            * Check if current context or it's parents are of a given type. Will work it's way all the way through the parent tree
            * @param aType The type that is being checked against the contexts
            */
            bool InScopeType(Type aType);

        private:

            /**
            * Compile regular scope or code
            */
            void compile_scope(token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Compile function parameters
            */
            void compile_params(token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Compile a function definiton
            */
            void compile_function(token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Compile an object definiton
            */
            void compile_object(token_entry& start_entry_, token_entry& end_entry_);


            /**
            * Compile a control structure
            * Possible control structures: if, switch, while
            */
            void compile_control_structure(token_entry& start_entry_, token_entry& end_entry_);
            

            token_list& tokens;
            token_entry& start_token;
            token_entry& end_token;
        };

        namespace exception {
            class syntax_error : std::exception {
                public:
                    syntax_error(const std::string& msg) : msg_(msg) {}
                    ~syntax_error() {}

                    std::string getMessage() const { return(msg_); }
                private:
                    std::string msg_;
            };
            class invalid_context : std::exception {
            public:
                invalid_context(const std::string& msg) : msg_(msg) {}
                ~invalid_context() {}

                std::string getMessage() const { return(msg_); }
            private:
                std::string msg_;
            };
            class missing_bracket : std::exception {
            public:
                missing_bracket(const std::string& msg) : msg_(msg) {}
                ~missing_bracket() {}

                std::string getMessage() const { return(msg_); }
            private:
                std::string msg_;
            };
            class mission_quotation : std::exception {
            public:
                mission_quotation(const std::string& msg) : msg_(msg) {}
                ~mission_quotation() {}

                std::string getMessage() const { return(msg_); }
            private:
                std::string msg_;
            };
            class invalid_operator : std::exception {
            public:
                invalid_operator(const std::string& msg) : msg_(msg) {}
                ~invalid_operator() {}

                std::string getMessage() const { return(msg_); }
            private:
                std::string msg_;
            };
            class unknown : std::exception {
            public:
                unknown(const std::string& msg) : msg_(msg) {}
                ~unknown() {}

                std::string getMessage() const { return(msg_); }
            private:
                std::string msg_;
            };
        };
	};
};

typedef carma::compiler::context CarmaScope;
typedef std::vector<carma::compiler::context> CarmaScopes;

typedef carma::compiler::exception::invalid_context CarmaInvalidContextException;
typedef carma::compiler::exception::invalid_operator CarmaInvalidOperatorException;
typedef carma::compiler::exception::missing_bracket CarmaMissingBracketException;
typedef carma::compiler::exception::mission_quotation CarmaMissionQuotationException;
typedef carma::compiler::exception::syntax_error CarmaSyntaxErrorException;
typedef carma::compiler::exception::unknown CarmaUnkownException;
