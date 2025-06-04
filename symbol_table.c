#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "ast.h" // For error reporting line numbers from nodes if needed

// --- Global Scope Management ---
Scope *current_scope = NULL;
Scope *global_root_scope = NULL; // Defined here, declared extern in .h
int next_scope_level = 0; // To assign scope levels

// --- Internal list to track all created scopes for robust freeing ---
// This dynamic array will hold pointers to all Scope structs allocated by create_scope.
static Scope **all_created_scopes = NULL;
static int num_created_scopes = 0;
static int capacity_created_scopes = 0; // Initial capacity, will grow as needed

// --- Helper Functions (Static for internal use) ---

// Helper function to dynamically resize the all_created_scopes array
static void resize_all_created_scopes_list() {
    int new_capacity = (capacity_created_scopes == 0) ? 4 : capacity_created_scopes * 2;
    Scope **temp = (Scope **)realloc(all_created_scopes, new_capacity * sizeof(Scope *));
    if (!temp) {
        fprintf(stderr, "Fatal: Memory reallocation failed for all_created_scopes list.\n");
        exit(EXIT_FAILURE);
    }
    all_created_scopes = temp;
    capacity_created_scopes = new_capacity;
}

// Helper function to free a single symbol entry (only its dynamically allocated members)
void free_symbol_entry(SymbolTableEntry *entry) {
    if (entry) {
        free(entry->name); // Free the strdup'd name
        // Note: entry->declaration_node is part of the AST and freed separately
        free(entry);
    }
}

// Helper function to free only the entries within a scope (but not the scope struct itself)
static void free_scope_entries_only(Scope *scope) {
    if (!scope) return;
    SymbolTableEntry *current_entry = scope->head_entry;
    SymbolTableEntry *next_entry;
    while (current_entry != NULL) {
        next_entry = current_entry->next;
        free_symbol_entry(current_entry);
        current_entry = next_entry;
    }
    scope->head_entry = NULL; // Mark as empty
}

// --- Symbol Table Operations ---

// Initializes the symbol table system by creating the global scope.
void init_symbol_table() {
    // If already initialized (e.g., from a previous compilation unit), clean up first.
    if (global_root_scope != NULL) {
        free_symbol_table(); // Clean up existing state
    }

    // Create the global scope (level 0)
    global_root_scope = create_scope(NULL); // Global scope has no parent
    global_root_scope->scope_level = 0; // Explicitly set level 0
    current_scope = global_root_scope; // Set current scope to global
    next_scope_level = 1; // Next scope created will be level 1
    // printf("Symbol table initialized. Global scope (level 0) entered.\n");
}

// Frees all allocated scopes and their symbol entries.
void free_symbol_table() {
    // Iterate through the list of all created scopes and free them
    for (int i = 0; i < num_created_scopes; i++) {
        if (all_created_scopes[i] != NULL) {
            free_scope_entries_only(all_created_scopes[i]); // Free entries within this scope
            free(all_created_scopes[i]); // Free the Scope struct itself
            all_created_scopes[i] = NULL; // Mark as freed
        }
    }

    // Free the list that tracked all scopes
    if (all_created_scopes != NULL) {
        free(all_created_scopes);
        all_created_scopes = NULL;
    }

    // Reset global pointers and counters
    current_scope = NULL;
    global_root_scope = NULL;
    num_created_scopes = 0;
    capacity_created_scopes = 0;
    next_scope_level = 0;
    // printf("All symbol table memory freed.\n");
}

// Creates a new scope object.
// This function is also responsible for tracking all created scopes for later cleanup.
Scope *create_scope(Scope *parent_scope) {
    Scope *new_scope = (Scope *)malloc(sizeof(Scope));
    if (!new_scope) {
        fprintf(stderr, "Fatal: Memory allocation failed for new scope.\n");
        exit(EXIT_FAILURE);
    }
    new_scope->head_entry = NULL;
    new_scope->parent = parent_scope;
    new_scope->scope_level = next_scope_level++; // Assign level and increment for next

    // Add this new scope to our global tracking list
    if (num_created_scopes == capacity_created_scopes) {
        resize_all_created_scopes_list();
    }
    all_created_scopes[num_created_scopes++] = new_scope;

    return new_scope;
}

