################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/HawkUtil/source/nedmalloc/nedmalloc.c 

OBJS += \
./src/HawkUtil/source/nedmalloc/nedmalloc.o 

C_DEPS += \
./src/HawkUtil/source/nedmalloc/nedmalloc.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkUtil/source/nedmalloc/%.o: ../src/HawkUtil/source/nedmalloc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


