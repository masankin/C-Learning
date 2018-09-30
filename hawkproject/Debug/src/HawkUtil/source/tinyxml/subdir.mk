################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkUtil/source/tinyxml/tinystr.cpp \
../src/HawkUtil/source/tinyxml/tinyxml.cpp \
../src/HawkUtil/source/tinyxml/tinyxmlerror.cpp \
../src/HawkUtil/source/tinyxml/tinyxmlparser.cpp 

OBJS += \
./src/HawkUtil/source/tinyxml/tinystr.o \
./src/HawkUtil/source/tinyxml/tinyxml.o \
./src/HawkUtil/source/tinyxml/tinyxmlerror.o \
./src/HawkUtil/source/tinyxml/tinyxmlparser.o 

CPP_DEPS += \
./src/HawkUtil/source/tinyxml/tinystr.d \
./src/HawkUtil/source/tinyxml/tinyxml.d \
./src/HawkUtil/source/tinyxml/tinyxmlerror.d \
./src/HawkUtil/source/tinyxml/tinyxmlparser.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkUtil/source/tinyxml/%.o: ../src/HawkUtil/source/tinyxml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


