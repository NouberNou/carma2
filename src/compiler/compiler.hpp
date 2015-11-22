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
        * @param tokens_ The tokens
        * @param start_entry_ Starting point from which the string will be compiled
        * @param end_entry_ End point till which everything will be converted into a string
        * @return A string from start_entry_ until the token before end_entry_
        */
		std::string build_string(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);

        /**
        * Convert tokens into a string
        * @param tokens_ The tokens
        * @param start_entry_ Starting point from which the string will be compiled
        * @param end_entry_ End point till which everything will be converted into a string
        * @return A string from start_entry_ until the token before end_entry_
        */
		std::string build_string_pretty(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);

        /**
        * Convert tokens into a mimimalized version, without empty tokens in between
        * @param tokens_ The tokens
        * @param start_entry_ Starting point
        * @param end_entry_ End point
        * @return A mimized version of the token list
        */
		token_list minimize(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);

        /**
        * Convert tokens to empty tokens
        * @param tokens_ The tokens
        * @param start_entry_ Starting point
        * @param end_entry_ End point
        */
        void empty_tokens(token_list& tokens_, token_entry& start_entry_, token_entry& end_entry_);

        class compiler_rule;

        class context {
        public:

            static std::vector<compiler_rule*> rules;

            /**
            * Possible context types
            */
            enum class type { FILE, SCOPE, OBJECT, FUNCTION, FUNCTION_PARAMS, CONTROL_STRUCTURE, STATEMENT };

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
            type context_type;

            std::vector<std::string> variables;
            std::vector<context*> functions;
            
            /**
            * Create a new parent context
            * @param context_type_ the type of context
            * @param tokens list of tokens that make up the context
            */
            context(type context_type_, token_list& tokens_);

            /**
            * Create a new sub context
            * @param context_type_ the type of context
            * @param tokens list of tokens that make up the context
            */
            context(context* parent_context_, type context_type_, token_list& tokens_);

            /**
            * Create a new parent context
            * @param context_type_ the type of context
            * @param tokens list of tokens that make up the context
            * @param start_entry_ Starting point in the tokens list
            * @param end_entry|_ End point in the tokens list
            */
            context(type parent_context_, token_list& tokens_, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Create a new sub context
            * @param context_type_ the type of context
            * @param tokens list of tokens that make up the context
            * @param start_entry_ Starting point in the tokens list
            * @param end_entry|_ End point in the tokens list
            */
            context(context* parent_context_, type context_type_, token_list& tokens_, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Compile the context
            * @returns Compiled contents as a string
            */
            std::string compile();

            /**
            * Check if current context or it's parents are of a given type. Will work it's way all the way through the parent tree
            * @param context_type_ The type that is being checked against the contexts
            */
            bool in_scope(type context_type_);

        private:

            /**
            * Compile regular scope or code
            */
            void compile_scope(token_entry& start_entry_, token_entry& end_entry_);
            void compile_scope(token_entry& start_entry_, token_entry& end_entry_, const context::type aTypeMatch);

            /**
            * Compile function parameters
            */
            void compile_params(token_entry& start_entry_, token_entry& end_entry_);

            token_list& _tokens;
            token_entry& _start_token;
            token_entry& _end_token;
        };


        class compiler_rule {
        public:
            static context::type type;
            compiler_rule() {};
            ~compiler_rule() {};
            virtual bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_) { return false; };
            virtual void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_) { };
        };

        namespace exception {
			class exception_base {
			public:
				explicit exception_base(const std::string& message_) : _message(message_) {}
				~exception_base() {}

				std::string message() const { return(_message); }
			private:
				std::string _message;
			};

		    class syntax_error : public exception_base { exception_base::exception_base;  };
            class invalid_context : public exception_base { exception_base::exception_base; };
            class missing_bracket : public exception_base { exception_base::exception_base; };
            class missing_quotation : public exception_base { exception_base::exception_base; };
            class invalid_operator : public exception_base { exception_base::exception_base; };
            class unknown : public exception_base { exception_base::exception_base; };
        };

        typedef std::vector<carma::compiler::context> scopes;
	};

    
};


