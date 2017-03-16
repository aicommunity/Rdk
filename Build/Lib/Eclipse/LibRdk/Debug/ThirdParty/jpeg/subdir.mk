################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../../../../../../Klavesin-STZ/Rdk/ThirdParty/jpeg/JpegIntegration.cpp 

OBJS += \
./ThirdParty/jpeg/JpegIntegration.o 

CPP_DEPS += \
./ThirdParty/jpeg/JpegIntegration.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/jpeg/JpegIntegration.o: ../../../../../../../Klavesin-STZ/Rdk/ThirdParty/jpeg/JpegIntegration.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


