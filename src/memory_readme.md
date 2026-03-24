# Master document on memory layout, management, and design for BluC
## Index
1. [Notes about this readme](#notes-about-this-readme)
1. [Pointers](#pointers)
    1. [Ownership semantics](#ownership-semantics)
1. [Borrow Checker & Ownership semantics](#borrow-checker--ownership-semantics-wip)
1. [Memory Allocation & deallocation](#memory-allocation--deallocation-wip)
    1. [alloc](#allocating-memory-wip)
    1. [free](#deallocating-memory-wip)
1. [Memory-related attributes](#memory-related-attributes-wip)

## Notes about this readme
This document is a "living document", meaning that it will be updated and edited as the design of memory management in BluC evolves. It will include both finalized designs as well as "in-progress" designs clearly-marked with ***WIP***.

The goal of this document is to have a single source of truth for all things related to memory management in BluC (other than "my brain"), and to keep track of the design process as it unfolds.

## Pointers
As pointers are a fundamental part of memory management, and a frequent source of bugs, all pointers (except those declared [unchecked](#unchecked-pointers)) must be declared with the appropriate [ownership semantics](#ownership-semantics).

To summarize, there are three ownership-based pointer types in BluC, and one "unchecked" pointer type which is itself part of a larger "unchecked" subsystem. Said types are as follows:
1. `owned*` - A pointer that owns the memory it points to. Whichever scope the pointer is declared in, it is responsible for deallocating the memory it points to when it goes out of scope. Pointers
of this type declared inside of a class field must be deallocated in a destructor, else it's a compiler error.

1. `borrowed*` - A pointer that does not own the memory it points to. It is simply a reference to memory owned by someone else, and it is not responsible for deallocating it. Pointers of this type must not outlive the memory they point to, else it's a compiler error.

1. `xfer*` - A pointer that is in the process of being transferred from one owner to another. It is a temporary state that a pointer can be in during a move operation, and it is not responsible for deallocating the memory it points to. Pointers of this type must be transferred out of the current scope, else it's a compiler error. If this pointer is declared as a class field, then the calling context must free it, else it's a compiler error.

1. `unchecked*` - A pointer that is not subject to any ownership semantics. It is simply a raw pointer, and it is the responsibility of the programmer to ensure that it is used correctly. This pointer type is part of a larger "unchecked" subsystem, which includes other features that are not subject to ownership semantics.

    `unchecked` pointers can be used responsibly, but the type system and borrow checker cannot check the safety of said pointers -- hence they are "unchecked". They should only be used when necessary, but "necessary" is left to the programmer to decide. Don't use them everywhere.

### Syntax
The syntax for declaring pointers with ownership semantics is as follows:
```c
owned*      myOwnedPointer;
borrowed*   myBorrowedPointer;
xfer*       myXferPointer;
unchecked*  myUncheckedPointer;
```

## Borrow Checker & Ownership Semantics ***WIP***
Borrow checking and ownership semantics are a crucial part of memory safety in BluC, and they are closely intertwined. The borrow checker is responsible for enforcing the rules of ownership  semantics, and ensuring that pointers are used correctly according to their ownership type.

The ownership semantics provide the rules that the borrow checker enforces. They define how pointers can be used, and what guarantees they provide.

The borrow checker automatically tracks pointer lifetimes and ownership, and it will produce compiler errors if it detects any violations of the ownership rules. If the borrow checker can't determine the lifetime of a pointer, a lifetime may be specified. ***WIP***: how will it be specified?

### Ownership semantics ***WIP***

#### Immutable pointers
Immutable pointers are pointers whose address cannot be changed after initialization.

#### `owned` pointers
This is a pointer whose context owns the memory that the pointer points to. Said context is responsible for deallocating the memory it points to when it goes out of scope, failure to do so is a compiler error. If an `owned` pointer is declared as a class field, then it must be deallocated in a destructor, else it's a compiler error.

Owned pointers cannot be transferred, but they may be borrowed. They are the sole owner of the memory they point to. You cannot have two `owned` pointers pointing to the same memory, as this would violate the ownership rules, but you may have as many `borrowed` pointers pointing to the same memory as needed.

> If you want to transfer ownership of an `owned` pointer, then you probably want an [xfer pointer](#xfer-pointers) instead, which is designed for that purpose.

> If you want to reference memory owned by an `owned` pointer without taking ownership, then you probably want a [borrowed pointer](#borrowed-pointers) instead, which is designed for that purpose.

#### `borrowed` pointers
This pointer type is intended for use when a pointer needs to reference memory owned by someone else.

It is not responsible for deallocating the memory it points to -- to do so is a compiler error. This pointer type also must not outlive the memory it points to, else it's a compiler error. Borrowed pointers cannot be transferred via an `xfer` pointer, but they may themselves be borrowed -- simply `return` them, pass them to another function, or assign them to another borrowed pointer.

#### `xfer` pointers
This pointer type is intended for use during move operations, when a pointer is being transferred from one owner to another. It is a temporary state that a pointer can be in, and it is not responsible for deallocating the memory it points to.

This pointer type must be transferred out of the current scope, else it's a compiler error. If this pointer is declared as a class field, then the context owning the class must free it (unless it is transferred again), else it's a compiler error. This pointer type cannot be owned or borrowed. You may transfer it whilst it is in an xfer state -- simply pass it to another function or assign it to another xfer pointer -- but you must not access the original pointer after transferring it, else it's a compiler error. The original "xfer" pointer is essentially "moved" (or transferred) to the new xfer pointer, and the original pointer is no longer valid.

#### `unchecked` pointers
 This pointer type is intended for use in low-level code, such as interfacing with C libraries, or in performance-critical code where the overhead of ownership semantics is not desirable. It is the programmer's responsibility to ensure that these pointers are used correctly, as the compiler will not enforce any rules on them. This pointer type should be used sparingly, and only when absolutely necessary, as it can easily lead to memory safety issues if used incorrectly.



## Memory Allocation & Deallocation ***WIP***
### Allocating memory ***WIP***
### Deallocating memory ***WIP***
Similarly to [alloc](#allocating-memory), "free" is a keyword instead of a function in BluC, and it has special semantics. It is used to deallocate dynamically-allocated memory. This can be from any memory source, an arena allocator, the heap, etc.

Unless specified by the [nofree](#nofree-attribute-wip) attribute, every function and method in BluC has a special parameter called "dealloc", which is a pointer to a function that can be used to deallocate memory. This allows for flexible memory management, as the caller can choose how to deallocate memory.

Functions can still choose which deallocator they want to use, by simply calling it directly instead of "free". For example, to free something allocated on the heap, you can call "stdlib.free_heap(ptr)" instead of "free ptr". This allows for more control over memory management, whilst also simplifying it in most use cases.

#### Syntax
The syntax for "free" is as follows.

To free a pointer:
```c
...

// somewhere else in our program
owned* bobsAddress = new House(name: "bobsHouse", numRooms: 5, alloc: stdlib.memory.heapAlloc);

...

// our call to free
free bobsHouse;
```

To free a pointer with a specific deallocator, from the function's calling context:
```c
free <pointer> using <deallocator>;
```

## Memory-related attributes ***WIP***
### `nofree` attribute ***WIP***
Specifies that a function or method does not have access to the "free" keyword. This does not mean that memory cannot be deallocated within the function, as it can still call deallocators directly, but it does mean that the "free" keyword cannot be used.

This is useful for performance-critical code, where the overhead on the stackframe for the "free" keyword is not desirable, or for low-level code where the programmer wants to have more control over memory management.