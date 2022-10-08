SRC_DIR ?= src/
INC_DIR ?= include/
OBJ_DIR ?= tmp/
BIN_DIR ?= bin/

EXEC_NAME ?= server
TARGET_EXEC ?= $(BIN_DIR)$(EXEC_NAME)
PREFIX ?= /usr/local/bin/

CFLAGS = -c -g -I$(INC_DIR) -I$(SRC) -Wall -O0

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
LDFLAGS = -o $(TARGET_EXEC) -g

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

default:
	mkdir -p tmp bin
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
