#include "AstNode.hpp"
#include "CodeGen.hpp"
#include "EmitObject.hpp"
#include "ErrorCode.hpp"
#include "Globals.hpp"
#include "Options.hpp"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/CodeGen.h"
#include <iostream>

extern FILE *yyin;
extern int yyparse();

int main(int argc, char **argv) {
  // check for input file in command line
  if (argc < 2) {
    usage(argv[0]);
  }

  comp_options opts;
  int index = parseOptions(argc, argv, &opts);

  yyin = fopen(argv[index], "r");
  // check if file opened successfully
  if (yyin == NULL) {
    std::cerr << "Error: could not open file " << argv[1] << std::endl;
    exit(ERROR_CODE::FILE_ERROR);
  }

  // setup code generation environment before parsing
  setupCodeGen();

  // set the filename for error messages
  filename = argv[1];
  // parse the input file
  yyparse();

  // check if the AST was built successfully
  if (rootAST != nullptr) {
    setupCodeGen();
    codeGen();
    if (opts.fileType != llvm::CodeGenFileType::ObjectFile) {
      set_entryPoint();
    }
    // verify the module
    llvm::verifyModule(*TheModule);
    emitObject(&opts);
  } else {
    std::cerr << RED("Error") << ": could not build AST" << std::endl;
    exit(ERROR_CODE::AST_BUILD_ERROR);
  }

  // free the memory used by the AST and close files
  delete rootAST;
  if (yyin != NULL) {
    fclose(yyin);
  }

  return ERROR_CODE::NO_ERROR;
}
