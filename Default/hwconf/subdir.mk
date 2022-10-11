################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hwconf/drv8301.c \
../hwconf/drv8305.c \
../hwconf/drv8320s.c \
../hwconf/drv8323s.c \
../hwconf/hw.c \
../hwconf/hw_100_250.c \
../hwconf/hw_40.c \
../hwconf/hw_410.c \
../hwconf/hw_45.c \
../hwconf/hw_46.c \
../hwconf/hw_48.c \
../hwconf/hw_49.c \
../hwconf/hw_60.c \
../hwconf/hw_75_300.c \
../hwconf/hw_a200s_v2.c \
../hwconf/hw_axiom.c \
../hwconf/hw_axiom_fpga_bitstream.c \
../hwconf/hw_binar_v1.c \
../hwconf/hw_das_mini.c \
../hwconf/hw_das_rs.c \
../hwconf/hw_hd.c \
../hwconf/hw_luna_bbshd.c \
../hwconf/hw_mini4.c \
../hwconf/hw_r2.c \
../hwconf/hw_rd2.c \
../hwconf/hw_rh.c \
../hwconf/hw_tp.c \
../hwconf/hw_uavc_omega.c \
../hwconf/hw_uavc_qcube.c \
../hwconf/hw_victor_r1a.c 

OBJS += \
./hwconf/drv8301.o \
./hwconf/drv8305.o \
./hwconf/drv8320s.o \
./hwconf/drv8323s.o \
./hwconf/hw.o \
./hwconf/hw_100_250.o \
./hwconf/hw_40.o \
./hwconf/hw_410.o \
./hwconf/hw_45.o \
./hwconf/hw_46.o \
./hwconf/hw_48.o \
./hwconf/hw_49.o \
./hwconf/hw_60.o \
./hwconf/hw_75_300.o \
./hwconf/hw_a200s_v2.o \
./hwconf/hw_axiom.o \
./hwconf/hw_axiom_fpga_bitstream.o \
./hwconf/hw_binar_v1.o \
./hwconf/hw_das_mini.o \
./hwconf/hw_das_rs.o \
./hwconf/hw_hd.o \
./hwconf/hw_luna_bbshd.o \
./hwconf/hw_mini4.o \
./hwconf/hw_r2.o \
./hwconf/hw_rd2.o \
./hwconf/hw_rh.o \
./hwconf/hw_tp.o \
./hwconf/hw_uavc_omega.o \
./hwconf/hw_uavc_qcube.o \
./hwconf/hw_victor_r1a.o 

C_DEPS += \
./hwconf/drv8301.d \
./hwconf/drv8305.d \
./hwconf/drv8320s.d \
./hwconf/drv8323s.d \
./hwconf/hw.d \
./hwconf/hw_100_250.d \
./hwconf/hw_40.d \
./hwconf/hw_410.d \
./hwconf/hw_45.d \
./hwconf/hw_46.d \
./hwconf/hw_48.d \
./hwconf/hw_49.d \
./hwconf/hw_60.d \
./hwconf/hw_75_300.d \
./hwconf/hw_a200s_v2.d \
./hwconf/hw_axiom.d \
./hwconf/hw_axiom_fpga_bitstream.d \
./hwconf/hw_binar_v1.d \
./hwconf/hw_das_mini.d \
./hwconf/hw_das_rs.d \
./hwconf/hw_hd.d \
./hwconf/hw_luna_bbshd.d \
./hwconf/hw_mini4.d \
./hwconf/hw_r2.d \
./hwconf/hw_rd2.d \
./hwconf/hw_rh.d \
./hwconf/hw_tp.d \
./hwconf/hw_uavc_omega.d \
./hwconf/hw_uavc_qcube.d \
./hwconf/hw_victor_r1a.d 


# Each subdirectory must supply rules for building sources it contributes
hwconf/%.o: ../hwconf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\common\ports\ARMCMx\devices\STM32F4xx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\ext\CMSIS\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\ext\CMSIS\ST" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\ports\ARMCMx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\rt\ports\ARMCMx\compilers\GCC" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\osal\rt" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\include" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\common\ARMCMx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\STM32F4xx" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\DACv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\GPIOv2" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\I2Cv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\OTGv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\RTCv2" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\SPIv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\TIMv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\ports\STM32\LLD\USARTv1" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\various" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\os\hal\lib\streams" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\mcconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\appconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\hwconf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\applications" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\nrf" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\libcanard" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\libcanard\dsdl" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\imu" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\imu\BMI160_driver" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\compression" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\blackmagic" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\blackmagic\target" -I"C:\Users\unbre\Documents\EarthSense\ES-ESC-FW\ChibiOS_3.0.2\ext\stdperiph_stm32f4\inc" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


