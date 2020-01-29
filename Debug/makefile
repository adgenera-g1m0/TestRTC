################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32L0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
TestRTC.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
TestRTC.list \

OBJCOPY_HEX += \
TestRTC.hex \

OBJCOPY_BIN += \
TestRTC.bin \


# All Target
all: TestRTC.elf secondary-outputs

# Tool invocations
TestRTC.elf: $(OBJS) $(USER_OBJS) D:\jar0d\Documents\Adgenera\Atollic\ Workspace\TestRTC\STM32L053R8TX_FLASH.ld
	arm-none-eabi-gcc -o "TestRTC.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0plus -T"D:\jar0d\Documents\Adgenera\Atollic Workspace\TestRTC\STM32L053R8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="TestRTC.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

TestRTC.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "TestRTC.list"
	@echo 'Finished building: $@'
	@echo ' '

TestRTC.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "TestRTC.hex"
	@echo 'Finished building: $@'
	@echo ' '

TestRTC.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "TestRTC.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
