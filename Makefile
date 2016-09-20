CFLAGS ?= -Wall -Wextra -std=c99
LDFLAGS ?=
CC ?= gcc
INCLUDE= -I include/

BIN = graph_test
SRC = graph.c graph_traversal.c main.c
OBJ = $(SRC:.c=.o)

#Mandatory flags
CFLAGS += -fstrict-aliasing ${INCLUDE}

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -g -fno-inline
else
	CFLAGS += -DNDEBUG -O3 -march=native
endif

STATIC ?= 0
ifeq ($(STATIC), 1)
	LDFLAGS += -static
endif

all: $(BIN)

$(BIN) : $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS) $(CFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean :
	@rm -f $(OBJ) $(BIN)
