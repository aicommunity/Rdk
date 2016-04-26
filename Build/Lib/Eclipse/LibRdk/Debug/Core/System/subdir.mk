################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../../../../../../Klavesin-STZ/Rdk/Core/System/UGenericMutex.cpp 

OBJS += \
./Core/System/UGenericMutex.o 

CPP_DEPS += \
./Core/System/UGenericMutex.d 


# Each subdirectory must supply rules for building sources it contributes
Core/System/UGenericMutex.o: ../../../../../../../Klavesin-STZ/Rdk/Core/System/UGenericMutex.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


