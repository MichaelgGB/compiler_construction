#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "symbol_table_types.h" // For TypeKind and Scope forward decl
#include "ast_fwd.h"            // Forward declarations of AST nodes if needed, or include full ast.h

// --- Symbol Table Entry ---
typedef struct SymbolTableEntry
{
    char *name;
    TypeKind type;
    int line_number;                  // Line number of declaration
    int is_final;                     // Is it a final variable?
    int is_initialized;               // Has it been initialized? (for semantic checks)
    struct AstNode *declaration_node; // Pointer back to the AST declaration node
    struct SymbolTableEntry *next;    // For linked list in scope
} SymbolTableEntry;

// --- Scope Structure ---
// Full definition of Scope
struct Scope
{
    SymbolTableEntry *head_entry; // Head of the list of symbols in this scope
    struct Scope *parent;         // Pointer to the parent (enclosing) scope
    int scope_level;              // Depth of the scope (0 for global)
    // You might add more: e.g., a list of child scopes, scope name for debugging
};

// --- Global Scope Management (declared here, defined in symbol_table.c) ---
extern Scope *current_scope;
extern Scope *global_root_scope;

// --- Symbol Table Operations ---
void init_symbol_table(void);
void free_symbol_table(void);

Scope *create_scope(Scope *parent_scope);
void enter_scope(void); // Creates a new scope and enters it
void exit_scope(void);  // Exits current scope, does not free it

int insert_symbol(const char *name, TypeKind type, int line_number, int is_final, struct AstNode *decl_node);
SymbolTableEntry *lookup_symbol(const char *name);                  // Searches current and parent scopes
SymbolTableEntry *lookup_symbol_in_current_scope(const char *name); // Searches only current scope

// Helper (if needed externally, otherwise static in .c)
// void free_symbol_entry(SymbolTableEntry *entry);

#endif // SYMBOL_TABLE_H