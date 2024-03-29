﻿cmake_minimum_required (VERSION 3.8)
include(${CMAKE_CURRENT_SOURCE_DIR}/SVASM/CMakeLists.txt)

project ("vpu")
project ("vas")
project ("vexe")
project ("testnet")

if (${CMAKE_SIZEOF_VOID_P} LESS 8)
	message(FATAL_ERROR "Platforms that are not supporting 64 bits are not supported")
endif()

add_library(registers STATIC
			"Registers/regs_decl.h" "Registers/registers_symbols.h" "Registers/registers_symbols.cpp"
			"Registers/mov.cpp" "Registers/native_maths.cpp" "Registers/extended_symbols.cpp" "Registers/native_binary.cpp" "utility.h")

add_library(memory STATIC
			"Memory/memory_decl.h" "Memory/memory_symbols.h" "Memory/memory_symbols.cpp" 			    
			"Memory/memory_decl.cpp" "utility.h")

add_library(exSymbols STATIC
			"ExtendedSymbols/files.cpp" "ExtendedSymbols/files.h")

add_library(virtual_engine STATIC
			"Actions/actions_symbols.h"  "Actions/action_decl.h" "Actions/action_decl.cpp" "Actions/v_engine.h"
			"Actions/v_engine.cpp" "Actions/threading.h" "Actions/threading.cpp" "Actions/process_symbols.h"
			"Actions/process_symbols.cpp" "Actions/interrupts.cpp" "Actions/interrupts.h" "utility.h")

add_library(compiler STATIC
			"Compiler/variables.h" "Compiler/variables.cpp" "Compiler/action_parser.cpp" "Compiler/action_parser.h" "utility.h")

add_library(netlib STATIC
			"Networking/net_hdr.h" "Networking/unixnet.cpp" "Networking/winnet.cpp" "utility.h" "Networking/net_symbols.cpp"
			"Networking/net_symbols.h" "Networking/netman.cpp" "Networking/netman.h")

add_library(wrpcurses STATIC
			"CursesWrapper/wrapper.hpp" "CursesWrapper/wrapper.cpp")

target_link_libraries(registers wrpcurses)
target_link_libraries(memory registers)
target_link_libraries(exSymbols memory)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")    
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	target_link_libraries(netlib memory)
	target_include_directories(wrpcurses PUBLIC "${CMAKE_SOURCE_DIR}/includes/")
	target_link_libraries(wrpcurses PUBLIC "${CMAKE_SOURCE_DIR}/lib/pdcurses.lib")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -lncurses")
	find_package(Threads REQUIRED)
	find_package(Curses REQUIRED)
	target_include_directories(wrpcurses PUBLIC ${CURSES_INCLUDE_DIRS})
	target_link_libraries(wrpcurses PUBLIC ${CURSES_LIBRARIES})
	target_link_libraries(netlib PUBLIC Threads::Threads memory)
endif()

target_link_libraries(virtual_engine exSymbols)
target_link_libraries(virtual_engine netlib)
target_link_libraries(compiler virtual_engine)

add_executable (vpu "VPU_main.cpp"  "utility.h")

target_link_libraries(vpu registers)
target_link_libraries(vpu memory)
target_link_libraries(vpu exSymbols)
target_link_libraries(vpu virtual_engine)
target_link_libraries(vpu compiler)

# vas: virtual assembler
add_executable (vas "VirtualAssembler/assembler_main.cpp"  "VirtualAssembler/assembler.h" "VirtualAssembler/instructions_set.cpp" "utility.h")

target_link_libraries(vas registers)
target_link_libraries(vas memory)
target_link_libraries(vas exSymbols)
target_link_libraries(vas virtual_engine)
target_link_libraries(vas compiler)

# vexe: virtual executable runner
add_executable (vexe "VirtualAssembler/instructions_set.cpp" "VirtualExecutableRunner/runner_main.cpp" 
				 "VirtualAssembler/assembler.h" "utility.h")

target_link_libraries(vexe registers)
target_link_libraries(vexe memory)
target_link_libraries(vexe exSymbols)
target_link_libraries(vexe virtual_engine)
target_link_libraries(vexe compiler)

# testnet: serves to test Unix & Windows compliant windows networking library
add_executable (testnet "Networking/testnet.cpp")

target_link_libraries(testnet netlib)

# svas target librairies
target_link_libraries(svas registers)
target_link_libraries(svas memory)
target_link_libraries(svas virtual_engine)

target_compile_features(wrpcurses PUBLIC cxx_std_20)
target_compile_features(registers PUBLIC cxx_std_20)
target_compile_features(memory PUBLIC cxx_std_20)
target_compile_features(compiler PUBLIC cxx_std_17)
target_compile_features(vas PUBLIC cxx_std_17)