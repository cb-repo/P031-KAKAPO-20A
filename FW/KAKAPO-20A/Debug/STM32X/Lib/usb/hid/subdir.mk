################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../STM32X/Lib/usb/hid/USB_HID.c 

OBJS += \
./STM32X/Lib/usb/hid/USB_HID.o 

C_DEPS += \
./STM32X/Lib/usb/hid/USB_HID.d 


# Each subdirectory must supply rules for building sources it contributes
STM32X/Lib/usb/hid/%.o STM32X/Lib/usb/hid/%.su STM32X/Lib/usb/hid/%.cyclo: ../STM32X/Lib/usb/hid/%.c STM32X/Lib/usb/hid/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C031xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I../STM32X/Lib -I../Modules/Utils/FPROM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-STM32X-2f-Lib-2f-usb-2f-hid

clean-STM32X-2f-Lib-2f-usb-2f-hid:
	-$(RM) ./STM32X/Lib/usb/hid/USB_HID.cyclo ./STM32X/Lib/usb/hid/USB_HID.d ./STM32X/Lib/usb/hid/USB_HID.o ./STM32X/Lib/usb/hid/USB_HID.su

.PHONY: clean-STM32X-2f-Lib-2f-usb-2f-hid

