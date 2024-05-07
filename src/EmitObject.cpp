#include "EmitObject.hpp"
#include "ErrorCode.h"
#include "Globals.hpp"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/CodeGen.h"
#include <iostream>


void emitObject(){
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    // target triple
    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    //initialize the target

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_);

    auto Filename = "output.s";
    std::error_code EC;
    llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

    if (EC) {
      exit(1);
    }

    llvm::legacy::PassManager pm;

    // IMPORTANT! Set the machine to emit an assembly
    if (TargetMachine->addPassesToEmitFile(pm, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        std::cerr << "TheTargetMachine can't emit a file of this type";
        return;
    }

    TheModule->setDataLayout(TargetMachine->createDataLayout());
    TheModule->setTargetTriple(TargetTriple);

    if (!TheModule) {
        std::cerr << "TheModule is null\n";
        exit(ERROR_CODE::OTHER_ERROR);
    }
    pm.run(*TheModule);
    dest.flush();
}
