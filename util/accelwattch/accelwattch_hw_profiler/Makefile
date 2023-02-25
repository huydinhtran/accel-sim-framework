TARGET = measureGpuPower

CC := g++
NVML_LIB_DIR := -L$(CUDA_INSTALL_PATH)/lib64/stubs/
CFLAGS  := -I$(CUDA_INSTALL_PATH)/include/ -pthread
LDFLAGS := $(NVML_LIB_DIR) -lnvidia-ml 
OBJECTS := bricklet_voltage_current_v2.o ip_connection.o 


$(TARGET): $(TARGET).cpp $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $< -o $@ $(LDFLAGS) 

%.o : %.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@ 


clean:
	rm -rf $(TARGET) *.o
