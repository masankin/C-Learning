################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Plugin/lib_mysqludf_redis/lib_mysqludf_redis.c \
../src/Plugin/lib_mysqludf_redis/utils.c 

OBJS += \
./src/Plugin/lib_mysqludf_redis/lib_mysqludf_redis.o \
./src/Plugin/lib_mysqludf_redis/utils.o 

C_DEPS += \
./src/Plugin/lib_mysqludf_redis/lib_mysqludf_redis.d \
./src/Plugin/lib_mysqludf_redis/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/Plugin/lib_mysqludf_redis/%.o: ../src/Plugin/lib_mysqludf_redis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


