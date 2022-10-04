SRC_DIR ?= src/
INC_DIR ?= include/
OBJ_DIR ?= tmp/
BIN_DIR ?= bin/

EXEC_NAME ?= server
TARGET_EXEC ?= $(BIN_DIR)$(EXEC_NAME)
PREFIX ?= /usr/local/bin

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
LDFLAGS = -o bin/server -g

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
	$(CC) src/main.c $(CFLAGS) -o tmp/main.o
	$(CC) src/ansi-colors.c $(CFLAGS) -o tmp/ansi-colors.o
	$(CC) src/util.c $(CFLAGS) -o tmp/util.o
	$(CC) src/log.c $(CFLAGS) -o tmp/log.o
	$(CC) tmp/*.o $(LDFLAGS)

install:
	cp $(TARGET_EXEC) $(PREFIX)$(EXEC_NAME)

uninstall:
	rm $(PREFIX)$(EXEC_NAME)

clean:
	rm $(OBJ_DIR)*.o
