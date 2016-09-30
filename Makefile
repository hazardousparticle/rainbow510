################################################################################
# rainbow 510 release make file
################################################################################

C_UPPER_SRCS := 
CXX_SRCS := 
C++_SRCS := 
OBJ_SRCS := 
CC_SRCS := 
ASM_SRCS := 
CPP_SRCS := 
C_SRCS := 
O_SRCS := 
S_UPPER_SRCS := 
CC_DEPS := 
C++_DEPS := 
EXECUTABLES := rainbow510
C_UPPER_DEPS := 
CXX_DEPS := 
OBJS := 
CPP_DEPS := 
C_DEPS := 
CC_FLAGS :=
CC_PREFIX :=
CC_INC_DIRS :=

# Every subdirectory with source files must be described here
SUBDIRS := \
. \

USER_OBJS :=
LIBS := -lusb-1.0


RM := rm -rf


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

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Color.cpp \
../KB_HID_access.cpp \
../LogitechLED.cpp \
../hidapi_mod.cpp \
../main.cpp 

OBJS += \
./Color.o \
./KB_HID_access.o \
./LogitechLED.o \
./hidapi_mod.o \
./main.o 

CPP_DEPS += \
./Color.d \
./KB_HID_access.d \
./LogitechLED.d \
./hidapi_mod.d \
./main.d 

CC_INC_DIRS += \
-I"./inc"


# Each subdirectory must supply rules for building sources it contributes
%.o: ./%.cpp
	@echo -e '\e[34mBuilding file: $<\e[0m'
	@echo 'Invoking: GCC C++ Compiler'
	$(CC_PREFIX)g++ $(CC_FLAGS) -std=c++0x  $(CC_INC_DIRS) -O2 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo -e '\e[92mFinished building: $<\e[0m'
	@echo ' '

# Add inputs and outputs from these tool invocations to the build variables 

# Tool invocations
rainbow510: $(OBJS) $(USER_OBJS)
	@echo -e '\e[35mBuilding target: $@\e[0m'
	@echo 'Invoking: GCC C++ Linker'
	$(CC_PREFIX)g++ $(CC_FLAGS) -o "$(EXECUTABLES)" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo -e '\e[92mFinished building target: $@\e[0m'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS) $(C++_DEPS) $(EXECUTABLES) $(C_UPPER_DEPS) $(CXX_DEPS) $(OBJS) $(CPP_DEPS) $(C_DEPS) 
	-@echo ' '

install:
	strip $(EXECUTABLES)
	cp $(EXECUTABLES) /usr/bin/
	cp 44-g510.rules /etc/udev/rules.d/

.PHONY: all clean dependents
.SECONDARY:
