#include "ErrorCode.hpp"
#include "Globals.hpp"
#include "Options.hpp"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include <cstdlib>
#include <iostream>
#include <llvm/IR/Verifier.h>

void emitObject(compiler_options *opts) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  // target triple
  // TODO: cmake should set this so we can cross compile
  auto TargetTriple = llvm::sys::getDefaultTargetTriple();

  // initialize the target
  std::string Error;
  auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

  auto CPU = "generic";
  auto Features = "";

  llvm::TargetOptions opt;
  auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features,
                                                   opt, llvm::Reloc::Static);

  std::error_code EC;

  std::string outName;
  std::string finalName;

  if (opts->outName) {
    outName = opts->outName;
    finalName = outName;
  } else {
    outName = opts->inName ? opts->inName : "out";
    finalName = outName;
    switch (opts->fileType) {
    case llvm::CodeGenFileType::ObjectFile:
      outName += ".o";
      break;
    case llvm::CodeGenFileType::AssemblyFile:
      outName += ".s";
      break;
    default:
      outName += ".o";
      break;
    }
  }
  llvm::raw_fd_ostream dest(outName, EC, llvm::sys::fs::OF_None);

  if (EC) {
    exit(1);
  }

  if (llvm::verifyModule(*TheModule, &llvm::errs())) {
    std::cerr << "Error: Module verification failed." << std::endl;
    return;
  }

  llvm::legacy::PassManager pm;

  // IMPORTANT! Set the machine to emit an assembly
  if (TargetMachine->addPassesToEmitFile(pm, dest, nullptr, opts->fileType)) {
    std::cerr << "TheTargetMachine can't emit a file of this type";
    exit(ERROR_CODE::OTHER_ERROR);
  }

  TheModule->setDataLayout(TargetMachine->createDataLayout());
  TheModule->setTargetTriple(TargetTriple);

  if (!TheModule) {
    exit(ERROR_CODE::OTHER_ERROR);
  }
  pm.run(*TheModule);
  dest.flush();

  // link the object file using ld
  // TODO: make this cross platform by using llvm's lld
  std::string linker = "ld -o " + finalName + " " + outName;
  std::string remove = "rm " + outName;
  if (opts->fileType == llvm::CodeGenFileType::ObjectFile &&
      !system(linker.c_str()) && !opts->outName) {
    system(remove.c_str());
  }
}
