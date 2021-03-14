#pragma once

#include <iostream>
#include <string>

#include "../Memory/memory_decl.h"
#include "../Registers/registers_symbols.h"

void p_inverseJmpSign(void* unused_p, regs* registers, memory* unused_m);
void p_jmp(void* unsued_p, regs* registers, memory* unused_m);
void p_cmp(void* unused_p, regs* registers, memory* mem);
void p_cmpstr(void* unused_p, regs* registers, memory* mem);

void p_je(void* unused_p, regs* registers, memory* unused_m);
void p_jne(void* unused_p, regs* registers, memory* unused_m);
void p_jl(void* unused_p, regs* registers, memory* unused_m);
void p_jg(void* unused_p, regs* registers, memory* unused_m);
void p_jle(void* unused_p, regs* registers, memory* unused_m);
void p_jge(void* unused_p, regs* registers, memory* unused_m);

void p_gca(void* unused_p, regs* registers, memory* mem);
void p_hlt(void* unused_p, regs* registers, memory* unsused_m);

void p_call(void* unused_p, regs* registers, memory* unused_m);
void p_lcall(void* unused_p, regs* registers, memory* unused_m);
void p_ret(void* unused_p, regs* registers, memory* unused_m);

void p_svcall(void* unused_p, regs* registers, memory* unused_m);
void p_rscall(void* unused_p, regs* registers, memory* unused_m);