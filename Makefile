# -------------------------------------------------------------------
# MiniJava Compiler Makefile
# -------------------------------------------------------------------

# 1. Compiler and tools
CC      := gcc
CFLAGS  := -Wall -g

FLEX    := flex
BISON   := bison

# 2. Names for generated parser/scanner files
#    By default, Bison (with -d) produces miniJava.tab.c and miniJava.tab.h
PARSER_SRC    := miniJava.tab.c
PARSER_HEADER := miniJava.tab.h

#    Flex will produce lex.yy.c
LEXER_SRC     := lex.yy.c

# 3. List of all .c source files (including generated ones)
C_SOURCES := \
    $(PARSER_SRC)        \
    $(LEXER_SRC)         \
    ast.c                \
    symbol_table.c       \
    semantic_analyzer.c  \
    ir_generator.c       \
    tac_gen.c            \
    front_end_main.c

# 4. Corresponding object files
OBJECTS := $(C_SOURCES:.c=.o)

# 5. Final executable name
TARGET := miniJavar

# -------------------------------------------------------------------
# Default target: build the compiler front end
# -------------------------------------------------------------------
all: $(TARGET)

# -------------------------------------------------------------------
# Link step: link all .o files into the final executable
# -------------------------------------------------------------------
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

# -------------------------------------------------------------------
# Rule to compile any .c into .o
# -------------------------------------------------------------------
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# -------------------------------------------------------------------
# Generate parser (miniJava.tab.c and miniJava.tab.h) from grammar
# -------------------------------------------------------------------
$(PARSER_SRC) $(PARSER_HEADER): miniJava.y
	$(BISON) -d miniJava.y
# -d tells Bison to generate the header (miniJava.tab.h) as well

# -------------------------------------------------------------------
# Generate scanner (lex.yy.c) from lexer spec
# -------------------------------------------------------------------
# Make sure it depends on the parser header, because the lexer may include tokens.h
$(LEXER_SRC): miniJava.l $(PARSER_HEADER)
	$(FLEX) miniJava.l
# By default Flex writes to lex.yy.c

# -------------------------------------------------------------------
# Clean up generated files
# -------------------------------------------------------------------
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS) \
	      $(PARSER_SRC) $(PARSER_HEADER) \
	      $(LEXER_SRC)

# -------------------------------------------------------------------
# Optional: a 'run' target to compile and then execute with arguments
# -------------------------------------------------------------------
# Usage: make run ARGS="someFile.java"
run: $(TARGET)
	./$(TARGET) $(ARGS)

# End of Makefile
