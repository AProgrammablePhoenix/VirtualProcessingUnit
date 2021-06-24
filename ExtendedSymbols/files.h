#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "../utility.h"
#include "../Registers/registers_symbols.h"
#include "../Memory/memory_symbols.h"

void ex_readFile(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void ex_writeFile(std::shared_ptr<void> unused_p, regs* registers, memory* mem);