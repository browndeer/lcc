# Makefile for lc2c2

CFLAGS=-g -O2 

DEFS=-DYYERROR_VERBOSE=1

prefix=/usr/local/browndeer
exec_prefix=${prefix}

### this useless define is needed to silence useless autoconf warning
datarootdir=${prefix}/share


######################################################################

BIN_NAME = lc2c

INSTALL_BIN_DIR=${exec_prefix}/bin
INSTALL_LIB_DIR=${exec_prefix}/lib
INSTALL_INCLUDE_DIR=${prefix}/include
INSTALL_MAN_DIR=${datarootdir}/man

OBJS = compiler.o compiler_scan.o compiler_gram.o compiler_node.o 

CFLAGS += -O1 -fPIC 

YACC = yacc
YACCFLAGS =  -y -v 

LEX = lex
LEXFLAGS = 

COMPILER_YYPREFIX = __compiler_

all: $(BIN_NAME)

.SUFFIXES:
.SUFFIXES: .l .y .c .o

$(BIN_NAME): compiler_gram.c $(OBJS) 
	$(CC) $(CFLAGS) -o $(BIN_NAME) $(INCS) $(OBJS) $(LIBS)

compiler_gram.c: compiler_gram.y
	$(YACC) $(YACCFLAGS) -p $(COMPILER_YYPREFIX) -d $<
	mv y.tab.c compiler_gram.c
	mv y.tab.h compiler_gram.h

compiler_scan.c: compiler_scan.l compiler_gram.c compiler_gram.h
	$(LEX) $(LEXFLAGS) -P$(COMPILER_YYPREFIX) $<
	mv lex.$(COMPILER_YYPREFIX).c compiler_scan.c

.c.o:
	$(CC) $(CFLAGS) $(DEFS) $(INCS) -c $<

install:
	install -m 755 $(BIN_NAME) $(INSTALL_BIN_DIR)

uninstall:
	rm -f $(INSTALL_BIN_DIR)/$(BIN_NAME)

clean:
	rm -f *.o 
	rm -f $(BIN_NAME)
	rm -f compiler_gram.c compiler_gram.h compiler_scan.c y.output

distclean: clean

