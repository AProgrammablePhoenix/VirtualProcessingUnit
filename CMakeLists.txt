cmake_minimum_required (VERSION 3.8)

project ("ThreadPoolEmulation")

add_library(registers STATIC
			"Registers/regs_decl.h" "Registers/registers_symbols.h" "Registers/registers_symbols.cpp"
			"Registers/mov.cpp" "Registers/native_maths.cpp" "Registers/extended_symbols.cpp")

add_library(memory STATIC
			"Memory/memory_decl.h" "Memory/memory_symbols.h" "Memory/memory_symbols.cpp" "Memory/mem_arrays.cpp"
			"Memory/mem_arrays.h" "Memory/dyn_vars.cpp" "Memory/dyn_vars.h")

add_library(virtual_engine STATIC
			"Actions/actions_symbols.h"  "Actions/action_decl.h" "Actions/action_decl.cpp" "Actions/v_engine.h"
			"Actions/v_engine.cpp" "Actions/threading.h" "Actions/threading.cpp" "Actions/process_symbols.h"
			"Actions/process_symbols.cpp")

add_library(compiler STATIC
			"Compiler/variables.h" "Compiler/variables.cpp" "Compiler/action_parser.cpp" "Compiler/action_parser.h")


add_executable (ThreadPoolEmulation "ThreadPoolEmulation.cpp" "ThreadPoolEmulation.h"  )

target_link_libraries(ThreadPoolEmulation registers)
target_link_libraries(ThreadPoolEmulation memory)
target_link_libraries(ThreadPoolEmulation virtual_engine)
target_link_libraries(ThreadPoolEmulation compiler)