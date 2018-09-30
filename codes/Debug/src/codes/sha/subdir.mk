################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/codes/sha/sha1.c 

OBJS += \
./src/codes/sha/sha1.o 

C_DEPS += \
./src/codes/sha/sha1.d 


# Each subdirectory must supply rules for building sources it contributes
src/codes/sha/%.o: ../src/codes/sha/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


