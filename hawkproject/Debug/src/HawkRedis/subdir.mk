################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkRedis/HawkRedis.cpp \
../src/HawkRedis/HawkRedisContext.cpp \
../src/HawkRedis/HawkRedisProxy.cpp \
../src/HawkRedis/HawkRedisReplay.cpp 

OBJS += \
./src/HawkRedis/HawkRedis.o \
./src/HawkRedis/HawkRedisContext.o \
./src/HawkRedis/HawkRedisProxy.o \
./src/HawkRedis/HawkRedisReplay.o 

CPP_DEPS += \
./src/HawkRedis/HawkRedis.d \
./src/HawkRedis/HawkRedisContext.d \
./src/HawkRedis/HawkRedisProxy.d \
./src/HawkRedis/HawkRedisReplay.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkRedis/%.o: ../src/HawkRedis/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


