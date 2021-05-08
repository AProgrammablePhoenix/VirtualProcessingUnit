cmake_minimum_required (VERSION 3.8)

project ("ThreadPoolEmulation")
project ("vas")
project ("vexe")

add_library(registers STATIC
			"Registers/regs_decl.h" "Registers/registers_symbols.h" "Registers/registers_symbols.cpp"
			"Registers/mov.cpp" "Registers/native_maths.cpp" "Registers/extended_symbols.cpp" "Registers/native_binary.cpp")

add_library(memory STATIC
			"Memory/memory_decl.h" "Memory/memory_symbols.h" "Memory/memory_symbols.cpp" "Memory/mem_arrays.cpp"
			"Memory/mem_arrays.h" "Memory/dyn_vars.cpp" "Memory/dyn_vars.h" "Memory/mem_structs.cpp" "Memory/mem_structs.h")

add_library(virtual_engine STATIC
			"Actions/actions_symbols.h"  "Actions/action_decl.h" "Actions/action_decl.cpp" "Actions/v_engine.h"
			"Actions/v_engine.cpp" "Actions/threading.h" "Actions/threading.cpp" "Actions/process_symbols.h"
			"Actions/process_symbols.cpp")

add_library(compiler STATIC
			"Compiler/variables.h" "Compiler/variables.cpp" "Compiler/action_parser.cpp" "Compiler/action_parser.h")

target_link_libraries(memory registers)
target_link_libraries(virtual_engine memory)
target_link_libraries(compiler virtual_engine)

add_executable (ThreadPoolEmulation "ThreadPoolEmulation.cpp" "ThreadPoolEmulation.h")

target_link_libraries(ThreadPoolEmulation registers)
target_link_libraries(ThreadPoolEmulation memory)
target_link_libraries(ThreadPoolEmulation virtual_engine)
target_link_libraries(ThreadPoolEmulation compiler)

# vas: virtual assembler
add_executable (vas "VirtualAssembler/assembler_main.cpp"  "VirtualAssembler/assembler.h" "VirtualAssembler/instructions_set.cpp")

target_link_libraries(vas registers)
target_link_libraries(vas memory)
target_link_libraries(vas virtual_engine)
target_link_libraries(vas compiler)

# vexe: virtual executable runner
add_executable (vexe "VirtualAssembler/instructions_set.cpp" "VirtualExecutableRunner/runner_main.cpp" "VirtualExecutableRunner/runner.h")

target_link_libraries(vexe registers)
target_link_libraries(vexe memory)
target_link_libraries(vexe virtual_engine)
target_link_libraries(vexe compiler)

target_compile_features(compiler PUBLIC cxx_std_17)

if (MSVC)    
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
elseif (GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()