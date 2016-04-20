################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/klavesin/workspace/tops-klavesin/Klavesin-STZ/Rdk/Core/System/Gcc/rdk_system.gcc.cpp 

OBJS += \
./Core/System/Gcc/rdk_system.gcc.o 

CPP_DEPS += \
./Core/System/Gcc/rdk_system.gcc.d 


# Each subdirectory must supply rules for building sources it contributes
Core/System/Gcc/rdk_system.gcc.o: /home/klavesin/workspace/tops-klavesin/Klavesin-STZ/Rdk/Core/System/Gcc/rdk_system.gcc.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


