#pragma once
#include "../compiler.hpp"

namespace carma {
    namespace rules {

        class operator_dot_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class function_call_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class array_access_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class member_access_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };
    }
}
