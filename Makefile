# See LICENSE file for copyright and license details.

include config.mk

all: example

example: txf.o txf_draw.o example.o

.c.o: config.mk
	@echo CC -c $<
	@${CC} -c $< ${CFLAGS}

example txf_example_path:
	@echo CC -o $@
	@${CC} -o $@ $+ ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f example *.o

update: clean example
