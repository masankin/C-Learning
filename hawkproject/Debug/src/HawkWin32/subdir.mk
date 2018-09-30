################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkWin32/HawkCrashReport.cpp \
../src/HawkWin32/HawkDetour.cpp \
../src/HawkWin32/HawkMemLeakDetect.cpp \
../src/HawkWin32/HawkWin32.cpp 

OBJS += \
./src/HawkWin32/HawkCrashReport.o \
./src/HawkWin32/HawkDetour.o \
./src/HawkWin32/HawkMemLeakDetect.o \
./src/HawkWin32/HawkWin32.o 

CPP_DEPS += \
./src/HawkWin32/HawkCrashReport.d \
./src/HawkWin32/HawkDetour.d \
./src/HawkWin32/HawkMemLeakDetect.d \
./src/HawkWin32/HawkWin32.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkWin32/%.o: ../src/HawkWin32/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


