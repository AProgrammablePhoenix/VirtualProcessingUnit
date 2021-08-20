cmake_minimum_required (VERSION 3.8)

project("svas")

add_executable(svas "SVASM/Assembler/main.cpp" "SVASM/Assembler/parser.cpp" "SVASM/Assembler/parser.h" "SVASM/Assembler/compiler.cpp"
			"SVASM/Assembler/compiler.h" "VirtualAssembler/instructions_set.cpp" "SVASM/Assembler/tokenizer.cpp" "SVASM/Assembler/util.h")

target_compile_features(svas PUBLIC cxx_std_17)