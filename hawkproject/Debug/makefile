################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/Tools/StringHasher/subdir.mk
-include src/Tools/ProtocolGen/subdir.mk
-include src/Tools/ProfilerMonitor/subdir.mk
-include src/Tools/ProcMonitor/subdir.mk
-include src/Tools/NetBridge/subdir.mk
-include src/Tools/LogServer/subdir.mk
-include src/Tools/GateServer/subdir.mk
-include src/Tools/GameAnalyst/subdir.mk
-include src/Tools/FileSender/subdir.mk
-include src/Tools/FileAcceptor/subdir.mk
-include src/Tools/ExcelExporter/subdir.mk
-include src/Tools/DomainSvr/subdir.mk
-include src/Tools/DataPacker/subdir.mk
-include src/Tools/CmdSender/subdir.mk
-include src/Samples/TestCase/debug/objs/subdir.mk
-include src/Samples/TestCase/subdir.mk
-include src/Samples/EchoServer/subdir.mk
-include src/Samples/EchoClient/subdir.mk
-include src/Plugin/lib_mysqludf_redis/subdir.mk
-include src/Plugin/lib_mysqludf_json/subdir.mk
-include src/HawkWin32/source/detours/subdir.mk
-include src/HawkWin32/subdir.mk
-include src/HawkUtil/source/zlib/subdir.mk
-include src/HawkUtil/source/tinyxml/subdir.mk
-include src/HawkUtil/source/sqlite/subdir.mk
-include src/HawkUtil/source/nedmalloc/subdir.mk
-include src/HawkUtil/source/json/subdir.mk
-include src/HawkUtil/subdir.mk
-include src/HawkRedis/subdir.mk
-include src/HawkProfiler/subdir.mk
-include src/HawkMemLeak/src/subdir.mk
-include src/HawkMemLeak/subdir.mk
-include src/HawkLog/subdir.mk
-include src/HawkGeometry/subdir.mk
-include src/HawkGateway/subdir.mk
-include src/HawkAppFrame/subdir.mk
-include src/Backup/HawkLog/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: hawkproject

# Tool invocations
hawkproject: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	g++  -o "hawkproject" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(CPP_DEPS)$(C_DEPS) hawkproject
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
