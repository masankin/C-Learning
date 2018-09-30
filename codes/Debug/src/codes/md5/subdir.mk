################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/codes/md5/md5.c 

OBJS += \
./src/codes/md5/md5.o 

C_DEPS += \
./src/codes/md5/md5.d 


# Each subdirectory must supply rules for building sources it contributes
src/codes/md5/%.o: ../src/codes/md5/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


