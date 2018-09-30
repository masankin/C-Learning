################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/example/messagereader/messagereader.cpp 

OBJS += \
./src/example/messagereader/messagereader.o 

CPP_DEPS += \
./src/example/messagereader/messagereader.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/messagereader/%.o: ../src/example/messagereader/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