// Enters a new scope by creating it and setting it as the current scope.
void enter_scope() {
    current_scope = create_scope(current_scope); // New scope's parent is the old current_scope
    // printf("Entered scope level %d. Total scopes created: %d.\n", current_scope->scope_level, num_created_scopes);
}

// Exits the current scope, moving current_scope to its parent.
// Note: This function does NOT free the exited scope's memory.
// Memory management is handled centrally by free_symbol_table().
void exit_scope() {
    if (!current_scope) {
        fprintf(stderr, "Fatal: Attempting to exit a null scope.\n");
        return;
    }
    Scope *exited_scope = current_scope; // Keep a pointer to the scope being exited
    current_scope = current_scope->parent; // Move up to parent scope
    next_scope_level--; // Decrement the level counter
    // printf("Exited scope level %d, back to %d.\n", exited_scope->scope_level, current_scope ? current_scope->scope_level : -1);
}

// Inserts a symbol into the *current* scope.
// Returns 1 on success, 0 if symbol already exists in the current scope (redeclaration error).
int insert_symbol(const char *name, TypeKind type, int line_number, int is_final, AstNode *decl_node) {
    if (!current_scope) {
        fprintf(stderr, "Error line %d: Cannot insert symbol '%s', no current scope active.\n", line_number, name);
        return 0; // Failure
    }

    // Check for redeclaration in the *current* scope only
    if (lookup_symbol_in_current_scope(name) != NULL) {
        fprintf(stderr, "Semantic Error line %d: Variable '%s' already declared in this scope.\n", line_number, name);
        return 0; // Failure - redeclaration
    }

    SymbolTableEntry *new_entry = (SymbolTableEntry *)malloc(sizeof(SymbolTableEntry));
    if (!new_entry) {
        fprintf(stderr, "Fatal: Memory allocation failed for symbol entry '%s'.\n", name);
        exit(EXIT_FAILURE);
    }

    new_entry->name = strdup(name); // Duplicate the name string
    if (!new_entry->name) {
        fprintf(stderr, "Fatal: Memory allocation failed for symbol name '%s'.\n", name);
        free(new_entry);
        exit(EXIT_FAILURE);
    }
    new_entry->type = type;
    new_entry->line_number = line_number;
    new_entry->is_final = is_final;
    // Set is_initialized based on initializer presence, and if it's final (finals usually must be initialized)
    new_entry->is_initialized = (decl_node && decl_node->type == NODE_VARIABLE_DECLARATION && ((VariableDeclarationNode *)decl_node)->initializer != NULL) || is_final;
    new_entry->declaration_node = decl_node;
    new_entry->next = NULL; // Initialize next pointer

    // Add to the head of the current scope's linked list of entries
    new_entry->next = current_scope->head_entry;
    current_scope->head_entry = new_entry;

    // printf("Inserted symbol '%s' (type %d) in scope level %d.\n", name, type, current_scope->scope_level);
    return 1; // Success
}

// Looks up a symbol only in the *current* scope.
// Returns a pointer to the SymbolTableEntry if found, otherwise NULL.
SymbolTableEntry *lookup_symbol_in_current_scope(const char *name) {
    if (!current_scope)
        return NULL;
    SymbolTableEntry *current_entry = current_scope->head_entry;
    while (current_entry != NULL) {
        if (strcmp(current_entry->name, name) == 0) {
            return current_entry; // Found in current scope
        }
        current_entry = current_entry->next;
    }
    return NULL; // Not found in current scope
}

// Looks up a symbol by name, starting from the current scope and going up to parent scopes.
// Returns a pointer to the SymbolTableEntry if found, otherwise NULL.
SymbolTableEntry *lookup_symbol(const char *name) {
    Scope *scope_to_search = current_scope; // Start searching from the current active scope
    while (scope_to_search != NULL) {
        SymbolTableEntry *entry = scope_to_search->head_entry;
        while (entry != NULL) {
            if (strcmp(entry->name, name) == 0) {
                return entry; // Found the symbol
            }
            entry = entry->next;
        }
        scope_to_search = scope_to_search->parent; // Move to the parent (enclosing) scope
    }
    // printf("Lookup: Symbol '%s' not found in any accessible scope.\n", name);
    return NULL; // Symbol not found in any scope
}