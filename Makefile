# -------------------------------------------------------------------
# MiniJava Compiler Makefile
# -------------------------------------------------------------------

# 1. Compiler and tools
CC      := gcc
# CFLAGS: -Wall (all warnings), -g (debug symbols), -I. (include current dir for headers like ast.h)
# -Wno-unused-function can be helpful for lex/yacc generated code that might have unused helpers.
# -Wno-parentheses for bison generated code if it triggers it.
# -lm for linking math library if needed by any part (e.g. codegen for abs or other math)
CFLAGS  := -Wall -g -I. -Wno-unused-function -Wno-parentheses
LDFLAGS := -lm # Link math library (for abs() in codegen.c, or other math functions)

FLEX    := flex
BISON   := bison
# BISONFLAGS: -d (generate .h file), -v (generate .output report file for conflicts)
BISONFLAGS := -d -v

# 2. Names for generated parser/scanner files
PARSER_C_SRC  := miniJava.tab.c
PARSER_HEADER := miniJava.tab.h
LEXER_C_SRC   := lex.yy.c

# 3. Source files (Manual C files - you provide these)
#    Ensure main.c contains your main() function.
C_FILES_MANUAL := \
    ast.c                \
    symbol_table.c       \
    semantic_analyzer.c  \
    ir_generator.c       \
    tac.c                \
    codegen.c            \
    main.c               # <-- Changed from front_end_main.c to main.c

# All C source files including generated ones
C_SOURCES := \
    $(PARSER_C_SRC)      \
    $(LEXER_C_SRC)       \
    $(C_FILES_MANUAL)

# 4. Object files (derived from C_SOURCES)
OBJECTS := $(C_SOURCES:.c=.o)

# 5. Final executable name
TARGET := miniJavac

# -------------------------------------------------------------------
# Default target: build the compiler
# -------------------------------------------------------------------
all: $(TARGET)

# -------------------------------------------------------------------
# Link step: Create the executable from all object files
# -------------------------------------------------------------------
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS) # Add $(LDFLAGS) here

# -------------------------------------------------------------------
# Generate parser (C source and header) from Bison grammar
# Depends only on the .y file.
# -------------------------------------------------------------------
$(PARSER_C_SRC) $(PARSER_HEADER): miniJava.y
	$(BISON) $(BISONFLAGS) -o $(PARSER_C_SRC) miniJava.y

# -------------------------------------------------------------------
# Generate scanner (C source) from Flex lexer specification
# Depends on the .l file and the PARSER_HEADER (for token definitions & YYSTYPE).
# -------------------------------------------------------------------
$(LEXER_C_SRC): miniJava.l $(PARSER_HEADER)
	$(FLEX) -o $(LEXER_C_SRC) miniJava.l

# -------------------------------------------------------------------
# Compilation rules for each object file.
# Dependencies here are crucial for correct recompilation.
# -------------------------------------------------------------------

# Object file for the generated parser C code:
# Depends on its C source, the header it generates, and ast.h (for semantic actions).
miniJava.tab.o: $(PARSER_C_SRC) $(PARSER_HEADER) ast.h
	$(CC) $(CFLAGS) -c $(PARSER_C_SRC) -o $@

# Object file for the generated lexer C code:
# Depends on its C source, PARSER_HEADER (for tokens, YYSTYPE via yylval),
# and ast.h (because YYSTYPE in PARSER_HEADER will use types from ast.h).
lex.yy.o: $(LEXER_C_SRC) $(PARSER_HEADER) ast.h
	$(CC) $(CFLAGS) -c $(LEXER_C_SRC) -o $@

# --- Object files for your manually written C code ---
# Add specific header dependencies for each.

ast.o: ast.c ast.h $(PARSER_HEADER) # ast.c might use token definitions for node types/debugging
	$(CC) $(CFLAGS) -c ast.c -o $@

symbol_table.o: symbol_table.c symbol_table.h ast.h
	$(CC) $(CFLAGS) -c symbol_table.c -o $@

semantic_analyzer.o: semantic_analyzer.c semantic_analyzer.h ast.h symbol_table.h $(PARSER_HEADER)
	$(CC) $(CFLAGS) -c semantic_analyzer.c -o $@

ir_generator.o: ir_generator.c ir_generator.h tac.h symbol_table.h ast.h $(PARSER_HEADER)
	$(CC) $(CFLAGS) -c ir_generator.c -o $@

tac.o: tac.c tac.h ast.h # tac.h includes ast.h for TypeKind
	$(CC) $(CFLAGS) -c tac.c -o $@

codegen.o: codegen.c codegen.h tac.h # codegen.h includes tac.h
	$(CC) $(CFLAGS) -c codegen.c -o $@

main.o: main.c ast.h symbol_table.h semantic_analyzer.h ir_generator.h tac.h codegen.h $(PARSER_HEADER)
	$(CC) $(CFLAGS) -c main.c -o $@

# -------------------------------------------------------------------
# Clean up generated files and object files
# -------------------------------------------------------------------
.PHONY: clean all run

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -f $(PARSER_C_SRC) $(PARSER_HEADER) $(LEXER_C_SRC)
	rm -f miniJava.output # Bison's verbose output file (from -v flag)
	rm -f y.output        # Default bison output if -o not specific for it
	rm -f *.s             # Remove generated assembly files
	rm -f *.o             # Also remove .o files (already in $(OBJECTS) but good for explicitness)

# -------------------------------------------------------------------
# Run the compiled program (example)
# Pass arguments via ARGS variable: make run ARGS="input.mj"
# -------------------------------------------------------------------
ARGS := "" # Default to no arguments
run: $(TARGET)
	./$(TARGET) $(ARGS)

# -------------------------------------------------------------------
# Rule to assemble and link a .s file generated by the compiler
# Example: make assemble ASM_FILE=test.s
# This is a utility rule, not part of the compiler build itself.
# -------------------------------------------------------------------
ASM_FILE := # To be set when calling, e.g., make assemble ASM_FILE=myprogram.s
ASM_TARGET := $(ASM_FILE:.s=) # Executable name without .s

assemble:
ifeq ($(ASM_FILE),)
	@echo "Usage: make assemble ASM_FILE=<your_assembly_file.s>"
	@echo "Example: make assemble ASM_FILE=test.s"
else
	@echo "Assembling $(ASM_FILE) with NASM..."
	nasm -f elf32 $(ASM_FILE) -o $(ASM_TARGET).o
	@echo "Linking $(ASM_TARGET).o with GCC (32-bit)..."
	$(CC) -m32 $(ASM_TARGET).o -o $(ASM_TARGET)
	@echo "Executable created: $(ASM_TARGET)"
	@echo "Run with: ./$(ASM_TARGET)"
endif


# End of Makefile