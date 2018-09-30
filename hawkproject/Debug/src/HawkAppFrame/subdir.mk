################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkAppFrame/HawkAppFrame.cpp \
../src/HawkAppFrame/HawkAppGateProxy.cpp \
../src/HawkAppFrame/HawkAppGateway.cpp 

OBJS += \
./src/HawkAppFrame/HawkAppFrame.o \
./src/HawkAppFrame/HawkAppGateProxy.o \
./src/HawkAppFrame/HawkAppGateway.o 

CPP_DEPS += \
./src/HawkAppFrame/HawkAppFrame.d \
./src/HawkAppFrame/HawkAppGateProxy.d \
./src/HawkAppFrame/HawkAppGateway.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkAppFrame/%.o: ../src/HawkAppFrame/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


