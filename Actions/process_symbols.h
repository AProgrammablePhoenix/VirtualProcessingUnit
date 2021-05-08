#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "../Memory/memory_decl.h"
#include "../Registers/registers_symbols.h"

void p_inverseJmpSign(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_jmp(std::shared_ptr<void> unsued_p, regs* registers, memory* unused_m);
void p_cmp(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void p_cmpstr(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

void p_je(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_jne(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_jl(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_jg(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_jle(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_jge(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);

void p_gca(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void p_hlt(std::shared_ptr<void> unused_p, regs* registers, memory* unsused_m);

void p_call(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_lcall(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_ret(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);

void p_svcall(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_rscall(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);