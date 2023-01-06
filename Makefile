SRC_DIR ?= src/
INC_DIR ?= include/
OBJ_DIR ?= tmp/
BIN_DIR ?= bin/

EXEC_NAME ?= server
TARGET_EXEC ?= $(BIN_DIR)$(EXEC_NAME)
PREFIX ?= /usr/local/bin/

CFLAGS = -c -g -I$(INC_DIR) -I$(SRC_DIR) -Wall -O0

ifeq ($(SINGLE_PROC),1)
CFLAGS += -D SINGLE_PROC
endif

ifeq ($(NO_PYTHON),)
CFLAGS += `python3-config --cflags --embed`
endif

ifeq ($(NO_PHP),)
ifeq ($(PHP_USE_EMBED),1)
CFLAGS += -D PHP_USE_EMBED=1\
	-I /usr/local/include/php/ \
	-I /usr/local/include/php/main/ \
	-I /usr/local/include/php/Zend/ \
	-I /usr/local/include/php/TSRM/
endif
endif

LDLIBS = -lm
LDFLAGS = -g

ifeq ($(NO_PYTHON),)
LDFLAGS += `python3-config --ldflags --embed`
endif
ifeq ($(NO_PYTHON),1)
CFLAGS += -D NO_PYTHON
endif

ifeq ($(NO_PHP),)
ifeq ($(PHP_USE_EMBED),1)
LDLIBS += -lphp
endif
endif
ifeq ($(NO_PHP),1)
CFLAGS += -D NO_PHP
endif

LDFLAGS += $(LDLIBS)

.PHONY: default all install uninstall clean

default: clean $(TARGET_EXEC)

all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJ_DIR) $(BIN_DIR) \
	$(OBJ_DIR)main.o \
	$(OBJ_DIR)ansi-colors.o \
	$(OBJ_DIR)util.o \
	$(OBJ_DIR)log.o \
	$(OBJ_DIR)exec-py.o
	$(CC) $(OBJ_DIR)*.o $(LDFLAGS) -o $(TARGET_EXEC)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $< $(CFLAGS) -o $@

install:
	cp $(TARGET_EXEC) $(PREFIX)$(EXEC_NAME)

uninstall:
	rm $(PREFIX)$(EXEC_NAME)

clean:
	rm -f $(OBJ_DIR)*.o
