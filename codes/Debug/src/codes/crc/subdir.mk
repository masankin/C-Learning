################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/codes/crc/crc64.c 

OBJS += \
./src/codes/crc/crc64.o 

C_DEPS += \
./src/codes/crc/crc64.d 


# Each subdirectory must supply rules for building sources it contributes
src/codes/crc/%.o: ../src/codes/crc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

