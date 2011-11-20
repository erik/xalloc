CSRC := $(shell find . -name "*.c")
CHDR := $(shell find . -name "*.h")

COBJ := $(CSRC:.c=.o)

CC := clang

LIBS := 

CFLAGS  := -Wall -Wextra -std=c99 -pedantic
LNFLAGS := $(LIBS)

TEST := tester

all: $(COBJ) $(CHDR)

test: $(TEST)

$(TEST): $(COBJ)
#	echo "link $(TEST)"
	$(CC) $(COBJ) $(LNFLAGS) -o$(TEST)

%.o: %.c
#	echo "   cc $<"
	$(CC) -c $(CFLAGS) $< -o $@

debug:
	$(MAKE) test "CFLAGS=$(CFLAGS) -g -O0"

clean:
	rm -f $(COBJ)

distclean: clean
	rm -rf atto/

todo:
	@find . -type f | xargs grep -n -i "TODO"
	@find . -type f | xargs grep -n -i "FIXME"

loc: 
	@wc -l ./*.[ch]

check-syntax:
	gcc -o nul -S $(CHK_SOURCES) -I $(CHDR)

# requires sloccount
sloc:
	@sloccount . | grep '(SLOC)'

.PHONY= loc sloc todo all clean distclean debug
