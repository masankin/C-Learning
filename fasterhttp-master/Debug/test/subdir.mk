################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../test/press.c \
../test/test_client_block.c \
../test/test_client_gzip.c \
../test/test_client_nonblock.c \
../test/test_client_nonblock_slow.c \
../test/test_client_stickrequests.c \
../test/test_demo.c \
../test/test_failure.c \
../test/test_server_block.c \
../test/test_server_gzip.c \
../test/test_server_nonblock.c \
../test/test_server_nonblock_slow.c \
../test/test_success.c 

OBJS += \
./test/press.o \
./test/test_client_block.o \
./test/test_client_gzip.o \
./test/test_client_nonblock.o \
./test/test_client_nonblock_slow.o \
./test/test_client_stickrequests.o \
./test/test_demo.o \
./test/test_failure.o \
./test/test_server_block.o \
./test/test_server_gzip.o \
./test/test_server_nonblock.o \
./test/test_server_nonblock_slow.o \
./test/test_success.o 

C_DEPS += \
./test/press.d \
./test/test_client_block.d \
./test/test_client_gzip.d \
./test/test_client_nonblock.d \
./test/test_client_nonblock_slow.d \
./test/test_client_stickrequests.d \
./test/test_demo.d \
./test/test_failure.d \
./test/test_server_block.d \
./test/test_server_gzip.d \
./test/test_server_nonblock.d \
./test/test_server_nonblock_slow.d \
./test/test_success.d 


# Each subdirectory must supply rules for building sources it contributes
test/%.o: ../test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


