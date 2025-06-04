// codegen.h
#ifndef CODEGEN_H
#define CODEGEN_H

#include "tac.h"
#include <stdio.h>

// Manages string literals to be placed in .data section
typedef struct StringLiteral
{
    char *label; // e.g., _str0
    char *value; // The actual string content
    struct StringLiteral *next;
} StringLiteral;

// Manages local variable/temporary stack offsets within a function
typedef struct VarOffset
{
    char *name;
    int offset;    // Offset from EBP (e.g., -4, -8)
    TypeKind type; // To know the size (e.g. char is 1 byte, int is 4)
    struct VarOffset *next;
} VarOffset;

typedef struct FunctionContext
{
    char *name;
    VarOffset *locals_head;
    int current_stack_offset;              // Keeps track of next available stack slot, negative from EBP
    int total_stack_size;                  // Total bytes allocated for locals/temps for this function
    StringLiteral *strings_head;           // List of string literals used in this function (or globally)
    struct FunctionContext *next_function; // If handling multiple functions
} FunctionContext;

// Main function to generate x86 assembly from TAC
void generate_x86_assembly(TacInstruction *tac_list, const char *output_filename);

#endif // CODEGEN_H