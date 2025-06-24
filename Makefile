TARGET=grs_fft

ifndef BUILD_DIR
	BUILD_DIR=$(CURDIR)
endif

CC=gcc
FLAGS=-fpic -std=c99 -Wall -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes
LIBS=-pthread -lm -lzmq -lfftw3

all: $(BUILD_DIR)/main.o
	$(CC) $(FLAGS) $(BUILD_DIR)/*.o -o $(BUILD_DIR)/$(TARGET) $(LIBS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@ $(LIBS)

install:
	@echo "Installing GRS FFT application..."
	install -m 0755 $(BUILD_DIR)/$(TARGET) /usr/local/bin/$(TARGET)

uninstall:
	@echo "Uninstalling GRS FFT application..."
	rm /usr/local/bin/$(TARGET)

clean:
	rm $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)
