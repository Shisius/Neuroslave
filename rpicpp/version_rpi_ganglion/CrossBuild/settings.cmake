# Set system & compiler
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Flags
#set(CPU_FLAGS "")#"-mtune=cortex-a53")# -march=armv8-a -mfpu=neon-fp-armv8 -mneon-for-64bits -mfloat-abi=hard ") 
#set(PLATFORM_FLAGS ${CPU_FLAGS})
