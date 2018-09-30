################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/HawkUtil/source/sqlite/sqlite3.c 

OBJS += \
./src/HawkUtil/source/sqlite/sqlite3.o 

C_DEPS += \
./src/HawkUtil/source/sqlite/sqlite3.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkUtil/source/sqlite/%.o: ../src/HawkUtil/source/sqlite/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


