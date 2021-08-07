#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "../Registers/regs_decl.h"
#include "../Registers/registers_symbols.h"
#include "memory_decl.h"

// Memory stack symbols
void pushMem(std::shared_ptr<void> reg, regs* registers, memory* mem);
void popMem(std::shared_ptr<void> reg, regs* registers, memory* mem);

void pushMemSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void popMemSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

void pushMemCR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void popMemCR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

void pushMemDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void popMemDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

// Memory heap symbols
void movsm(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);
void movgm(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);

void movsmSR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);
void movgmSR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);

void movsmCR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);
void movgmCR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);

void movsmDR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);
void movgmDR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem);

// Memory arrays symbols
void m_declArray(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_setAt(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_getAt(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_getDynSize(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

// Memory dynamic variables symbols
void m_dyndecl(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_dynset(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_dynget(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

// Memory structs symbols
void m_structdecl(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_structselect(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_structdeclprop(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_structset(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void m_structget(std::shared_ptr<void> unused_p, regs* registers, memory* mem);