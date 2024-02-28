//extern int yyparse();
#include <iostream>

extern FILE *yyin;
extern int yyparse();

char *filename;

/**
 * Print usage message and exit
 * @param name name of the program
*/
const static void usage(std::string name) {
    std::cerr << "Usage: " << name << " <input file>" << std::endl;
    exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }
    // set the filename for error messages
    filename = argv[1]; 
    // parse the input file
    yyparse();
    
    return 0;
}