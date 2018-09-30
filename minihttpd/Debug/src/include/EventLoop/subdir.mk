################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/include/EventLoop/Channel.cc \
../src/include/EventLoop/IOMultiplexing.cc 

CC_DEPS += \
./src/include/EventLoop/Channel.d \
./src/include/EventLoop/IOMultiplexing.d 

OBJS += \
./src/include/EventLoop/Channel.o \
./src/include/EventLoop/IOMultiplexing.o 


# Each subdirectory must supply rules for building sources it contributes
src/include/EventLoop/%.o: ../src/include/EventLoop/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


