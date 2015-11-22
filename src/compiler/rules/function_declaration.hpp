#pragma once
#include "../tokenizer.hpp"
#include "../compiler.hpp"

namespace carma {
    namespace rules {

        class function_keyword_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };

        class return_keyword_rule : public carma::compiler::compiler_rule {
        public:
            static compiler::context::type type;
            bool match(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
            void apply(carma::compiler::context& a_scope, token_list &tokens_, token_entry& start_entry_, token_entry& end_entry_);
        };
    }
}