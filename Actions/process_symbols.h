#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "../Memory/memory_decl.h"
#include "../Registers/registers_symbols.h"

#define THREAD_DEAD -1
#define THREAD_STOPPED 0
#define THREAD_ALIVE 1
#define THREAD_SIGTERM 2

void p_jmp(std::shared_ptr<void> unsued_p, regs* registers, memory* unused_m);
void p_cmp(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void p_cmpstr(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

void p_je(std::shared_ptr<void> args_p, regs* registers, memory* mem);
void p_jne(std::shared_ptr<void> args_p, regs* registers, memory* mem);
void p_jl(std::shared_ptr<void> args_p, regs* registers, memory* mem);
void p_jg(std::shared_ptr<void> args_p, regs* registers, memory* mem);
void p_jle(std::shared_ptr<void> args_p, regs* registers, memory* mem);
void p_jge(std::shared_ptr<void> args_p, regs* registers, memory* mem);

void p_gca(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void p_hlt(std::shared_ptr<void> unused_p, regs* registers, memory* unsused_m);

void p_call(std::shared_ptr<void> value_ptr, regs* registers, memory* mem);
void p_lcall(std::shared_ptr<void> value_ptr, regs* registers, memory* mem);
void p_ret(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);

void p_svcall(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void p_rscall(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);

void p_crtthread(std::shared_ptr<void> thread_id_ptr, regs* registers, memory* mem);
void p_rstthread(std::shared_ptr<void> thread_id_ptr, regs* registers, memory* mem);
void p_endthread(std::shared_ptr<void> thread_id_ptr, regs* registers, memory* mem);