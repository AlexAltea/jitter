/**
 * (c) 2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#ifndef _JITTER_MAP_H_
#define _JITTER_MAP_H_


#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/TargetSelect.h"

#include <map>
#include <iostream>

namespace jitter {

template <class TKey, class TValue>
class map : public std::map<TKey, TValue> {
    TValue(*m_func)(TKey);
    bool m_compiled;
    bool m_autocompile;

public:
    static_assert(std::is_integral<TKey>::value, "Key type must be integral");
    static_assert(std::is_integral<TValue>::value, "Value type must be integral");

    // Constructor
    map(std::initializer_list<std::pair<const TKey, TValue>> arg) : std::map<TKey, TValue>(arg),
        m_func(nullptr),
        m_compiled(false),
        m_autocompile(false) {
    }
    
    /**
     * Compile the std::map object to a native switch/case function.
     * @return  True on success.
     */
    void compile() {
        llvm::LLVMContext& context = llvm::getGlobalContext();
        llvm::IRBuilder<> builder(context);
        llvm::Module* module = new llvm::Module("module", context);

        llvm::IntegerType* typeI64 = builder.getInt64Ty();
        llvm::FunctionType* funcType = llvm::FunctionType::get(typeI64, { typeI64 }, false);
        llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
        llvm::BasicBlock* bEntry = llvm::BasicBlock::Create(context, "entry", func);
        llvm::BasicBlock* bDefault = llvm::BasicBlock::Create(context, "default", func);

        builder.SetInsertPoint(bEntry);
        llvm::Value* arg = &func->getArgumentList().front();
        llvm::SwitchInst* s = builder.CreateSwitch(arg, bDefault, size());
        for (auto it = begin(); it != end(); it++) {
            llvm::BasicBlock* bCase = llvm::BasicBlock::Create(context, "", func);
            s->addCase(llvm::ConstantInt::get(typeI64, it->first), bCase);
            builder.SetInsertPoint(bCase);
            builder.CreateRet(llvm::ConstantInt::get(typeI64, it->second));
        }
        builder.SetInsertPoint(bDefault);
        builder.CreateUnreachable();

        // Compile function
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        std::unique_ptr<llvm::Module> pmod(module);
        llvm::EngineBuilder engineBuilder(std::move(pmod));
        engineBuilder.setEngineKind(llvm::EngineKind::JIT);
        engineBuilder.setOptLevel(llvm::CodeGenOpt::Default);
        llvm::ExecutionEngine* executionEngine = engineBuilder.create();
        executionEngine->finalizeObject();

        m_func = reinterpret_cast<decltype(m_func)>(executionEngine->getFunctionAddress("main"));
        m_compiled = true;
    }

    // Overriden methods

    /**
     * Returns a reference to the mapped value of the element identified with key k.
     * @param[in]  k  Key value of the element whose mapped value is accessed.
     * @return        Reference to the mapped value of the element with a key value equivalent to k.
     */
    const TValue& at(const TKey& k) const {
        if (m_compiled) {
            return m_func(k);
        } else {
            return std::map<TKey, TValue>::at(k);
        }
    }
};

}  // namespace jitter

#endif  // _JITTER_MAP_H_
