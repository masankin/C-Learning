################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/include/Http/Http.cc \
../src/include/Http/HttpAuthorization.cc \
../src/include/Http/HttpBase.cc \
../src/include/Http/HttpRequest.cc \
../src/include/Http/HttpResponse.cc 

CC_DEPS += \
./src/include/Http/Http.d \
./src/include/Http/HttpAuthorization.d \
./src/include/Http/HttpBase.d \
./src/include/Http/HttpRequest.d \
./src/include/Http/HttpResponse.d 

OBJS += \
./src/include/Http/Http.o \
./src/include/Http/HttpAuthorization.o \
./src/include/Http/HttpBase.o \
./src/include/Http/HttpRequest.o \
./src/include/Http/HttpResponse.o 


# Each subdirectory must supply rules for building sources it contributes
src/include/Http/%.o: ../src/include/Http/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


