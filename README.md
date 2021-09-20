# CrimVm

Specs of the VM can be found [here](Specs.md).

## Example

Using [example.cas](tests/inputs/example/example.cas) as an example:

First, assemble the text file into an object file

>./a.out -a example.cas -o example.o

Then, run the file

>./a.out example.o

Output:

>60

**or**

Simply assemble and run (generates no files)
>./a.out -ar example.cas

