| OPERATION | ARGUMENTS | DESCRIPTION |
| :---			| :----:		| ---:				|
|	OP_MVI	| u8 REG, u32 val | Stores <val> in register <REG> |
|	OP_MOV	| u8 REG1, u8 REG2 | Copies the value in REG2 to REG 1 |
|	OP_ADD | u8 REG, u8 REG | Adds the values in the registers and pushes the result |
| OP_DUMP |  | Pops a value from the stack and prints it to the console |
| OP_STORE | u8 REG1, u8 REG2 | Stores the value in REG2 to the location specified in REG1 |
| OP_LOAD | u8 REG1, u8 REG2 | Loads the value in the location REG2 onto REG1 |
| OP_PUSH | u8 REG | Pushes to the stack the value in REG |
| OP_POP | u8 REG | Pops from the stack to REG |
