##
# flash Make
# Copyright(c) 2012 King Pearl LLC
# MIT Licensed
##
SHELL := /bin/bash

PREFIX = /usr/local
SRC = src/flash.c deps/commander/commander.c
OBJ = $(SRC:.c=.o)
REPO = kingpearl/flash
CFLAGS = -std=c99 -I deps/

flash: $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

install: flash
	cp -f flash $(PREFIX)/bin/flash

uninstall:
	rm -f $(PREFIX)/bin/flash

clean:
	rm -f flash $(OBJ)

push:
	rm -fR .git
	git init
	git add .
	git commit -m "Initial release"
	git remote add origin gh:$(REPO).git
	git push origin master

.PHONY: clean install uninstall