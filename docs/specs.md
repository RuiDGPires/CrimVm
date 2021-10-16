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
| LAND | u8 REG1, u8 REG2 | Performs logic and between the values in the registers and stores the result in REG1 |
| LOR | u8 REG1, u8 REG2 | Performs logic or between the values in the registers and stores the result in REG1 |
| LNOT | u8 REG | Performs logic not between the values in the registers and stores the result in REG1 |
| GTU | u8 REG1, u32 REG2| Stores 1 in REG1 if REG1 > REG2 (unsigned), else stores 0 |
| GTI | u8 REG1, u32 REG2| Stores 1 in REG1 if REG1 > REG2 (signed), else stores 0 |

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
| SYS <id> | Executes the system call identified by <id>. Documentation for sys calls is bellow |

---

## Registers

| Register | Decription | Preserved after a function call |
| :---: | :--- | :---: |
| R0 | A register that always equals to 0 | Yes |
| R1 | General purpose register | No |
| R2 | General purpose register | No |
| R3 | General purpose register | No |
| R4 | General purpose register | No |
| R5 | General purpose register | No |
| R6 | General purpose register | No |
| R7 | General purpose register | No |
| R8 | Auxiliar register | Yes |
| R9 | Auxiliar register | Yes |
| RS | Stack pointer | Yes (Except for passed arguents) |
| RF | Frame pointer | Yes | 
| RE | Return program address | No |

---

## Flags

| Flag | Description |
| :---: | :--- |
| Z | Value is zero |
| P | Value is positive |
| N | Value is negative |
| O | Value overflowed |

---

## System calls

Args and return are done via stack and are presented by the order in which they are pushed

| ID | NAME |  Args | Return | Description |
| :---: | :---: | :---: | :---: | :--- |
| 0 | OPEN | Address, Mode | Fd | Opens the file with name starting at **Address** with **Mode**. Returns the file descriptor |
| 1 | CLOSE | Fd | | Closes the file descriptor **fd**| 
| 2 | WRITE | Fd, Address, N | n | Writes **N** bytes (memory spaces) starting at **Address** to **Fd**, returns the number of bytes written |
| 3 | READ | Fd, Address, N | n | Reads **N** bytes (memory spaces) from **Fd** to the **N** locations starting at **Address**, returns the number of bytes read |
| 4 | REMOVE | Address | | Removes the file with name starting at **Address** |
| 5 | SHELL| Address | Res | Calls the command with text starting at **Address** and returns the result |

### File opening modes

- 0: Read
- 1: Write
 
---

## Exit codes

The exit code of the program will be equal to the last value on the stack. If the stack is empty, the exit code 0 is returned. If the stack has more than one item on it, an error is thrown.

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
