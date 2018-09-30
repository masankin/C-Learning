################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/example/tutorial/subdir.mk
-include src/example/simplewriter/subdir.mk
-include src/example/simplereader/subdir.mk
-include src/example/simpledom/subdir.mk
-include src/example/serialize/subdir.mk
-include src/example/schemavalidator/subdir.mk
-include src/example/prettyauto/subdir.mk
-include src/example/pretty/subdir.mk
-include src/example/parsebyparts/subdir.mk
-include src/example/messagereader/subdir.mk
-include src/example/jsonx/subdir.mk
-include src/example/filterkeydom/subdir.mk
-include src/example/filterkey/subdir.mk
-include src/example/condense/subdir.mk
-include src/example/capitalize/subdir.mk
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
all: rapidjson

# Tool invocations
rapidjson: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	g++  -o "rapidjson" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(CPP_DEPS)$(C_DEPS) rapidjson
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
