################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Plugin/lib_mysqludf_json/lib_mysqludf_json.c 

OBJS += \
./src/Plugin/lib_mysqludf_json/lib_mysqludf_json.o 

C_DEPS += \
./src/Plugin/lib_mysqludf_json/lib_mysqludf_json.d 


# Each subdirectory must supply rules for building sources it contributes
src/Plugin/lib_mysqludf_json/%.o: ../src/Plugin/lib_mysqludf_json/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


