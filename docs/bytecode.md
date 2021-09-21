# Bytecode

So...
What is going on with this messy bytecode? How is it generated? How does the VM read it?

## Common to all CrimVm bytecode files
Every bytecode file **must** start with the 32 bit word "CRIM" followed by a 32 value corresponding to the where in the program memory the execution should start


## How it is generated

Given the assembly (.cas) file, the asssembling process starts with the pre-processor, which runs through the file, keeping track of the "supposed" program counter by incrementing it for each known operation. Whenever it reaches a label, it updates a symbol table, marking that label with it's corresponding PC.

After that is done, assembling starts by parsing each instruction and checking it's corresponding OPCode and how many arguments to take (and how long they are). Everything is written to the output binary file during this process.

Load and Store instructions are a bit different, because they can also take an offset. This is done by signalling to the loader (via the most significant bit) if there actually is an offset, and what type that offset is (register or literal).
when load or store is found in bytecode, the loader checks each register for the most significant bit, if this is set then there's an offset. The loader then checks the next byte's most significant bit. If it is set, it means that the offset is represented as a register, if not, it is a 32 bit value (it needs to get 3 more bytes).

