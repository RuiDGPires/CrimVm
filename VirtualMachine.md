# Virtual Machine

## Core Components

### Loader and Linker

These two may be implemented as a single component.

#### Loader

Loads the application into memory, parse the package into data structures and load addition resources needed.

Produce refelction data and metadata at loadtime to help the VM understand the application.

#### Linker

Resolves all the referenced symbols into accessible memory addresses. May trigger the loader to load more data and code if symbols are not already loaded.

### Execution Engine

Performs operations specified by the program code.

### Memory Manager

Manages data (and memory containing the data)

Types of data:

- Virtual machine data:
		VM needs memory to load the application code and hold supporting data -> invisible to the application

- Application data:
		Storage for static and dynamic data. Dynamic -> heap

The memory manager usually only manages the application data and mainly the dynamic part of it.

### Thread Scheduler

Support multithreading. Manage processor usage.

### Language Extension

Foreign function interface


-----------------


A simple interpreter:

```
interpret(method){
	while(code remains in sequence){ // Dispatching loop
		read the next code from the sequence;
		if (code needs more data){
			read more data from the sequence;
		}
		perform actions specified by the code;
	}
}
```
