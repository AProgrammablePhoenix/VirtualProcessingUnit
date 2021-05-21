This project allows you to run an engine which emulates threading behaviour. It allows you to run tasks in parallel in consequence.  
Actually, the language used to run tasks is not directly C++, it's VASM (which means Virtual ASM), which is a language that is representing
simplified Assembly (really simplified) with pre-built procedures for string manipulations.

Towards V1.8:
- [ ] Add new instruction: int (interrupts)
- [ ] Add support for a major part of current instructions to int instruction set.
- [ ] Remove instructions that are supported by int action from the base instructions set, to reduce as high as possible the size of base instruction set.
