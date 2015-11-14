#pragma once

#include "compiler\tokenizer.hpp"
#include "compiler\compiler.hpp"


using namespace carma::tokenizer;
using namespace carma::compiler;

namespace carma {
	namespace process {
		std::string process_input(std::string input_);
	}
}