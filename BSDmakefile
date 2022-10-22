SRC_DIR ?= src/
INC_DIR ?= include/
OBJ_DIR ?= tmp/
BIN_DIR ?= bin/

EXEC_NAME ?= server
TARGET_EXEC ?= $(BIN_DIR)$(EXEC_NAME)
PREFIX ?= /usr/local/bin/

CFLAGS = -c -g -I$(INC_DIR) -I$(SRC_DIR) -Wall -O0

.if SINGLE_PROC
CFLAGS += -D SINGLE_PROC=1
.endif

.ifndef NO_PYTHON
CFLAGS += `python3-config --cflags --embed`
.endif

.ifndef NO_PHP
.if PHP_USE_EMBED
CFLAGS += -D PHP_USE_EMBED=1\
	-I /usr/local/include/php/ \
	-I /usr/local/include/php/main/ \
	-I /usr/local/include/php/Zend/ \
	-I /usr/local/include/php/TSRM/
.endif
.endif

LDLIBS = -lm
LDFLAGS = -g

.ifndef NO_PYTHON
LDFLAGS += `python3-config --ldflags --embed`
.endif
.if NO_PYTHON
CFLAGS += -D NO_PYTHON=1
.endif

.ifndef NO_PHP
.if PHP_USE_EMBED
LDLIBS += -lphp
.endif
.endif
.if NO_PHP
CFLAGS += -D NO_PHP=1
.endif

LDFLAGS += $(LDLIBS)

.PHONY: default all install uninstall clean

default: $(TARGET_EXEC)

all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJ_DIR) $(BIN_DIR) \
	$(OBJ_DIR)main.o \
	$(OBJ_DIR)ansi-colors.o \
	$(OBJ_DIR)util.o \
	$(OBJ_DIR)log.o
	$(CC) $(OBJ_DIR)*.o $(LDFLAGS) -o $(TARGET_EXEC)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)main.o: $(SRC_DIR)main.c
	$(CC) $(SRC_DIR)main.c $(CFLAGS) -o $(OBJ_DIR)main.o

$(OBJ_DIR)ansi-colors.o: $(SRC_DIR)ansi-colors.c
	$(CC) $(SRC_DIR)ansi-colors.c $(CFLAGS) -o $(OBJ_DIR)ansi-colors.o

$(OBJ_DIR)util.o: $(SRC_DIR)util.c
	$(CC) $(SRC_DIR)util.c $(CFLAGS) -o $(OBJ_DIR)util.o

$(OBJ_DIR)log.o: $(SRC_DIR)log.c
	$(CC) $(SRC_DIR)log.c $(CFLAGS) -o $(OBJ_DIR)log.o

install:
	cp $(TARGET_EXEC) $(PREFIX)$(EXEC_NAME)

uninstall:
	rm $(PREFIX)$(EXEC_NAME)

clean:
	rm $(OBJ_DIR)*.o
