#pragma once
#include "../tokenizer.hpp"
#include "../compiler.hpp"

namespace carma {
    namespace rules {
        class function_declaration
        {
        public:
            static void functionKeyWord(CarmaScope& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);
            static void returnKeyWord(CarmaScope& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);

            function_declaration();
            ~function_declaration();
        };

    }
}