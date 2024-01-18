################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Libraries/iLLD/TC38A/Tricore/_PinMap/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/_Lib/DataHandling/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/_Impl/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Stm/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Src/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Scu/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Qspi/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Qspi/SpiSlave/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Port/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Pms/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Mtu/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/I2c/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/I2c/I2c/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Gtm/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Evadc/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Evadc/Adc/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Dma/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Dma/Dma/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Cpu/Trap/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Cpu/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Cpu/Irq/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Can/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Can/Can/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Asclin/Std/subdir.mk
-include Libraries/iLLD/TC38A/Tricore/Asclin/Asc/subdir.mk
-include Libraries/Service/CpuGeneric/SysSe/Bsp/subdir.mk
-include Libraries/Service/CpuGeneric/StdIf/subdir.mk
-include Libraries/Service/CpuGeneric/If/subdir.mk
-include Libraries/Infra/Ssw/TC38A/Tricore/subdir.mk
-include Libraries/Infra/Platform/Tricore/Compilers/subdir.mk
-include Configurations/Debug/subdir.mk
-include Configurations/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := TC387_Zeekr_CodeBase_A1_continue
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
TC387_Zeekr_CodeBase_A1_continue.siz \


# All Target
all: main-build

# Main-build Target
main-build: TC387_Zeekr_CodeBase_A1_continue.elf secondary-outputs

# Tool invocations
TC387_Zeekr_CodeBase_A1_continue.elf: $(OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: TASKING Linker'
	cctc --lsl-file="../Lcf_Tasking_Tricore_Tc.lsl" -Wl-Oc -Wl-OL -Wl-Ot -Wl-Ox -Wl-Oy -Wl--map-file="TC387_Zeekr_CodeBase_A1_continue.map" -Wl-mc -Wl-mf -Wl-mi -Wl-mk -Wl-ml -Wl-mm -Wl-md -Wl-mr -Wl-mu --no-warnings= -Wl--error-limit=42 --fp-model=3 -lrt --lsl-core=vtc --exceptions --strict --anachronisms --force-c++ -Ctc38x -o"TC387_Zeekr_CodeBase_A1_continue.elf" -Wl-o"TC387_Zeekr_CodeBase_A1_continue.hex:IHEX" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

TC387_Zeekr_CodeBase_A1_continue.siz: TC387_Zeekr_CodeBase_A1_continue.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Print Size'
	elfsize  "TC387_Zeekr_CodeBase_A1_continue.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	find . ! -name "*.mk" ! -name "makefile" ! -name "." -type f -delete 
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
