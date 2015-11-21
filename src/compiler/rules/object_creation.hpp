#pragma once
#include "../tokenizer.hpp"
#include "../compiler.hpp"

namespace carma {
    namespace rules {
        class object_creation
        {
        public:

            /**
            * Handle the new keyword
            * @param a_scope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            */
            static void new_object(carma::compiler::context& a_scope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);

            /**
            * Handle the anonymous object creation ( { } )
            * @param a_scope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            */
            static void handle_anon_object(carma::compiler::context& a_scope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);
            
            /**
            * Handle the del keyword
            * @param a_scope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            */
            static void handle_del_keyword(carma::compiler::context& a_scope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);
             
            object_creation();
            ~object_creation();
        };
    }
}

