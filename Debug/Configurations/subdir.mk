################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Configurations/Ifx_Cfg_Ssw.c \
../Configurations/Ifx_Cfg_SswBmhd.c 

COMPILED_SRCS += \
./Configurations/Ifx_Cfg_Ssw.src \
./Configurations/Ifx_Cfg_SswBmhd.src 

C_DEPS += \
./Configurations/Ifx_Cfg_Ssw.d \
./Configurations/Ifx_Cfg_SswBmhd.d 

OBJS += \
./Configurations/Ifx_Cfg_Ssw.o \
./Configurations/Ifx_Cfg_SswBmhd.o 


# Each subdirectory must supply rules for building sources it contributes
Configurations/%.src: ../Configurations/%.c Configurations/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 -D__CPU__=tc38x "-fC:/Intel_zeekr_mcu_codebase/TC387_Zeekr_CodeBase_A1_continue/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/Configurations\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

Configurations/%.o: ./Configurations/%.src Configurations/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Configurations

clean-Configurations:
	-$(RM) ./Configurations/Ifx_Cfg_Ssw.d ./Configurations/Ifx_Cfg_Ssw.o ./Configurations/Ifx_Cfg_Ssw.src ./Configurations/Ifx_Cfg_SswBmhd.d ./Configurations/Ifx_Cfg_SswBmhd.o ./Configurations/Ifx_Cfg_SswBmhd.src

.PHONY: clean-Configurations

