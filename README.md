# MiniJava Compiler

This project is a compiler for MiniJava, a subset of the Java programming language. It takes MiniJava source code (`.txt` files in this example), performs lexical analysis, parsing, semantic analysis, generates Three-Address Code (TAC) as an Intermediate Representation (IR), and finally produces 32-bit x86 assembly code.

## Features

*   **Lexical Analysis:** Converts source code into a stream of tokens.
*   **Parsing:** Builds an Abstract Syntax Tree (AST) from tokens, verifying syntactic correctness.
*   **Semantic Analysis:** Performs type checking, variable declaration checks, and other meaning-related validations.
*   **Intermediate Code Generation:** Translates the AST into Three-Address Code (TAC).
*   **Target Code Generation:** Converts TAC into 32-bit x86 assembly language (NASM syntax).
*   **Supported Language Constructs (based on `test_code.txt`):**
    *   Class and `public static void main()` method structure.
    *   Variable declarations: `int`, `char`, `boolean`.
    *   Initialization of variables.
    *   Arithmetic expressions: `+`, `-`, `*`, `/` (with operator precedence).
    *   Boolean expressions: `>`, `<=`, `&&`.
    *   Conditional statements: `if-else if-else`.
    *   Looping: `while` loops.
    *   `print()` statement for integers, characters, and booleans (outputs "true"/"false").
    *   *(You can expand this list based on what your compiler actually supports, e.g., arrays, other operators, method calls with parameters, etc.)*

## Project Structure
miniJava-Compiler/
├── miniJavac # The compiled compiler executable
├── codegen.c # Source code for x86 code generation
├── codegen.h # Header file for codegen
├── tac.c # Source code for TAC generation (assumption)
├── tac.h # Header file for TAC (assumption)
├── parser.y # Bison/Yacc grammar file (assumption)
├── lexer.l # Flex/Lex lexical analyzer file (assumption)
├── ast.h # Header for Abstract Syntax Tree structures (assumption)
├── symbol_table.h # Header for Symbol Table (assumption)
├── Makefile # Makefile for building the project
├── test_code.txt # Example MiniJava source file
└── README.md # This file

## Prerequisites

To build and run this compiler and its output, you will need:

*   A C compiler (e.g., `gcc`)
*   `make` (for building the compiler)
*   `flex` (or a compatible lexer generator)
*   `bison` (or a compatible parser generator)
*   `nasm` (Netwide Assembler) for assembling the generated x86 code
*   A 32-bit linking environment (e.g., `gcc -m32`)

## Building the Compiler

1.  Navigate to the project's root directory:
    ```bash
    cd /path/to/miniJava-Compiler
    ```
2.  Run `make` to build the `miniJavac` executable:
    ```bash
    make
    ```
    This should compile all necessary C files and link them into the `miniJavac` compiler executable.

## Compiling a MiniJava Program

1.  Once `miniJavac` is built, you can compile a MiniJava source file (e.g., `test_code.txt`):
    ```bash
    ./miniJavac test_code.txt
    ```
2.  This command will perform the following steps:
    *   Lexing and Parsing
    *   Semantic Analysis
    *   Three-Address Code (TAC) generation (printed to the console)
    *   x86 Assembly code generation (saved to `test_code.s` and printed to console)

3.  The output will show the progress, the generated TAC, and the generated x86 assembly. An assembly file named `<sourcename>.s` (e.g., `test_code.s`) will be created.

## Assembling and Running Generated Code

After the compiler generates the `.s` assembly file (e.g., `test_code.s`), you can assemble and run it:

1.  **Assemble** the `.s` file into a 32-bit ELF object file using `nasm`:
    ```bash
    nasm -f elf32 test_code.s -o test_code.o
    ```
2.  **Link** the object file with the C standard library using `gcc` (for `printf`, `exit`, etc.) to create a 32-bit executable:
    ```bash
    gcc -m32 test_code.o -o test_program
    ```
    (You can replace `test_program` with your desired executable name).
3.  **Run** the executable:
    ```bash
    ./test_program
    ```
    The output of the MiniJava program's `print()` statements will be displayed on the console.
Known Issues / TODO
1. Unhandled TAC Opcode for &&: The x86 code generator currently does not handle the TAC opcode for logical AND (&&). This means the _t5 = _t4 && isActive TAC instruction is not translated, potentially leading to incorrect behavior in conditional statements using &&. The relevant x86 code needs to be implemented in codegen.c.
2. Memory Management Error in Compiler: The compiler (miniJavac) itself shows a double free or corruption (fasttop) error upon exiting after generating code. This indicates a memory management bug within the compiler's C code that needs to be debugged and fixed (likely related to freeing AST or TAC structures).
3. Support for arrays (declaration, access, new array).
4. Support for method calls with parameters and return values.
5. More data types (e.g., float, String objects).
6. Additional optimizations at IR or target code level.
7. Error reporting improvements.

Author(s)
MichaelgGB, Calebnzm, 
