################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/main.cpp 

OBJS += \
./Src/main.o 

CPP_DEPS += \
./Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.cpp Src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/barbarossa/Documents/HEAAN/HEAAN/lib -I"/home/barbarossa/Documents/Workspace/SecuSim_HEAANDemo/Header" -I/home/barbarossa/Documents/gmp-6.2.1 -I/home/barbarossa/Documents/ntl-11.5.1/include/NTL -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


