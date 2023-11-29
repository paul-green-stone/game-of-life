CC			:= gcc
LD			:= ld
CFLAGS 		:= -g -c
ALL_CFLAGS 	:= -Wall -Wextra -pedantic-errors -O2 `pkg-config --cflags --libs sdl2` `pkg-config --cflags --libs SDL2_image` `pkg-config --cflags --libs SDL2_ttf`

LDFLAGS		:= `pkg-config --cflags --libs sdl2` `pkg-config --cflags --libs SDL2_image` `pkg-config --cflags --libs SDL2_ttf` liblilen.a

PROG		:= a

OBJDIR		:= objects
OBJS		:= $(addprefix $(OBJDIR)/, main.o world.o array.o)

INCLUDE		:= source/include.h
MAIN		:= main.c

# ================================================================ #
# World module
WORLD		:= $(addprefix source/World/, world.c world.h)

# ================================================================ #
# array module
ARRAY		:= $(addprefix source/, array.c array.h)

# ================================================================ #
# file module
FILE		:= $(addprefix source/, file.c file.h)

# ================================================================ #

$(PROG): $(OBJS)
	$(CC) -o $@.out $^ $(LDFLAGS)

$(OBJDIR)/main.o: $(MAIN) $(INCLUDE)
	$(CC) -o $@ $(CFLAGS) $(ALL_CFLAGS) $<

# ================================================================ #
# World module
$(OBJDIR)/world.o: $(WORLD) $(INCLUDE)
	$(CC) -o $@ $(CFLAGS) $(ALL_CFLAGS) $<

# ================================================================ #
# array module
$(OBJDIR)/array.o: $(ARRAY) $(INCLUDE)
	$(CC) -o $@ $(CFLAGS) $(ALL_CFLAGS) $<

# ================================================================ #
# file module
$(OBJDIR)/file.o: $(FILE) $(INCLUDE)
	$(CC) -o $@ $(CFLAGS) $(ALL_CFLAGS) $<

$(shell mkdir -p $(OBJDIR))

# ================================ #

.PHONY: clean

clean:
	rm -rf $(OBJDIR) *.out