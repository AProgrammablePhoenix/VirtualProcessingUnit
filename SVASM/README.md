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
by the assembler itself when generating VEXE code.
+ Say goodbye to 'cmpstr', since comparisons between registers will only be made
through 'cmp', type casts will be handled implicitely (for instance, comparing SR and EAX,
will compare the value of SR with the string representation of EAX).

(Of course, refer to [SVASM Roadmap](#svasm-roadmap) to stay informed of the language
implementation status, note that the latter is subject to changes)

### Few recommandations you should follow
+ Firstly, you should avoid allocating memory on first 128 bytes of memory, because those 128 bytes might be used by compiler, and thus maybe
overwritten at runtime, then it might cause damage to your program, you might allocate data on this area of memory if you only know what you are
doing.
+ Last `N * <stacksize>` bytes of memory are used by your program stack (if it runs as a kernel), where N is number of running threads
(including kernel thread) and stacksize is of 32 KB nowadays (and shouldn't change in the future). you may read/change those bytes values if
you're using RBP or RSP as pointer when calling mload
+ First `<sdzsize>` byte are reserved for static data storage, you can use them to read/write data. To get the size of SDZ, you could the
`sdzs` instruction or implement your own procedure to determine its size. Those bytes are from now on accessible through memory addressing
instructions, no matter if you're using VASM or SVASM.

### SVASM Roadmap
* [x] Implement mov instruction support
* [x] Implement maths related instructions
* [x] Implement memory related instructions (mload for I/O on heap, alloc to resize heap)
* [x] Implement auto-saving management for calls
* [x] Implement new comparisons system
* [x] Implement labels (equivalent of VASM tags, but declared as '`<labelname>:`' instead of '`decltag <tagname>`')
* [x] Implement safe and unsafe code directives (to enable/disable data protection, when assembler needs to put temp value in a register for instance)
* [x] Implement RSP and RBP registers in both VASM and SVASM, changing memory behavior (espcially stack) => unifying stack and user memory
* [x] Unifying unified memory (stack + user memory) with readonly memory (ROZ) [ROZ becoming SDZ (Static Data Zone)]

* [ ] Implement auto assigning values for maths operations (ex: 'mul rax, 3' instead of 'mov rbx, 3 ; mul rax, rbx') -> In progress
* [ ] Implement auto assigning values for memory operations (ex: 'push 3', instead of 'mov rax, 3 ; push rax') -> Planned
* [ ] Implement auto assigning values for comparisons
* [ ] Replace current DR register with FPRx registers where x varies from integers 0 to 3 (4 registers storing floating point numbers), which will be extended later (EFPRx & RFPRx) -> In progress
* [ ] Make files operations more low-level, thus more flexible and optimized -> Planned