################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../STM32X/Lib/usb/mtp/MTP.c \
../STM32X/Lib/usb/mtp/MTP_FS.c \
../STM32X/Lib/usb/mtp/USB_MTP.c 

OBJS += \
./STM32X/Lib/usb/mtp/MTP.o \
./STM32X/Lib/usb/mtp/MTP_FS.o \
./STM32X/Lib/usb/mtp/USB_MTP.o 

C_DEPS += \
./STM32X/Lib/usb/mtp/MTP.d \
./STM32X/Lib/usb/mtp/MTP_FS.d \
./STM32X/Lib/usb/mtp/USB_MTP.d 


# Each subdirectory must supply rules for building sources it contributes
STM32X/Lib/usb/mtp/%.o STM32X/Lib/usb/mtp/%.su STM32X/Lib/usb/mtp/%.cyclo: ../STM32X/Lib/usb/mtp/%.c STM32X/Lib/usb/mtp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C031xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I../STM32X/Lib -I../Modules/Utils/FPROM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-STM32X-2f-Lib-2f-usb-2f-mtp

clean-STM32X-2f-Lib-2f-usb-2f-mtp:
	-$(RM) ./STM32X/Lib/usb/mtp/MTP.cyclo ./STM32X/Lib/usb/mtp/MTP.d ./STM32X/Lib/usb/mtp/MTP.o ./STM32X/Lib/usb/mtp/MTP.su ./STM32X/Lib/usb/mtp/MTP_FS.cyclo ./STM32X/Lib/usb/mtp/MTP_FS.d ./STM32X/Lib/usb/mtp/MTP_FS.o ./STM32X/Lib/usb/mtp/MTP_FS.su ./STM32X/Lib/usb/mtp/USB_MTP.cyclo ./STM32X/Lib/usb/mtp/USB_MTP.d ./STM32X/Lib/usb/mtp/USB_MTP.o ./STM32X/Lib/usb/mtp/USB_MTP.su

.PHONY: clean-STM32X-2f-Lib-2f-usb-2f-mtp

