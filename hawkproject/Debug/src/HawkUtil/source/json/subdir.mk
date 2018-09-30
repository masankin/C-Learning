################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkUtil/source/json/json_reader.cpp \
../src/HawkUtil/source/json/json_value.cpp \
../src/HawkUtil/source/json/json_writer.cpp 

OBJS += \
./src/HawkUtil/source/json/json_reader.o \
./src/HawkUtil/source/json/json_value.o \
./src/HawkUtil/source/json/json_writer.o 

CPP_DEPS += \
./src/HawkUtil/source/json/json_reader.d \
./src/HawkUtil/source/json/json_value.d \
./src/HawkUtil/source/json/json_writer.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkUtil/source/json/%.o: ../src/HawkUtil/source/json/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


