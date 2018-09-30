################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/binder.cpp \
../src/connection.cpp \
../src/demo.cpp \
../src/exception.cpp \
../src/result_metadata.cpp \
../src/resultset.cpp \
../src/statement.cpp 

OBJS += \
./src/binder.o \
./src/connection.o \
./src/demo.o \
./src/exception.o \
./src/result_metadata.o \
./src/resultset.o \
./src/statement.o 

CPP_DEPS += \
./src/binder.d \
./src/connection.d \
./src/demo.d \
./src/exception.d \
./src/result_metadata.d \
./src/resultset.d \
./src/statement.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


