################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/gpt_lld.c \
../ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/icu_lld.c \
../ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/pwm_lld.c \
../ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/st_lld.c 

OBJS += \
./ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/gpt_lld.o \
./ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/icu_lld.o \
./ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/pwm_lld.o \
./ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/st_lld.o 

C_DEPS += \
./ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/gpt_lld.d \
./ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/icu_lld.d \
./ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/pwm_lld.d \
./ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/st_lld.d 


# Each subdirectory must supply rules for building sources it contributes
ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/%.o: ../ChibiOS_3.0.2/os/hal/ports/STM32/LLD/TIMv1/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\common\ports\ARMCMx\devices\STM32F4xx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\ext\CMSIS\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\ext\CMSIS\ST" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\ports\ARMCMx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\ports\ARMCMx\compilers\GCC" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\osal\rt" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\common\ARMCMx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\STM32F4xx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\DACv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\GPIOv2" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\I2Cv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\OTGv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\RTCv2" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\SPIv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\TIMv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\USARTv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\various" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\lib\streams" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\mcconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\appconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\hwconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\applications" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\nrf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\libcanard" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\libcanard\dsdl" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\imu" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\imu\BMI160_driver" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\compression" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\blackmagic" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\blackmagic\target" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\ext\stdperiph_stm32f4\inc" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


