################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/include/Base/Base64.cc \
../src/include/Base/Helper.cc \
../src/include/Base/Logger.cc \
../src/include/Base/RandomString.cc \
../src/include/Base/StrTime.cc \
../src/include/Base/Timerfd.cc 

CC_DEPS += \
./src/include/Base/Base64.d \
./src/include/Base/Helper.d \
./src/include/Base/Logger.d \
./src/include/Base/RandomString.d \
./src/include/Base/StrTime.d \
./src/include/Base/Timerfd.d 

OBJS += \
./src/include/Base/Base64.o \
./src/include/Base/Helper.o \
./src/include/Base/Logger.o \
./src/include/Base/RandomString.o \
./src/include/Base/StrTime.o \
./src/include/Base/Timerfd.o 


# Each subdirectory must supply rules for building sources it contributes
src/include/Base/%.o: ../src/include/Base/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


