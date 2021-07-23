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

### Few recommandations you should follow
+ First, you should avoid allocating memory on first 128 bytes of memory, because those 128 bytes might be used by compiler, and thus maybe
overwritten at runtime, then it might cause damage to your program, you might allocate data on this area of memory if you only know what you are
doing.

### SVASM Roadmap
* [x] Implement mov instruction support
* [x] Implement maths related instructions
* [x] Implement memory related instructions (mload for I/O on heap, alloc to resize heap)
* [ ] Implement auto-saving management for calls -> In Progress
* [ ] Implement new comparisons system
* [ ] Implement auto assigning values for maths operations (ex: 'mul rax, 3' instead of 'mov rbx, 3 ; mul rax, rbx')
* [ ] Implement labels (equivalent of VASM tags, but declared as '<labelname>:' instead of 'decltag <tagname>')