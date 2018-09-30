################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/example/simplereader/simplereader.cpp 

OBJS += \
./src/example/simplereader/simplereader.o 

CPP_DEPS += \
./src/example/simplereader/simplereader.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/simplereader/%.o: ../src/example/simplereader/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


