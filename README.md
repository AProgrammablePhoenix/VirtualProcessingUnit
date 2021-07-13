This project allows you to run an engine which emulates threading behaviour. It allows you to run tasks in parallel in consequence.  
Actually, the language used to run tasks is not directly C++, it's VASM (which means Virtual ASM), which is a language that is representing
simplified Assembly (really simplified) with pre-built procedures for string manipulations.

V1.12 Changes:
- [x] Implemented sub-threading engine
- [x] Implemented sub-threading symbols
- [x] Added new sub-threading symbols into Compiler instructions set
- [x] Added new sub-threading symbols into VAS/VEXE engines instructions set
- [x] Replaced some pointers parameters to const ref parameters into execution engine