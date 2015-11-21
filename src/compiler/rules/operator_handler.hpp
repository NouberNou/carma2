#pragma once
#include "../compiler.hpp"

namespace carma {
    namespace rules {
        class operator_handler
        {
        public:
            operator_handler();
            ~operator_handler();
            
            /**
            * Handle the . operator
            * @param aScope Scope context in which this operator is placed
            * @param start_entry_ Location of the operator in the token list
            * @param end_entry_ Max point to where can be parsed
            */
            static void dot_operator(carma::compiler::context& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Handle the () operator
            * @param aScope Scope context in which this operator is placed
            * @param start_entry_ Location of the operator in the token list
            * @param end_entry_ Max point to where can be parsed
            */
            static void method_call_operator(carma::compiler::context& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Handle the [] operator
            * @param aScope Scope context in which this operator is placed
            * @param start_entry_ Location of the operator in the token list
            * @param end_entry_ Max point to where can be parsed
            */
            static void array_access_operator(carma::compiler::context& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);

            /**
            * Handle the {} operator
            * @param aScope Scope context in which this operator is placed
            * @param start_entry_ Location of the operator in the token list
            * @param end_entry_ Max point to where can be parsed
            */
            static void member_access_operator(carma::compiler::context& aScope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };
    }
}
