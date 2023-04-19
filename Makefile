
TARGET=tritontalk
SRC_DIR=./src
CC = cc
DEBUG = -g

LDFLAGS = -lresolv -lconfig -lm

CCFLAGS = -std=c11 -fcommon -Wall -Wextra -Werror=implicit-function-declaration $(DEBUG)

# add object file names here
OBJS = main.o util.o input.o host.o sender.o receiver.o run_main.o switch.o

all: tritontalk

%.o : $(SRC_DIR)/%.c
	$(CC) -c $(CCFLAGS) $<

%.o : $(SRC_DIR)/%.cc
	$(CC) -c $(CCFLAGS) $<

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CCFLAGS) $(LDFLAGS); rm *.o 

clean:
	rm -f $(TARGET)

submit: clean
	rm -f project1.tgz; tar czvf project1.tgz *; turnin project1.tgz -c cs123f -p project1
