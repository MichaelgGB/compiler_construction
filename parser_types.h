#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

#include "ast.h" // Include ast.h to get MainMethodNode and VariableDeclarationListNode definitions

// Helper struct to temporarily hold class contents during parsing
typedef struct ParsedClassContents
{
    MainMethodNode *main_method; // No 'struct' keyword needed now due to ast.h
    VariableDeclarationListNode *var_decls;
} ParsedClassContents;

#endif // PARSER_TYPES_H