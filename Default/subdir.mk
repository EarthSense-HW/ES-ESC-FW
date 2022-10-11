################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board.c \
../buffer.c \
../comm_can.c \
../comm_usb.c \
../comm_usb_serial.c \
../commands.c \
../conf_general.c \
../confgenerator.c \
../crc.c \
../digital_filter.c \
../eeprom.c \
../encoder.c \
../flash_helper.c \
../gpdrive.c \
../i2c_bb.c \
../irq_handlers.c \
../led_external.c \
../ledpwm.c \
../main.c \
../mc_interface.c \
../mcpwm.c \
../mcpwm_foc.c \
../packet.c \
../servo_dec.c \
../servo_simple.c \
../shutdown.c \
../terminal.c \
../timeout.c \
../timer.c \
../utils.c \
../virtual_motor.c \
../ws2811.c 

OBJS += \
./board.o \
./buffer.o \
./comm_can.o \
./comm_usb.o \
./comm_usb_serial.o \
./commands.o \
./conf_general.o \
./confgenerator.o \
./crc.o \
./digital_filter.o \
./eeprom.o \
./encoder.o \
./flash_helper.o \
./gpdrive.o \
./i2c_bb.o \
./irq_handlers.o \
./led_external.o \
./ledpwm.o \
./main.o \
./mc_interface.o \
./mcpwm.o \
./mcpwm_foc.o \
./packet.o \
./servo_dec.o \
./servo_simple.o \
./shutdown.o \
./terminal.o \
./timeout.o \
./timer.o \
./utils.o \
./virtual_motor.o \
./ws2811.o 

C_DEPS += \
./board.d \
./buffer.d \
./comm_can.d \
./comm_usb.d \
./comm_usb_serial.d \
./commands.d \
./conf_general.d \
./confgenerator.d \
./crc.d \
./digital_filter.d \
./eeprom.d \
./encoder.d \
./flash_helper.d \
./gpdrive.d \
./i2c_bb.d \
./irq_handlers.d \
./led_external.d \
./ledpwm.d \
./main.d \
./mc_interface.d \
./mcpwm.d \
./mcpwm_foc.d \
./packet.d \
./servo_dec.d \
./servo_simple.d \
./shutdown.d \
./terminal.d \
./timeout.d \
./timer.d \
./utils.d \
./virtual_motor.d \
./ws2811.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\common\ports\ARMCMx\devices\STM32F4xx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\ext\CMSIS\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\ext\CMSIS\ST" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\ports\ARMCMx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\ports\ARMCMx\compilers\GCC" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\osal\rt" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\common\ARMCMx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\STM32F4xx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\DACv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\GPIOv2" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\I2Cv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\OTGv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\RTCv2" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\SPIv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\TIMv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\USARTv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\various" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\lib\streams" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\mcconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\appconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\hwconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\applications" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\nrf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\libcanard" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\libcanard\dsdl" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\imu" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\imu\BMI160_driver" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\compression" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\blackmagic" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\blackmagic\target" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\ext\stdperiph_stm32f4\inc" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


