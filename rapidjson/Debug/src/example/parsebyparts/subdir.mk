################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/example/parsebyparts/parsebyparts.cpp 

OBJS += \
./src/example/parsebyparts/parsebyparts.o 

CPP_DEPS += \
./src/example/parsebyparts/parsebyparts.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/parsebyparts/%.o: ../src/example/parsebyparts/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


