CC        := mpicc
CFLAGS    := -std=c99 -O3 -Wall -Wextra
DEPEND    := -MMD
LIBS      := -lm
INCLUDES  := -I../../include
SRCSDIR   := ../../src/sdecomp
OBJSDIR   := obj/sdecomp
SRCS      := $(foreach dir, $(shell find $(SRCSDIR) -type d), $(wildcard $(dir)/*.c))
OBJS      := $(addprefix $(OBJSDIR)/, $(subst $(SRCSDIR)/,,$(SRCS:.c=.o)))
DEPS      := $(addprefix $(OBJSDIR)/, $(subst $(SRCSDIR)/,,$(SRCS:.c=.d)))
TARGET    := a.out

help:
	@echo "all   : create \"$(TARGET)\""
	@echo "clean : remove \"$(TARGET)\" and object files \"$(OBJSDIR)/*.o\""
	@echo "help  : show this help message"

all: $(TARGET)

$(TARGET): $(OBJS) obj/main.o
	$(CC) $(CFLAGS) $(DEPEND) -o $@ $^ $(LIBS)

$(OBJSDIR)/%.o: $(SRCSDIR)/%.c
	@if [ ! -e `dirname $@` ]; then \
		mkdir -p `dirname $@`; \
	fi
	$(CC) $(CFLAGS) $(DEPEND) $(INCLUDES) -c $< -o $@

obj/main.o: main.c
	$(CC) $(CFLAGS) $(DEPEND) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -r obj $(TARGET)

-include $(DEPS)

.PHONY : help all clean

