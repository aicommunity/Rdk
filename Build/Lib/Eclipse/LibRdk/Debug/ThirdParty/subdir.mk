################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../../../../../../Klavesin-STZ/Rdk/ThirdParty/ThirdParty.cpp 

OBJS += \
./ThirdParty/ThirdParty.o 

CPP_DEPS += \
./ThirdParty/ThirdParty.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/ThirdParty.o: ../../../../../../../Klavesin-STZ/Rdk/ThirdParty/ThirdParty.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


