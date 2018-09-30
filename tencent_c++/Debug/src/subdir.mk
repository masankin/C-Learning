################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Sample.cpp \
../src/WXBizMsgCrypt.cpp \
../src/tinyxml2.cpp 

OBJS += \
./src/Sample.o \
./src/WXBizMsgCrypt.o \
./src/tinyxml2.o 

CPP_DEPS += \
./src/Sample.d \
./src/WXBizMsgCrypt.d \
./src/tinyxml2.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0   -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


