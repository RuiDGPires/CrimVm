# Vm Specs

## Operations
### Arithmetic

The result of the operation (except CMP) is always stored in the first register. These operations always update the flags


| OPERATION | ARGUMENTS | DESCRIPTION |
| :---:			| :----:		| :---				|
| ADD | u8 REG1, u8 REG2 | Adds the values in the registers | 
| SUB | u8 REG1, u8 REG2 | Subtracts REG1 by REG2 | 
| MUL | u8 REG1, u8 REG2 | Multiplies the values in the registers |
| DIV | u8 REG1, u8 REG2 | Subtracts REG1 by REG2 | 
| CMP | u8 REG1, u8 REG2 | Updates the flags according to the subtraction of REG1 by REG2|
| INC | u8 REG | Increments REG by 1 |
| DEC | u8 REG | Decrements REG by 1 |


### Logic

| OPERATION | ARGUMENTS | DESCRIPTION |
| :---:			| :----:		| :---				|
| AND | u8 REG1, u8 REG2 | Performs bitwise and between the values in the registers and stores the result in REG1 |
| OR | u8 REG1, u8 REG2 | Performs bitwise or between the values in the registers and stores the result in REG1 |
| XOR | u8 REG1, u8 REG2 | Performs bitwise xor between the values in the registers and stores the result in REG1 |
| NOT | u8 REG | Performs bitwise not between the values in the registers and stores the result in REG1 |

### Shifting

| OPERATION | ARGUMENTS | DESCRIPTION |
| :---:			| :----:		| :---				|
| SHR | u8 REG | Shifts all the bits in REG once to the right |
| SHL | u8 REG | Shifts all the bits in REG once to the right |

### Loading

| OPERATION | ARGUMENTS | DESCRIPTION |
| :---:			| :----:		| :---				|
| MVI	| u8 REG, u32 val | Stores <val> in register <REG> |
| MOV	| u8 REG1, u8 REG2 | Copies the value in REG2 to REG 1 |

### Read and Write Memory

| OPERATION | ARGUMENTS | DESCRIPTION |
| :---:			| :----:		| :---				|
| STORE | m[REG1] [[Reg or Val]], u8 REG2 | Stores the value in REG2 to the location specified in REG1 with or without an offset|
| LOAD | u8 REG1, m[REG2] [[Reg or Val]] | Loads the value in the location REG2 (with or without an offset) onto REG1 |
| PSH | u8 REG | Pushes to the stack the value in REG |
| POP | u8 REG | Pops from the stack to REG |
| STR | u8 REG, "STRING" | Stores the STRING in the memory subsequent to the address pointed by REG (including) |

### Flow control

| OPERATION | ARGUMENTS | DESCRIPTION |
| :---:			| :----:		| :---				|
| BR | [.FLAGS, ] Label or u32 val | Branches to the label or offset val|
| JMP | [.FLAGS, ] Label or u32 val | Branches to the label or to absolute val storing the previous PC in the RE register|
| END | | Terminates the program |
| RET | | Jumps to the location stored in the RE register |

---


## Traps

| TRAP | DESCRIPTION |
| :---: | :--- |
| GET | Waits for a character via stdin and pushes the obtained value |
| OUT | Pops a value and prints it as an ASCII char |
| PRNT | Pops a memory address and prints every following space in memory until it reaches '\0' |
| DMP | Pops a value from the stack and prints it as an integer|

---


## Registers

| Register | Decription | Preserved after a function call |
| :---: | :--- | :---: |
| R0 | A register that always equals to 0 | Yes |
| R1 | General purpose register | No |
| R2 | General purpose register | No |
| R3 | Auxiliar register | Yes |
| R4 | Auxiliar register | Yes |
| SP | Stack pointer | Yes (Except for passed arguents) |
| RE | Return program address | Yes |

---

## Flags

| Flag | Description |
| :---: | :--- |
| Z | Value is zero |
| P | Value is positive |
| N | Value is negative |
| O | Value overflowed |

---

## Other

Labels can be assigned by writing a word (no spacing) and a ':' at the end, like so:
```
this_is_a_label:
MVI R1 20
(...)
JMP this_is_a_label
```

The name "\_start:" os reserved to point to where the program execution should start. For example:
```
MVI R1 20

_start:
(...)
```
Here, the operation "MVI R1 20" is never executed (unless there is a jump to it) because the program starts ahead of it.
