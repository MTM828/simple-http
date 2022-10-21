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
LDFLAGS = -o $(TARGET_EXEC) -g

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

default:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)
	$(CC) $(SRC_DIR)main.c $(CFLAGS) -o $(OBJ_DIR)main.o
	$(CC) $(SRC_DIR)ansi-colors.c $(CFLAGS) -o $(OBJ_DIR)ansi-colors.o
	$(CC) $(SRC_DIR)util.c $(CFLAGS) -o $(OBJ_DIR)util.o
	$(CC) $(SRC_DIR)log.c $(CFLAGS) -o $(OBJ_DIR)log.o
	$(CC) $(OBJ_DIR)*.o $(LDFLAGS)

install:
	cp $(TARGET_EXEC) $(PREFIX)$(EXEC_NAME)

uninstall:
	rm $(PREFIX)$(EXEC_NAME)

clean:
	rm $(OBJ_DIR)*.o
