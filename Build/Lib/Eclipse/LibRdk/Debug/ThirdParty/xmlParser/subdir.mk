################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/klavesin/workspace/tops-klavesin/Klavesin-STZ/Rdk/ThirdParty/xmlParser/xmlParser.cpp 

OBJS += \
./ThirdParty/xmlParser/xmlParser.o 

CPP_DEPS += \
./ThirdParty/xmlParser/xmlParser.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/xmlParser/xmlParser.o: /home/klavesin/workspace/tops-klavesin/Klavesin-STZ/Rdk/ThirdParty/xmlParser/xmlParser.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


