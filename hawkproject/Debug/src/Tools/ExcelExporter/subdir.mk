################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Tools/ExcelExporter/ExcelExporter.cpp \
../src/Tools/ExcelExporter/ExcelExporterDlg.cpp \
../src/Tools/ExcelExporter/IllusionExcelFile.cpp \
../src/Tools/ExcelExporter/stdafx.cpp 

OBJS += \
./src/Tools/ExcelExporter/ExcelExporter.o \
./src/Tools/ExcelExporter/ExcelExporterDlg.o \
./src/Tools/ExcelExporter/IllusionExcelFile.o \
./src/Tools/ExcelExporter/stdafx.o 

CPP_DEPS += \
./src/Tools/ExcelExporter/ExcelExporter.d \
./src/Tools/ExcelExporter/ExcelExporterDlg.d \
./src/Tools/ExcelExporter/IllusionExcelFile.d \
./src/Tools/ExcelExporter/stdafx.d 


# Each subdirectory must supply rules for building sources it contributes
src/Tools/ExcelExporter/%.o: ../src/Tools/ExcelExporter/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


