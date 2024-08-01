#pragma once
#include "ErrorCode.hpp"
#include "llvm/Support/CodeGen.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

typedef struct comp_options {
  llvm::CodeGenFileType fileType = llvm::CodeGenFileType::ObjectFile;
  char *outName = NULL;
} comp_options;

/**
 * Print usage message and exit
 * @param name name of the program
 */
const static void usage(std::string name) {
  std::cerr << "Usage: " << name << " <input file>" << std::endl;
  exit(ERROR_CODE::USAGE_ERROR);
}

static int parseOptions(int argc, char *argv[], comp_options *opts) {
  int c;
  while ((c = getopt(argc, argv, "sco:")) != -1)
    switch (c) {
    case 's':
      opts->fileType = llvm::CodeGenFileType::AssemblyFile;
      break;
    case 'c':
      opts->fileType = llvm::CodeGenFileType::ObjectFile;
      break;
    case 'o':
      opts->outName = strdup(optarg);
      break;
    case '?':
      if (optopt == 'o') {
        std::cerr << "Option -%c requires an argument.\n"
                  << optopt << std::endl;
        exit(ERROR_CODE::USAGE_ERROR);
      } else if (isprint(optopt)) {
        std::cerr << "Unknown option `-%c'.\n" << optopt << std::endl;
        usage(argv[0]);
      } else {
        std::cerr << "Unknown option character `\\x%x'.\n"
                  << optopt << std::endl;
        exit(ERROR_CODE::OTHER_ERROR);
      }
    default:
      abort();
    }
  return optind;
}
