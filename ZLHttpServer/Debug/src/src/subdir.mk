################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/src/ActiveSocket.cpp \
../src/src/HttpKeyValue.cpp \
../src/src/HttpRequest.cpp \
../src/src/HttpResponse.cpp \
../src/src/HttpServer.cpp \
../src/src/Location.cpp \
../src/src/Log.cpp \
../src/src/PassiveSocket.cpp \
../src/src/Socket.cpp \
../src/src/TcpServer.cpp \
../src/src/Utils.cpp \
../src/src/main.cpp 

OBJS += \
./src/src/ActiveSocket.o \
./src/src/HttpKeyValue.o \
./src/src/HttpRequest.o \
./src/src/HttpResponse.o \
./src/src/HttpServer.o \
./src/src/Location.o \
./src/src/Log.o \
./src/src/PassiveSocket.o \
./src/src/Socket.o \
./src/src/TcpServer.o \
./src/src/Utils.o \
./src/src/main.o 

CPP_DEPS += \
./src/src/ActiveSocket.d \
./src/src/HttpKeyValue.d \
./src/src/HttpRequest.d \
./src/src/HttpResponse.d \
./src/src/HttpServer.d \
./src/src/Location.d \
./src/src/Log.d \
./src/src/PassiveSocket.d \
./src/src/Socket.d \
./src/src/TcpServer.d \
./src/src/Utils.d \
./src/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/src/%.o: ../src/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


