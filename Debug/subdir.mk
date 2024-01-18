################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ADC_VIN.c \
../ASCLIN_UART.c \
../Cpu0_Main.c \
../Cpu1_Main.c \
../Cpu2_Main.c \
../Cpu3_Main.c \
../Cpu4_Main.c \
../Cpu5_Main.c \
../GPIO_Init.c \
../I2C_Read_EC.c \
../MCMCAN.c \
../SPI_CPU.c 

COMPILED_SRCS += \
./ADC_VIN.src \
./ASCLIN_UART.src \
./Cpu0_Main.src \
./Cpu1_Main.src \
./Cpu2_Main.src \
./Cpu3_Main.src \
./Cpu4_Main.src \
./Cpu5_Main.src \
./GPIO_Init.src \
./I2C_Read_EC.src \
./MCMCAN.src \
./SPI_CPU.src 

C_DEPS += \
./ADC_VIN.d \
./ASCLIN_UART.d \
./Cpu0_Main.d \
./Cpu1_Main.d \
./Cpu2_Main.d \
./Cpu3_Main.d \
./Cpu4_Main.d \
./Cpu5_Main.d \
./GPIO_Init.d \
./I2C_Read_EC.d \
./MCMCAN.d \
./SPI_CPU.d 

OBJS += \
./ADC_VIN.o \
./ASCLIN_UART.o \
./Cpu0_Main.o \
./Cpu1_Main.o \
./Cpu2_Main.o \
./Cpu3_Main.o \
./Cpu4_Main.o \
./Cpu5_Main.o \
./GPIO_Init.o \
./I2C_Read_EC.o \
./MCMCAN.o \
./SPI_CPU.o 


# Each subdirectory must supply rules for building sources it contributes
%.src: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 -D__CPU__=tc38x "-fC:/Intel_zeekr_mcu_codebase/TC387_Zeekr_CodeBase_A1_continue/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

%.o: ./%.src subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean--2e-

clean--2e-:
	-$(RM) ./ADC_VIN.d ./ADC_VIN.o ./ADC_VIN.src ./ASCLIN_UART.d ./ASCLIN_UART.o ./ASCLIN_UART.src ./Cpu0_Main.d ./Cpu0_Main.o ./Cpu0_Main.src ./Cpu1_Main.d ./Cpu1_Main.o ./Cpu1_Main.src ./Cpu2_Main.d ./Cpu2_Main.o ./Cpu2_Main.src ./Cpu3_Main.d ./Cpu3_Main.o ./Cpu3_Main.src ./Cpu4_Main.d ./Cpu4_Main.o ./Cpu4_Main.src ./Cpu5_Main.d ./Cpu5_Main.o ./Cpu5_Main.src ./GPIO_Init.d ./GPIO_Init.o ./GPIO_Init.src ./I2C_Read_EC.d ./I2C_Read_EC.o ./I2C_Read_EC.src ./MCMCAN.d ./MCMCAN.o ./MCMCAN.src ./SPI_CPU.d ./SPI_CPU.o ./SPI_CPU.src

.PHONY: clean--2e-

