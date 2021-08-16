#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "../utility.h"
#include "../Registers/registers_symbols.h"
#include "../Memory/memory_symbols.h"

/* Registers statuses/values before call:
*	SR: input filename
*  Stack:
*	... file_offset output_addr
*  Arg:
*	number of bytes to read
*  Exit code: set to rax
*/
void ex_rfread(std::shared_ptr<void> args_p, regs* registers, memory* mem);
/* Registers statuses/values before call:
*	SR: output filename
*  Stack:
*	... file_offset input_addr
*  Arg:
*	number of bytes to write
*  Exit code: set to rax
*/
void ex_rfwrite(std::shared_ptr<void> args_p, regs* registers, memory* mem);

// SR: file which size has to be computed
void ex_rflen(std::shared_ptr<void> reg, regs* registers, memory* mem);