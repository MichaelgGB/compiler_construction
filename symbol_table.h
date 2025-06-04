#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h" // For TypeKind and other AST-related enums/structs

// Forward declaration
typedef struct Scope Scope;

// --- Symbol Table Entry ---
typedef struct SymbolTableEntry
{
    char *name;                // Identifier name
    TypeKind type;             // Type of the symbol (e.g., TYPE_INT, TYPE_BOOLEAN)
    AstNode *declaration_node; // Optional: Pointer to the AST declaration node (useful for more info)
    int line_number;           // Line number of declaration
    int is_final;              // Flag: 1 if final, 0 otherwise
    int is_initialized;        // Flag: 1 if initialized, 0 otherwise (especially for final)
    // Add other attributes if needed: e.g., for parameters, function signature, etc.
    // For this subset, 'is_final' and 'type' are key.

    struct SymbolTableEntry *next; // For linked list in the scope
} SymbolTableEntry;

// --- Scope Structure ---
typedef struct Scope
{
    SymbolTableEntry *head_entry; // Head of the linked list of symbols in this scope
    Scope *parent;                // Pointer to the parent (enclosing) scope
    int scope_level;              // Nesting depth (0 for global/class, 1 for main, 2+ for blocks)
    // You might add more scope-specific info if needed later
} Scope;

// --- Global Scope Management ---
extern Scope *current_scope;     // Will be managed by your semantic analyzer and IR generator
extern Scope *global_root_scope; // Points to the very first global scope

// --- Symbol Table Operations ---

// Initialization
void init_symbol_table();
void free_symbol_table(); // Frees all allocated scopes and their entries

// Scope Management
Scope *create_scope(Scope *parent_scope); // Creates a new scope
void enter_scope();                       // Creates a new scope and makes it current
void exit_scope();                        // Exits current scope, restores parent as current (does NOT free the scope struct)

// Symbol Management
// Inserts a symbol into the *current* scope.
// Returns 1 on success, 0 if symbol already exists in the current scope (error).
int insert_symbol(const char *name, TypeKind type, int line_number, int is_final, AstNode *decl_node);

// Looks up a symbol by name, starting from the current scope and going up to parent scopes.
// Returns a pointer to the SymbolTableEntry if found, otherwise NULL.
SymbolTableEntry *lookup_symbol(const char *name);

// Looks up a symbol only in the *current* scope.
// Returns a pointer to the SymbolTableEntry if found, otherwise NULL.
SymbolTableEntry *lookup_symbol_in_current_scope(const char *name);

// Memory Management Helpers (used internally by free_symbol_table)
void free_symbol_entry(SymbolTableEntry *entry); // Frees a single entry's contents

#endif // SYMBOL_TABLE_H