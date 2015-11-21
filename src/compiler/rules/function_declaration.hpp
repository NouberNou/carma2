#pragma once
#include "../tokenizer.hpp"
#include "../compiler.hpp"

namespace carma {
    namespace rules {
        class function_declaration
        {
        public:

            /**
            * Handle the function keyword
            * @param aScope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            */
            static void function_keyword(carma::compiler::context& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);

            /**
            * Handle the return keyword
            * @param aScope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            */
            static void return_keyword(carma::compiler::context& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);

            function_declaration();
            ~function_declaration();
        };

    }
}