################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/UEyeOpenCV.cpp \
../src/frame.cpp \
../src/main.cpp \
../src/serialib.cpp \
../src/utils.cpp 

OBJS += \
./src/UEyeOpenCV.o \
./src/frame.o \
./src/main.o \
./src/serialib.o \
./src/utils.o 

CPP_DEPS += \
./src/UEyeOpenCV.d \
./src/frame.d \
./src/main.d \
./src/serialib.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/jetski/boost_1_65_1 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


