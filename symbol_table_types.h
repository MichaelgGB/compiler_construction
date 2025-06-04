#ifndef SYMBOL_TABLE_TYPES_H
#define SYMBOL_TABLE_TYPES_H

// --- Type System ---
typedef enum TypeKind
{
    TYPE_ERROR = -1, // Represents a type error during semantic analysis
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_INT_ARRAY,
    // Add other types like TYPE_CLASS if needed
    // TYPE_FUNCTION, // If functions are first-class citizens or for type checking
    TYPE_UNDEFINED // For symbols before their type is known (e.g. forward decls)
} TypeKind;

// Forward declaration of Scope. Its full definition is in symbol_table.h
// This allows AST nodes to have a pointer to a Scope without needing the full symbol_table.h
typedef struct Scope Scope;

#endif // SYMBOL_TABLE_TYPES_H