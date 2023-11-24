CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = simple-shell.c read_user_input.c create_process_and_run.c piped_commands.c history.c background_commands.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run bonus

shell: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) shell
