################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkWin32/source/detours/creatwth.cpp \
../src/HawkWin32/source/detours/detours.cpp \
../src/HawkWin32/source/detours/disasm.cpp \
../src/HawkWin32/source/detours/image.cpp \
../src/HawkWin32/source/detours/modules.cpp \
../src/HawkWin32/source/detours/uimports.cpp 

OBJS += \
./src/HawkWin32/source/detours/creatwth.o \
./src/HawkWin32/source/detours/detours.o \
./src/HawkWin32/source/detours/disasm.o \
./src/HawkWin32/source/detours/image.o \
./src/HawkWin32/source/detours/modules.o \
./src/HawkWin32/source/detours/uimports.o 

CPP_DEPS += \
./src/HawkWin32/source/detours/creatwth.d \
./src/HawkWin32/source/detours/detours.d \
./src/HawkWin32/source/detours/disasm.d \
./src/HawkWin32/source/detours/image.d \
./src/HawkWin32/source/detours/modules.d \
./src/HawkWin32/source/detours/uimports.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkWin32/source/detours/%.o: ../src/HawkWin32/source/detours/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


