#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>
#include "tokenizer.hpp"

using namespace carma::tokenizer;

namespace carma {
	namespace compiler {

		std::string build_string(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);
		std::string build_string_pretty(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);
		token_list minimize(const token_list &tokens_, const token_entry start_entry_, const token_entry end_entry_);
        
        void empty_tokens(token_list& tokens_, token_entry& start_entry_, token_entry& end_entry_);

        class context {
        public:
            enum class Type { FILE, SCOPE, OBJECT, FUNCTION, FUNCTION_PARAMS, CONTROL_STRUCTURE, STATEMENT };

            context* parent;
            std::vector<context*> scopes;
            Type type;

            std::vector<std::string> variables;
            std::vector<context*> functions;
            
            context(Type aType, token_list& tokens);
            context(context* parentContext, Type aType, token_list& tokens);


            context(Type aType, token_list& tokens, token_entry& start_entry_, token_entry& end_entry_);
            context(context* parentContext, Type aType, token_list& tokens, token_entry& start_entry_, token_entry& end_entry_);

            std::string Compile();

            bool InScopeType(Type aType);

        private:
            void compile_scope(token_entry& start_entry_, token_entry& end_entry_);
            void compile_params(token_entry& start_entry_, token_entry& end_entry_);
            void compile_function(token_entry& start_entry_, token_entry& end_entry_);
            void compile_object(token_entry& start_entry_, token_entry& end_entry_);
            void compile_control_structure(token_entry& start_entry_, token_entry& end_entry_);
            
            std::string getString();

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
