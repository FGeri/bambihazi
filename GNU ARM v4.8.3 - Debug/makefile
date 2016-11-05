################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/efm32gg/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Bambihazi.axf

# Tool invocations
Bambihazi.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "Bambihazi.ld" -Xlinker --gc-sections -Xlinker -Map="Bambihazi.map" --specs=nano.specs -o Bambihazi.axf "./src/Bambihazi.o" "./emlib/em_system.o" "./CMSIS/efm32gg/startup_gcc_efm32gg.o" "./CMSIS/efm32gg/system_efm32gg.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: Bambihazi.hex'
	arm-none-eabi-objcopy -O ihex "Bambihazi.axf" "Bambihazi.hex"
	@echo ' '

	@echo 'Building bin file: Bambihazi.bin'
	arm-none-eabi-objcopy -O binary "Bambihazi.axf" "Bambihazi.bin"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Bambihazi.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Bambihazi.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
