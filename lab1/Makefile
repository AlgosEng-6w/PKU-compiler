# lab1/Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

SRC_DIR = src
LEXER_SRC = $(SRC_DIR)/sysy.l
PARSER_SRC = $(SRC_DIR)/sysy.y
MAIN_SRC = $(SRC_DIR)/main.cpp

LEXER_CPP = $(SRC_DIR)/sysy.lex.cpp
PARSER_CPP = $(SRC_DIR)/sysy.tab.cpp
PARSER_HEADER = $(SRC_DIR)/sysy.tab.hpp

TARGET = compiler

all: $(TARGET)

$(TARGET): $(LEXER_CPP) $(PARSER_CPP) $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(LEXER_CPP): $(LEXER_SRC)
	flex -o $@ $<

$(PARSER_CPP) $(PARSER_HEADER): $(PARSER_SRC)
	bison -d -o $(PARSER_CPP) $<

clean:
	rm -f $(LEXER_CPP) $(PARSER_CPP) $(PARSER_HEADER) $(TARGET)

.PHONY: all clean