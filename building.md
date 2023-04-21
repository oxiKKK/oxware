# Building the cheat

These are some things you should know when building cheat modules, unless you want to explicitly break the injection process.

## 1. Shellcode

The shellcode routine is a function that is copied over to another process's memory and begins execution there. In order to properly execute the function, and since we define it inside our code not in the remote process, it MUSN'T have any dependent code. I'll explain.

For example you define a shellcode function and it starts the exeuction inside another process, fine. The function will have a function call to `strcpy`. Oh well, but since `strcpy` that is here is pointing to an address based on our process's address space, when executed in another process, the function will basically point to invalid memory location. So this is the problem, and how to get around this?

Well, you need to pass everything via an argument to the shellcode function. This argument would contain all the function pointers that you will use inside the functions (including strings! you cannot just use raw const char pointers inside the shellcode routine). This data itself has to have allocated region inside the target process.

## 2. What CANNOT be inside a shellcode routine

- anything from this module or other modules loaded by current executable (functions, global variables, strings, NOTHING.) simply - just pure code, because it's executed inside another process and yet it don't have access to local process memory. (as already stated before)
- lambdas, since they're a separate function call, unless inlined (and you CANNOT explicitly inline a lambda..)

### __declspec(safebuffers)

Tells the compiler not to insert buffer overrun security checks for a function. This is so that the /GS compiler option isn't enabled on single variable / function. The shellcode routine must have this. This has to also do with the security cookie concept.

https://learn.microsoft.com/en-us/cpp/cpp/safebuffers?view=msvc-170&viewFallbackFrom=vs-2017

https://stackoverflow.com/questions/19945479/what-is-security-cookie-in-c

## 3. What compiler options can affect the shellcode 

Some of these options must be disabled globally, and some only on the source files of where the shellcode is located. For details see below.

| Compiler option name | Must be disabled globally | Must be disabled only on shellcode source file |
| :---: | :---: | :---: |
| /RTC | No | Yes |
| /JMC | No | Yes |
| Incremental linking | Yes | Yes |
| /Gy | No | Yes |


### /RTC or "Run-time error checks"

The compiler explicitly inserts functions that makes the code safer - but we don't want this inside of our shellcode.

This MUST BE DISABLED on the source file where the shellcode routine is placed. Can be enabled on other files.

https://learn.microsoft.com/en-us/cpp/build/reference/rtc-run-time-error-checks?view=msvc-170

### /JMC or "Enable Just My Code" / "Basic Runtime Checks"

Similar to /RTC, the compiler explicitly puts functions inside the code at compilation. (e.g. __CheckForDebuggerJustMyCode() and others)

Since this is a C++ feature, it can be enabled, but MUST BE DISABLED on the source file of where the shellcode lies.

https://learn.microsoft.com/en-us/cpp/build/reference/jmc?view=msvc-170

### Incremental linking

incremental linking enables fast linking when compiling however, it affects how functions are placed inside the executable. It explicitly creates a jump table containing entires for every function. When enabled, it breaks the way how we copy the shellcode to the target process as well as getting the shellcode function size at runtime.

This MUST BE DISABLED on ALL projects that MAY BE INJECTED TO TARGET PROCESS, because this is a linker option, it cannot be disabled on one source file. (e.g. the cheat, oxui, util, etc... basically all cheat dependencies...)

https://stackoverflow.com/questions/61841099/where-are-there-areas-in-a-program-full-of-jmp-instructions

https://learn.microsoft.com/en-us/cpp/build/reference/incremental-link-incrementally?view=msvc-170

## /Gy or "Function-level linking"

This compiler option makes it so that functions aren't placed in the same order in memory as they're defined in the source code. When enabled, it breaks the shellcode routine size calculations.

This MUST BE DISABLED on the source file where the shellcode routine is.

https://learn.microsoft.com/en-us/cpp/build/reference/gy-enable-function-level-linking?view=msvc-170