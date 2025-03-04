################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../cc32xxs_nortos.cmd 

SYSCFG_SRCS += \
../image.syscfg \
../uart2echo.syscfg 

C_SRCS += \
../main_nortos.c \
../uart2echo.c \
./syscfg/ti_drivers_config.c 

GEN_FILES += \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./syscfg \
./syscfg 

C_DEPS += \
./main_nortos.d \
./uart2echo.d \
./syscfg/ti_drivers_config.d 

OBJS += \
./main_nortos.o \
./uart2echo.o \
./syscfg/ti_drivers_config.o 

GEN_MISC_FILES += \
./syscfg/ti_drivers_net_wifi_config.json \
./syscfg/ti_drivers_config.h \
./syscfg/ti_utils_build_linker.cmd.genlibs \
./syscfg/syscfg_c.rov.xs 

GEN_MISC_DIRS__QUOTED += \
"syscfg" \
"syscfg" 

OBJS__QUOTED += \
"main_nortos.o" \
"uart2echo.o" \
"syscfg\ti_drivers_config.o" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_net_wifi_config.json" \
"syscfg\ti_drivers_config.h" \
"syscfg\ti_utils_build_linker.cmd.genlibs" \
"syscfg\syscfg_c.rov.xs" 

C_DEPS__QUOTED += \
"main_nortos.d" \
"uart2echo.d" \
"syscfg\ti_drivers_config.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_drivers_config.c" 

SYSCFG_SRCS__QUOTED += \
"../image.syscfg" \
"../uart2echo.syscfg" 

C_SRCS__QUOTED += \
"../main_nortos.c" \
"../uart2echo.c" \
"./syscfg/ti_drivers_config.c" 


