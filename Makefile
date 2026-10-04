CC = gcc
LIB_DIR = /home/pi/lib
LIB_PATH = /usr/local

CFLAGS = -Wall -Wextra -O2 -fPIC\
         -I$(LIB_DIR) -I$(LIB_DIR)/MPU6050\
         -I$(LIB_DIR)/max7219_led\
         -I$(LIB_DIR)/rgb_led\
         -I$(LIB_DIR)/ds3231

LDFLAGS = -shared

# Output library name
LIB_NAME = librpidev.so

# Source files using absolute paths
SRCS = $(LIB_DIR)/MPU6050/mpu6050.c\
       $(LIB_DIR)/max7219_led/max7219_led.c\
       $(LIB_DIR)/rgb_led/rgb_led.c\
       $(LIB_DIR)/ds3231/ds3231.c\
       $(LIB_DIR)/max7219_matrix/max7219_matrix.c


OBJS = $(SRCS:.c=.o)

all: $(LIB_NAME)

$(LIB_NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Install: copy library and headers to system directories, then update dynamic linker cache
install: $(LIB_NAME)
	cp $(LIB_NAME) $(LIB_PATH)/lib/
	cp $(LIB_DIR)/MPU6050/mpu6050.h $(LIB_PATH)/include/
	cp $(LIB_DIR)/max7219_led/max7219_led.h $(LIB_PATH)/include/
	cp $(LIB_DIR)/rgb_led/rgb_led.h $(LIB_PATH)/include/
	cp $(LIB_DIR)/ds3231/ds3231.h $(LIB_PATH)/include/
	cp $(LIB_DIR)/max7219_matrix/max7219_matrix.h $(LIB_PATH)/include/

	ldconfig

# Uninstall: remove installed library and headers from system directories
uninstall:
	rm -f $(LIB_PATH)/lib/$(LIB_NAME)
	rm -f $(LIB_PATH)/include/mpu6050.h
	rm -f $(LIB_PATH)/include/max7219_led.h
	rm -f $(LIB_PATH)/include/rgb_led.h
	rm -f $(LIB_PATH)/include/ds3231.h
	rm -f $(LIB_PATH)/include/max7219_matrix.h

	ldconfig

# Clean build artifacts
clean:
	rm -f $(OBJS) $(LIB_NAME)
