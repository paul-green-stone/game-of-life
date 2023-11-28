CC			:= gcc
LD			:= ld
CFLAGS 		:= -g -c
ALL_CFLAGS 	:= -Wall -Wextra -pedantic-errors -O2 `pkg-config --cflags --libs sdl2` `pkg-config --cflags --libs SDL2_image` `pkg-config --cflags --libs SDL2_ttf`

LDFLAGS		:= `pkg-config --cflags --libs sdl2` `pkg-config --cflags --libs SDL2_image` `pkg-config --cflags --libs SDL2_ttf` liblilen.a

PROG		:= a

OBJDIR		:= objects
OBJS		:= $(addprefix $(OBJDIR)/, main.o world.o)

INCLUDE		:= source/include.h
MAIN		:= main.c
WORLD		:= $(addprefix source/World/, world.c world.h)

# ================================================================ #

$(PROG): $(OBJS)
	$(CC) -o $@.out $^ $(LDFLAGS)

$(OBJDIR)/main.o: $(MAIN) $(INCLUDE)
	$(CC) -o $@ $(CFLAGS) $(ALL_CFLAGS) $<

$(OBJDIR)/world.o: $(WORLD) $(INCLUDE)
	$(CC) -o $@ $(CFLAGS) $(ALL_CFLAGS) $<

$(shell mkdir -p $(OBJDIR))

# ================================ #

.PHONY: clean

clean:
	rm -rf $(OBJDIR) *.out