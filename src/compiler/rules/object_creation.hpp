#pragma once
#include "../tokenizer.hpp"
#include "../compiler.hpp"

namespace carma {
    namespace rules {
        class object_creation
        {
        public:

            static void newObject(CarmaScope& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);
            static void handleAnonObject(CarmaScope& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);
            static void handleDelKeyword(CarmaScope& aScope, tokenizer::token_list &tokens_, tokenizer::token_entry& start_entry_, tokenizer::token_entry& end_entry_);
             
            object_creation();
            ~object_creation();
        };
    }
}

