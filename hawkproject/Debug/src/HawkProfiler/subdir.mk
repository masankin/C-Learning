################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkProfiler/HawkLnxProfile.cpp \
../src/HawkProfiler/HawkProfiler.cpp \
../src/HawkProfiler/HawkWinProfile.cpp 

OBJS += \
./src/HawkProfiler/HawkLnxProfile.o \
./src/HawkProfiler/HawkProfiler.o \
./src/HawkProfiler/HawkWinProfile.o 

CPP_DEPS += \
./src/HawkProfiler/HawkLnxProfile.d \
./src/HawkProfiler/HawkProfiler.d \
./src/HawkProfiler/HawkWinProfile.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkProfiler/%.o: ../src/HawkProfiler/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


