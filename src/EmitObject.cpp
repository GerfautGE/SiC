#include "ErrorCode.h"
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

void emitObject(comp_options *opts) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  // target triple
  auto TargetTriple = llvm::sys::getDefaultTargetTriple();
  // initialize the target

  std::string Error;
  auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

  auto CPU = "generic";
  auto Features = "";

  llvm::TargetOptions opt;
  auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features,
                                                   opt, llvm::Reloc::PIC_);

  std::error_code EC;

  std::string outName;

  if (opts->outName) {
    outName = opts->outName;
  } else {
    switch (opts->fileType) {
    case llvm::CodeGenFileType::ObjectFile:
      outName = "out.o";
      break;
    case llvm::CodeGenFileType::AssemblyFile:
      outName = "out.s";
      break;
    default:
      outName = "out.o";
      break;
    }
  }
  llvm::raw_fd_ostream dest(outName, EC, llvm::sys::fs::OF_None);

  if (EC) {
    exit(1);
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
  if (!system("ld -o a.out out.o")) {
    system("rm out.o");
  }
}
