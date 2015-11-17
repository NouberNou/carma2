#pragma once

#include "../compiler.hpp"

namespace carma {
    namespace rules {
        class control_structures
        {
        public:
            control_structures();
            ~control_structures();

            static CarmaScopes handleIfStatement(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            static CarmaScopes handleSwitchStatement(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            static CarmaScopes handleWhileStatement(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            static CarmaScopes handleWaitUntilStatement(CarmaScope& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };
    }
}
