//extern int yyparse();
#include <iostream>
#include "AstNode.hpp"
#include "ErrorCode.h"

#define RED(x) "\033[1;31m" x "\033[0m"

extern FILE *yyin;
extern int yyparse();

extern Block *programBlock;

char *filename;

/**
 * Print usage message and exit
 * @param name name of the program
*/
const static void usage(std::string name) {
    std::cerr << "Usage: " << name << " <input file>" << std::endl;
    exit(ERROR_CODE::USAGE_ERROR);
}

int main(int argc, char**argv) {
    // check for input file in command line
    if (argc < 2) {
        usage(argv[0]);
    }
    yyin = fopen(argv[1], "r");
    // check if file opened successfully
    if (yyin == NULL) {
        std::cerr << "Error: could not open file " << argv[1] << std::endl;
        exit(ERROR_CODE::FILE_ERROR);
    }
    // set the filename for error messages
    filename = argv[1];
    // parse the input file
    yyparse();

    // check if the AST was built successfully
    if (programBlock != nullptr) {
        std::cout << "AST built successfully" << std::endl;
    }
    else {
        std::cerr << RED("Error") << ": could not build AST" << std::endl;
        exit(ERROR_CODE::AST_BUILD_ERROR);
    }

    // free the memory used by the AST and close files
    delete programBlock;
    if (yyin != NULL) {
        fclose(yyin);
    }

    return ERROR_CODE::NO_ERROR;
}
