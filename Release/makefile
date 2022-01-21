################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
FLASH_IMAGE += \
D0003E_Lab_1.hex \


# All Target
all: D0003E_Lab_1.elf secondary-outputs

# Tool invocations
D0003E_Lab_1.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,D0003E_Lab_1.map -mmcu=atmega169 -o"D0003E_Lab_1.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

D0003E_Lab_1.hex: D0003E_Lab_1.elf
	@echo 'Create Flash image (ihex format)'
	avr-objcopy -I ihex -O ihex D0003E_Lab_1.elf "D0003E_Lab_1.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(C_DEPS) D0003E_Lab_1.elf
	-@echo ' '

secondary-outputs: $(FLASH_IMAGE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
