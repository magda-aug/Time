# Time

Interpreter executing program in language Time. 
Time is programming language of abstract machine Time.

## Grammar
Syntax of this language is described by grammar with terminal symbols:
* *identifier*
* *number* representing integer written in decimal system, optionally with leading sign "-" or "+".
* signs within apostrophes

Before or after source code of program there can be spaces, tabs, newlines and "|" signs, in any cardinality. They are called separators and don't influence interpretation of program.

The start symbol of grammar is Program.

Program → ε | Element Program

Element → Definition | Instruction

Definition → ':' Label

Instruction → Subtract | Jump | Call | Return | Read | Write

Subtract → Address Address

Jump → Address Label

Call → Label

Return → ';'

Read → '^' Address

Write → Address '^'

Label → identifier

Address → number

## Supported operations
* Subtract "address1 address2"  
Writes to cell of address *address1 result of subtraction of **address2 from **address1.

* Jump "address label"  
Checks if **address is bigger than 0. If so, next executed instruction is instruction described by "label".

* Call "label"   
Tells that next executed instruction is going to be instruction described by "label". It pushes on return stack place in code directly behind current instruction.

* Return   
Tells that next instruction is going to be one popped from return stack. If stack is empty, return instruction shuts down the program.

* Read "^ address"  
Writes in cell of address *address code of sign read with function getchar() from C language or -1, if getchar() returns EOF.

*  Write "address ^"  
Writes, with function putchar() from C language, sign of code **address.
