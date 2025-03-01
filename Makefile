CC = gcc
CFLAGS = -Wall -O2
LIBS = -lX11 -lm

SRC = src/main.c src/coin.c src/animation.c src/gui_animation.c
OBJDIR = obj
OBJ = $(OBJDIR)/main.o $(OBJDIR)/coin.o $(OBJDIR)/animation.o $(OBJDIR)/gui_animation.o
TARGET = coinfipper

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

