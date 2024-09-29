CC = gcc
CFLAGS = -I../include
OBJDIR = ../obj
SRCDIR = ../src
BINDIR = ../bin
TARGET = $(BINDIR)/coin_flip_random_number

OBJS = $(OBJDIR)/main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJS) -lncurses -o $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)
