# CrimVm

Specs of the VM can be found [here](docs/specs.md).

Project is UNIX focused for now, maybe that will change later.

## Example

Using [example.cas](tests/inputs/example/example.cas) as an example:

First, assemble the text file into an object file

>./crimvm -a example.cas -o example.o

Then, run the file

>./crimvm example.o

Output:

>60

**or**

Simply assemble and run (generates no files)
>./crimvm -ar example.cas

