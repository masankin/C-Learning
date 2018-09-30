################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkMemLeak/src/HawkMemLeakDetect.cpp 

OBJS += \
./src/HawkMemLeak/src/HawkMemLeakDetect.o 

CPP_DEPS += \
./src/HawkMemLeak/src/HawkMemLeakDetect.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkMemLeak/src/%.o: ../src/HawkMemLeak/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


