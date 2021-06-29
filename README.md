This project allows you to run an engine which emulates threading behaviour. It allows you to run tasks in parallel in consequence.  
Actually, the language used to run tasks is not directly C++, it's VASM (which means Virtual ASM), which is a language that is representing
simplified Assembly (really simplified) with pre-built procedures for string manipulations.

V1.11 Changes:
- [x] Reduced amount of binary code generated/handled by VAS/VEXE engines
- [x] Added Unix & Windows compliant networking library (will be used soonly to make network-based VASM instructions)
- [x] Added symbols of new networking instructions (their integration is in progress)