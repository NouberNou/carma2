#pragma once

#include "../compiler.hpp"

namespace carma {
    namespace rules {
        class control_structures
        {
        public:
            control_structures();
            ~control_structures();

            /**
            * Handle the if statement
            * @param aScope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            * @returns all code blocks (scopes) from control structure
            */
            static carma::compiler::scopes if_statement(carma::compiler::context& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Handle the switch statement
            * @param aScope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            * @returns all code blocks (scopes) from control structure
            */
            static carma::compiler::scopes switch_statement(carma::compiler::context& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Handle the while loop
            * @param aScope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            * @returns all code blocks (scopes) from control structure
            */
            static carma::compiler::scopes while_statement(carma::compiler::context& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Handle the waitUntil loop
            * @param aScope Scope context
            * @param start_entry_ Starting point
            * @param end_entry_ Max token to where can be parsed
            * @returns all code blocks (scopes) from control structure
            */
            static carma::compiler::scopes waituntil_statement(carma::compiler::context& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };
    }
}
