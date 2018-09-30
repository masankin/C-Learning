################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cJSON.c \
../src/cJSON_Utils.c \
../src/test.c \
../src/test_utils.c 

OBJS += \
./src/cJSON.o \
./src/cJSON_Utils.o \
./src/test.o \
./src/test_utils.o 

C_DEPS += \
./src/cJSON.d \
./src/cJSON_Utils.d \
./src/test.d \
./src/test_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


