#include "FunctionDeclaration.hpp"
#include "ErrorCode.h"
#include <iostream>

FunctionDeclaration::FunctionDeclaration(Identifier *name, Block*body) : id(name), body(body) {
    // if name is main, then create the basic block for the main function
    if (name->to_string() == "main") {
        std::cout << "Creating main function" << std::endl;
    } else {
        // if name is not main, exit for now
        exit(ERROR_CODE::NO_MAIN_ERROR);
    }
}
