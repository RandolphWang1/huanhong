TARGET = query_server
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c = .o)
CC = arm-linux-gcc

query_server : $(OBJS)
	$(CC) $^ -o $@ -I ./include  -lcurl -lexpat -L ../libs/ 
%.o : %.c
	$(CC) -c $<
clean:
	@rm -vf $(TARGET) *.o *~

