| OPERATION | ARGUMENTS | DESCRIPTION |
| :---:			| :----:		| :---				|
| MVI	| u8 REG, u32 val | Stores <val> in register <REG> |
| MOV	| u8 REG1, u8 REG2 | Copies the value in REG2 to REG 1 |
| ADD | u8 REG1, u8 REG2 | Adds the values in the registers and stores the result in REG1 |
| SUB | u8 REG1, u8 REG2 | Subtracts the values in the registers and stores the result in REG1 |
| AND | u8 REG1, u8 REG2 | Performs bitwise and between the values in the registers and stores the result in REG1 |
| OR | u8 REG1, u8 REG2 | Performs bitwise or between the values in the registers and stores the result in REG1 |
| XOR | u8 REG1, u8 REG2 | Performs bitwise xor between the values in the registers and stores the result in REG1 |
| NOT | u8 REG | Performs bitwise or between the values in the registers and stores the result in REG1 |
| DMP |  | Pops a value from the stack and prints it to the console |
| STORE | u8 REG1, u8 REG2 | Stores the value in REG2 to the location specified in REG1 |
| LOAD | u8 REG1, u8 REG2 | Loads the value in the location REG2 onto REG1 |
| PSH | u8 REG | Pushes to the stack the value in REG |
| POP | u8 REG | Pops from the stack to REG |
| CMP | u8 REG1, u8 REG2 | Updates the flags according to the subtraction of REG1 by REG2|
| BR | [FLAGS, ] Label or u32 val | Branches to the label or offset val|
| JMP | [FLAGS, ] Label or u32 val | Branches to the label or to absolute val storing the previous PC in the RE register|
| END | | Terminates the program |