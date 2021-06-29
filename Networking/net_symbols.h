#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <map>

#include "../utility.h"
#include "../Registers/registers_symbols.h"
#include "../Memory/memory_symbols.h"

void net_open(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem);
void net_close(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem);
void net_get(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem);
void net_send(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem);
void net_hrecv(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem);