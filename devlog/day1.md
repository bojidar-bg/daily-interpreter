# Daily Interpreter, Day 1½

## Introduction

I decided to start a series of devlogs or articles about making a simple, interpreted, staticly-typed programming language, and potentially AOT/JIT-compiling it later on. It is inspired by [Aras's "Daily Pathtracer" series](http://aras-p.info/blog/2018/03/28/Daily-Pathtracer-Part-0-Intro/), which is pretty cool; better check it out if you are interested in how pathtracers (such as Blender's Cycles) work.

At this point, I have no idea what the language would look like, so I'm going to start with a simple bytecode and VM, and later on work on the syntax. I would like to do something easily extensible, so that it can be tailored for different projects.

The implementation is going to be written in C, with some slight macro magic, so as to keep the code focused on the logic and not on the repetitive elements.

## The VM

Most interpreted langauges have a VM, which is the component which runs the instructions. Most VMs are based on bytecodes (also known as opcodes), which are just instructions with or without some parameters.

There are two main divisions of VMs and bytecodes: stack-based and register-based. Stack-based opcodes take their parameters off the top of a stack, pop them from there (changing the size of the stack), then push the result. Register-based opcodes take their parameters from locations in memory (which come with the instruction), then save the result to another location.

Doing a quick Google search about stack-based vs register-based VMs gives many results about register-based VMs being faster. At the same time, many languages have stack-based VMs, probably because it is easier to compile for them. With this in mind, I'm going for a register-based VM.

One thing which is nice about stack-based VMs is the ability to leave the return value/s on the top of the stack. I guess my bytecode might support such usage, since it sounds quite cool.

## Implementation

### Bytecode

For the first day, I did a few opcodes to test the waters:

* `Allocate(X)`: Allocate X more words on the stack. I'm thinking of using this at the beginning of functions or scopes.
* `Deallocate(X)`: Deallocate X words of the stack. Thinking to use this when ending a function or scope.
* `Set(X, Y)`: Set the X-th word on the stack to Y. This is to be used to insert initial data or constants.
* `Copy(X, Y, Z)`: Copy Z words from Y into X, where X and Y are positions on the stack. Initially, it had only two parameters and copied a single word, but since I added a third parameter to some other opcodes, it seemed natural to use it here.
* `Int{32,64}{Add,Multiply,etc.}(X, Y, Z)`: Math functions for 32-bit and 64-bit integers, which apply an operation to Y and Z and store the result into X. Going to expand those to 32/64-bit floats and 8/16-bit ints later on, when everything else is working.

### VM

The most basic way to make a VM is to just make a switch inside a while loop. In psuedocode:
```c
while(true) {
  switch(operations[i]) {
    case OpX:
      // Do stuff
      break;
    case OpReturn:
      // At least one opcode should have a return statement
      return 0;
  }
  i ++;
}
```

While this approach works great for simpler usecases, it doesn't use the full power of today's CPUs. This is because it completely confuses the branch predictor, and it ends up unable to guess which is the next instruction before fetching it from memory.

A better solution is to use a computed goto:
```c
jump_table = {
  &&label_OpX, // Take the labels by-address
  &&label_OpReturn,
}
#define dispatch i++; goto jump_table[operations[i]]
i--; dispatch;
{
  label_OpX:
    // Do stuff
    dispatch;
  label_OpReturn:
    return 0;
}
```

Basically, it creates a new branch point at the end of each opcode, instead of a single branch point for the whole program. The code is somewhat contrived, but you can visualize it as a switch with gotos at the end of each instruction, which jumps to the other instructions. Search for "computed goto" for a more throughout explanation.

Since computed goto does not work on MSVC, I implemented both approaches with a bit of macro magic.

### Macros

I used a few macros while implementing the whole thing, most of which just help with not repeating things:

```c
#define CHECK_POINTER(pointer, size) if (pointer < stack || pointer - size > stack_pointer) return 2
```

Additionally, I used a cool technique I learned about earlier, X macros. They allow us to have compile-time lists of stuff without having to repeat things everywhere. They look like this:

```c
// X isn't defined yet
#define MY_LIST \
  X(item_1) \
  X(item_2)

// Later on:
#define X(_item) int _item;
MY_LIST
#undef X

// Compiles to:
int item_1;
int item_2;
```



## Future

So far, there is no programming language, just a simple bytecode interpreter, which even lacks control flow. I plan to add pointer support and control flow to the bytecode, and then start thinking about the language which would use it.

So, that's what I managed to do for about a day and a half, maybe a bit more. Feel free to comment if there is something I missed, or something you would like to see.


[Back to the index](../README.md)