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

NASM    := nasm
NASMFLAGS := -f elf32

# 2. Names for generated parser/scanner files
PARSER_C_SRC  := miniJava.tab.c
PARSER_HEADER := miniJava.tab.h
LEXER_C_SRC   := lex.yy.c

# 3. Source files (Manual C files - you provide these)
C_FILES_MANUAL := \
    ast.c                \
    symbol_table.c       \
    semantic_analyzer.c  \
    ir_generator.c       \
    tac.c                \
    codegen.c            \
    main.c

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
.PHONY: all clean run compile_and_run_test assemble_and_link_sfile
all: $(TARGET)

# -------------------------------------------------------------------
# Link step: Create the executable from all object files
# -------------------------------------------------------------------
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)
	@echo "Compiler '$(TARGET)' built successfully."

# -------------------------------------------------------------------
# Generate parser (C source and header) from Bison grammar
# -------------------------------------------------------------------
$(PARSER_C_SRC) $(PARSER_HEADER): miniJava.y
	$(BISON) $(BISONFLAGS) -o $(PARSER_C_SRC) miniJava.y

# -------------------------------------------------------------------
# Generate scanner (C source) from Flex lexer specification
# -------------------------------------------------------------------
$(LEXER_C_SRC): miniJava.l $(PARSER_HEADER)
	$(FLEX) -o $(LEXER_C_SRC) miniJava.l

# -------------------------------------------------------------------
# Compilation rules for each object file.
# -------------------------------------------------------------------
miniJava.tab.o: $(PARSER_C_SRC) $(PARSER_HEADER) ast.h symbol_table_types.h # Added symbol_table_types.h
	$(CC) $(CFLAGS) -c $(PARSER_C_SRC) -o $@

lex.yy.o: $(LEXER_C_SRC) $(PARSER_HEADER) ast.h symbol_table_types.h # Added symbol_table_types.h
	$(CC) $(CFLAGS) -c $(LEXER_C_SRC) -o $@

ast.o: ast.c ast.h $(PARSER_HEADER) symbol_table_types.h # Added symbol_table_types.h
	$(CC) $(CFLAGS) -c ast.c -o $@

symbol_table.o: symbol_table.c symbol_table.h symbol_table_types.h ast.h # Added symbol_table_types.h and ast.h
	$(CC) $(CFLAGS) -c symbol_table.c -o $@

semantic_analyzer.o: semantic_analyzer.c semantic_analyzer.h ast.h symbol_table.h $(PARSER_HEADER)
	$(CC) $(CFLAGS) -c semantic_analyzer.c -o $@

ir_generator.o: ir_generator.c ir_generator.h tac.h symbol_table.h ast.h $(PARSER_HEADER)
	$(CC) $(CFLAGS) -c ir_generator.c -o $@

tac.o: tac.c tac.h ast.h
	$(CC) $(CFLAGS) -c tac.c -o $@

codegen.o: codegen.c codegen.h tac.h
	$(CC) $(CFLAGS) -c codegen.c -o $@

main.o: main.c ast.h symbol_table.h semantic_analyzer.h ir_generator.h tac.h codegen.h $(PARSER_HEADER)
	$(CC) $(CFLAGS) -c main.c -o $@

# -------------------------------------------------------------------
# Clean up generated files and object files
# -------------------------------------------------------------------
clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -f $(PARSER_C_SRC) $(PARSER_HEADER) $(LEXER_C_SRC)
	rm -f miniJava.output # Bison's verbose output file
	rm -f y.output        # Default bison output
	rm -f *.s             # Remove generated assembly files
	rm -f *.tac           # Remove generated TAC files (if your main.c creates them with specific names)
	rm -f test_program    # Remove default test executable name
	@echo "Cleaned up project files."


# -------------------------------------------------------------------
# Target to compile a specific .s file and run its executable
# Usage: make assemble_and_link_sfile S_FILE=test_code.s
# Optional: S_TARGET=my_executable (defaults to S_FILE basename)
# -------------------------------------------------------------------
S_FILE := # Must be set
S_TARGET := $(basename $(S_FILE)) # Default executable name is basename of S_FILE

assemble_and_link_sfile:
ifeq ($(S_FILE),)
	@echo "Usage: make assemble_and_link_sfile S_FILE=<your_assembly_file.s> [S_TARGET=<executable_name>]"
	@exit 1
endif
	@echo "--- Assembling $(S_FILE) ---"
	$(NASM) $(NASMFLAGS) $(S_FILE) -o $(S_TARGET).o
	@echo "--- Linking $(S_TARGET).o to create $(S_TARGET) ---"
	$(CC) -m32 -no-pie $(S_TARGET).o -o $(S_TARGET) -Wl,-z,noexecstack $(LDFLAGS)
	@echo "--- Executable $(S_TARGET) created. Run with: ./$(S_TARGET) ---"
	rm -f $(S_TARGET).o # Clean up intermediate object file


# -------------------------------------------------------------------
# Target to compile a MiniJava file, then assemble, link, and run it
# Usage: make compile_and_run_test MJ_FILE=test_code.txt
# Optional: MJ_TARGET=test_program (defaults to MJ_FILE basename)
# -------------------------------------------------------------------
MJ_FILE := # Must be set by user
MJ_BASE := $(basename $(MJ_FILE))
MJ_S_FILE := $(MJ_BASE).s
MJ_TARGET_EXEC := $(MJ_BASE) # Default executable name for the test

compile_and_run_test: $(TARGET) # Ensure miniJavac compiler is built first
ifeq ($(MJ_FILE),)
	@echo "Usage: make compile_and_run_test MJ_FILE=<your_minijava_file.txt>"
	@exit 1
endif
	@echo "--- Compiling $(MJ_FILE) with $(TARGET) to generate $(MJ_S_FILE) ---"
	./$(TARGET) $(MJ_FILE)
	@echo "--- Assembling $(MJ_S_FILE) ---"
	$(NASM) $(NASMFLAGS) $(MJ_S_FILE) -o $(MJ_TARGET_EXEC).o
	@echo "--- Linking $(MJ_TARGET_EXEC).o to create $(MJ_TARGET_EXEC) ---"
	$(CC) -m32 -no-pie $(MJ_TARGET_EXEC).o -o $(MJ_TARGET_EXEC) -Wl,-z,noexecstack $(LDFLAGS)
	@echo "--- Running $(MJ_TARGET_EXEC) ---"
	./$(MJ_TARGET_EXEC)
	@echo "--- Cleaning up intermediate files for $(MJ_TARGET_EXEC) ---"
	rm -f $(MJ_TARGET_EXEC).o # $(MJ_S_FILE) is kept as output of miniJavac
	# Optionally remove $(MJ_S_FILE) as well if you don't want to keep it:
	# rm -f $(MJ_S_FILE) $(MJ_TARGET_EXEC).o

# -------------------------------------------------------------------
# Generic run target (renamed from 'run' to 'run_compiler' for clarity)
# Pass arguments via ARGS variable: make run_compiler ARGS="input.mj"
# This just runs the miniJavac compiler itself.
# -------------------------------------------------------------------
ARGS := "" # Default to no arguments
run_compiler: $(TARGET)
	./$(TARGET) $(ARGS)


# End of Makefile