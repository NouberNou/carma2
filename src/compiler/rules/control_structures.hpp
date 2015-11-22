#pragma once

#include "../compiler.hpp"

namespace carma {
    namespace rules {

        class if_statement_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class switch_statement_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class while_statement_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class waituntil_statement_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class foreach_statement_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class try_block_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };        
    }
}
