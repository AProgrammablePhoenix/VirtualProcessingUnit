# SVASM
- SVASM stands for SimplifiedVASM, indeed this new version of the language VASM
allows you to make cleaner code and, especially, shorter code with a reduced instructions set
compared to the previous version.

### What's new ?
+ First, forget all those set[register], [preprocessed value] and those
mov[register], [register], and welcome the fully remade "mov" instruction, which now
handles both previous set and mov instructions.
+ Note that this new feature will also apply to maths related instructions such as
inc, dec, add, sub, mul, div ; and to memory related instructions.
+ Indeed, memory instructions such as push and pop will be affected by that feature,
but on top of that, "movgm" and "movsm" instructions will be merged into one single
instruction: mload (stands for memoryload)
+ Calls to labels won't need to be saved by 'svcall' and restored by 'rscall' if you want
to make calls and subcalls and so on... Now those instructions will be handled automatically
by the assembler itself when it generates VEXE code.
+ Say goodbye to 'cmpstr', since comparisons between registers will only be made
through 'cmp', type casts will be handled implicitely (for instance, comparing SR and EAX,
will only compare the value of EAX with the numercial value of the first character of SR).

(Of course, refer to [SVASM Roadmap](#svasm-roadmap) to stay informed of the language
implementation status, note that the latter is subject to changes)

### SVASM Roadmap
* [x] Implement mov instruction support
* [ ] Implement maths related instructions
* [ ] Implement memory related instructions
* [ ] Implement auto-saving management for calls
* [ ] Implement new comparisons system