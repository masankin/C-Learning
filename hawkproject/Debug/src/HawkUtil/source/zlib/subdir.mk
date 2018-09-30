################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/HawkUtil/source/zlib/adler32.c \
../src/HawkUtil/source/zlib/compress.c \
../src/HawkUtil/source/zlib/crc32.c \
../src/HawkUtil/source/zlib/deflate.c \
../src/HawkUtil/source/zlib/infback.c \
../src/HawkUtil/source/zlib/inffast.c \
../src/HawkUtil/source/zlib/inflate.c \
../src/HawkUtil/source/zlib/inftrees.c \
../src/HawkUtil/source/zlib/ioapi.c \
../src/HawkUtil/source/zlib/trees.c \
../src/HawkUtil/source/zlib/uncompr.c \
../src/HawkUtil/source/zlib/unzip.c \
../src/HawkUtil/source/zlib/zutil.c 

OBJS += \
./src/HawkUtil/source/zlib/adler32.o \
./src/HawkUtil/source/zlib/compress.o \
./src/HawkUtil/source/zlib/crc32.o \
./src/HawkUtil/source/zlib/deflate.o \
./src/HawkUtil/source/zlib/infback.o \
./src/HawkUtil/source/zlib/inffast.o \
./src/HawkUtil/source/zlib/inflate.o \
./src/HawkUtil/source/zlib/inftrees.o \
./src/HawkUtil/source/zlib/ioapi.o \
./src/HawkUtil/source/zlib/trees.o \
./src/HawkUtil/source/zlib/uncompr.o \
./src/HawkUtil/source/zlib/unzip.o \
./src/HawkUtil/source/zlib/zutil.o 

C_DEPS += \
./src/HawkUtil/source/zlib/adler32.d \
./src/HawkUtil/source/zlib/compress.d \
./src/HawkUtil/source/zlib/crc32.d \
./src/HawkUtil/source/zlib/deflate.d \
./src/HawkUtil/source/zlib/infback.d \
./src/HawkUtil/source/zlib/inffast.d \
./src/HawkUtil/source/zlib/inflate.d \
./src/HawkUtil/source/zlib/inftrees.d \
./src/HawkUtil/source/zlib/ioapi.d \
./src/HawkUtil/source/zlib/trees.d \
./src/HawkUtil/source/zlib/uncompr.d \
./src/HawkUtil/source/zlib/unzip.d \
./src/HawkUtil/source/zlib/zutil.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkUtil/source/zlib/%.o: ../src/HawkUtil/source/zlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


