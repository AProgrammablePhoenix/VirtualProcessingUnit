This project allows you to run an engine which emulates threading behaviour. It allows you to run tasks in parallel in consequence.  
Actually, the language used to run tasks is not directly C++, it's VASM (which means Virtual ASM), which is a language that is representing
simplified Assembly (really simplified) with pre-built procedures for string manipulations.

Towards V1.10.1:
- [ ] Add memory container (unsigned char array | unsigned char*)
- [ ] Add base symbols for memory addressing.
- [ ] Add support of the previous symbols to the compiler, virtual assembler/runner.
- [ ] Patch currently known memory leaks
