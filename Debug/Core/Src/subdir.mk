################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Flash.c \
../Core/Src/I2C_no_interupt.c \
../Core/Src/Init_stm.c \
../Core/Src/MPU6050.c \
../Core/Src/main.c \
../Core/Src/sound.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/Flash.o \
./Core/Src/I2C_no_interupt.o \
./Core/Src/Init_stm.o \
./Core/Src/MPU6050.o \
./Core/Src/main.o \
./Core/Src/sound.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/Flash.d \
./Core/Src/I2C_no_interupt.d \
./Core/Src/Init_stm.d \
./Core/Src/MPU6050.d \
./Core/Src/main.d \
./Core/Src/sound.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -c -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Flash.cyclo ./Core/Src/Flash.d ./Core/Src/Flash.o ./Core/Src/Flash.su ./Core/Src/I2C_no_interupt.cyclo ./Core/Src/I2C_no_interupt.d ./Core/Src/I2C_no_interupt.o ./Core/Src/I2C_no_interupt.su ./Core/Src/Init_stm.cyclo ./Core/Src/Init_stm.d ./Core/Src/Init_stm.o ./Core/Src/Init_stm.su ./Core/Src/MPU6050.cyclo ./Core/Src/MPU6050.d ./Core/Src/MPU6050.o ./Core/Src/MPU6050.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/sound.cyclo ./Core/Src/sound.d ./Core/Src/sound.o ./Core/Src/sound.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

