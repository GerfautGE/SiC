#include "AstNode.hpp"
#include "CodeGen.hpp"
#include "EmitObject.hpp"
#include "ErrorCode.hpp"
#include "Globals.hpp"
#include "Options.hpp"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/CodeGen.h"
#include <iostream>
#include <regex>

extern FILE *yyin;
extern int yyparse();

char *extractFilename(const char *filepath) {
  // Define the regex pattern to match the filename without the extension.
  std::regex pattern(R"(.*[\\/](.+)\.[^\.]+$)");
  std::smatch matches;

  std::string filename = filepath;

  if (std::regex_match(filename, matches, pattern)) {
    // The filename without the extension is in the first capture group.
    return strdup(matches[1].str().c_str());
  }

  // If no match, return an empty string or handle as needed.
  return nullptr;
}

int main(int argc, char **argv) {
  // check for input file in command line
  if (argc < 2) {
    usage(argv[0]);
  }

  compiler_options opts;
  opts.inName = extractFilename(argv[1]);
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
