#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "../Registers/regs_decl.h"
#include "../Registers/registers_symbols.h"
#include "memory_decl.h"

void sdzsMem(std::shared_ptr<void> reg, regs* registers, memory* mem);

// Memory stack symbols
void pushMem(std::shared_ptr<void> reg, regs* registers, memory* mem);
void popMem(std::shared_ptr<void> reg, regs* registers, memory* mem);

void pushMemSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void popMemSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

void pushMemCR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void popMemCR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

void pushMemFPR(std::shared_ptr<void> reg, regs* registers, memory* mem);
void popMemFPR(std::shared_ptr<void> reg, regs* registers, memory* mem);

// Memory heap symbols
void movsm(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);
void movgm(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);

void movsmSR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);
void movgmSR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);

void movsmCR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);
void movgmCR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);

void movsmFPR(std::shared_ptr<void> reg, regs* registers, memory* mem);
void movgmFPR(std::shared_ptr<void> reg, regs* registers, memory* mem);