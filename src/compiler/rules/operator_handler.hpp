#pragma once
#include "../compiler.hpp"

namespace carma {
    namespace rules {
        class operator_handler
        {
        public:
            operator_handler();
            ~operator_handler();

            static void handleDotOperator(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            static void handleMethodCallOperator(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            static void handleArrayAccessOperator(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            static void handleMemberAccessOperator(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };
    }
}
