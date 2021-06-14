This project allows you to run an engine which emulates threading behaviour. It allows you to run tasks in parallel in consequence.  
Actually, the language used to run tasks is not directly C++, it's VASM (which means Virtual ASM), which is a language that is representing
simplified Assembly (really simplified) with pre-built procedures for string manipulations.

Towards V1.10.1:
- [x] Finalize new stack structure
- [ ] Add memory space manager to be able to store data structures into a specific readonly place.
- [ ] Modify compiler to use new memory space manager
- [ ] Delete old memory symbols