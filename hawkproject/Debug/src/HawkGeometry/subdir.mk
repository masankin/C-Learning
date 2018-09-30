################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HawkGeometry/HawkAxisAlignedBox.cpp \
../src/HawkGeometry/HawkBezier.cpp \
../src/HawkGeometry/HawkCapsule.cpp \
../src/HawkGeometry/HawkDegree.cpp \
../src/HawkGeometry/HawkGeometry.cpp \
../src/HawkGeometry/HawkLine2D.cpp \
../src/HawkGeometry/HawkLine3D.cpp \
../src/HawkGeometry/HawkMatrix2.cpp \
../src/HawkGeometry/HawkMatrix3.cpp \
../src/HawkGeometry/HawkMatrix4.cpp \
../src/HawkGeometry/HawkOBB.cpp \
../src/HawkGeometry/HawkPlane3D.cpp \
../src/HawkGeometry/HawkPoint2D.cpp \
../src/HawkGeometry/HawkQuaternion.cpp \
../src/HawkGeometry/HawkRadian.cpp \
../src/HawkGeometry/HawkRay2D.cpp \
../src/HawkGeometry/HawkRay3D.cpp \
../src/HawkGeometry/HawkRect.cpp \
../src/HawkGeometry/HawkRect2D.cpp \
../src/HawkGeometry/HawkRect3D.cpp \
../src/HawkGeometry/HawkSegment2D.cpp \
../src/HawkGeometry/HawkSegment3D.cpp \
../src/HawkGeometry/HawkSphere.cpp \
../src/HawkGeometry/HawkTransform.cpp \
../src/HawkGeometry/HawkTriangle2D.cpp \
../src/HawkGeometry/HawkTriangle3D.cpp \
../src/HawkGeometry/HawkVector2D.cpp \
../src/HawkGeometry/HawkVector3D.cpp \
../src/HawkGeometry/HawkVector4D.cpp 

OBJS += \
./src/HawkGeometry/HawkAxisAlignedBox.o \
./src/HawkGeometry/HawkBezier.o \
./src/HawkGeometry/HawkCapsule.o \
./src/HawkGeometry/HawkDegree.o \
./src/HawkGeometry/HawkGeometry.o \
./src/HawkGeometry/HawkLine2D.o \
./src/HawkGeometry/HawkLine3D.o \
./src/HawkGeometry/HawkMatrix2.o \
./src/HawkGeometry/HawkMatrix3.o \
./src/HawkGeometry/HawkMatrix4.o \
./src/HawkGeometry/HawkOBB.o \
./src/HawkGeometry/HawkPlane3D.o \
./src/HawkGeometry/HawkPoint2D.o \
./src/HawkGeometry/HawkQuaternion.o \
./src/HawkGeometry/HawkRadian.o \
./src/HawkGeometry/HawkRay2D.o \
./src/HawkGeometry/HawkRay3D.o \
./src/HawkGeometry/HawkRect.o \
./src/HawkGeometry/HawkRect2D.o \
./src/HawkGeometry/HawkRect3D.o \
./src/HawkGeometry/HawkSegment2D.o \
./src/HawkGeometry/HawkSegment3D.o \
./src/HawkGeometry/HawkSphere.o \
./src/HawkGeometry/HawkTransform.o \
./src/HawkGeometry/HawkTriangle2D.o \
./src/HawkGeometry/HawkTriangle3D.o \
./src/HawkGeometry/HawkVector2D.o \
./src/HawkGeometry/HawkVector3D.o \
./src/HawkGeometry/HawkVector4D.o 

CPP_DEPS += \
./src/HawkGeometry/HawkAxisAlignedBox.d \
./src/HawkGeometry/HawkBezier.d \
./src/HawkGeometry/HawkCapsule.d \
./src/HawkGeometry/HawkDegree.d \
./src/HawkGeometry/HawkGeometry.d \
./src/HawkGeometry/HawkLine2D.d \
./src/HawkGeometry/HawkLine3D.d \
./src/HawkGeometry/HawkMatrix2.d \
./src/HawkGeometry/HawkMatrix3.d \
./src/HawkGeometry/HawkMatrix4.d \
./src/HawkGeometry/HawkOBB.d \
./src/HawkGeometry/HawkPlane3D.d \
./src/HawkGeometry/HawkPoint2D.d \
./src/HawkGeometry/HawkQuaternion.d \
./src/HawkGeometry/HawkRadian.d \
./src/HawkGeometry/HawkRay2D.d \
./src/HawkGeometry/HawkRay3D.d \
./src/HawkGeometry/HawkRect.d \
./src/HawkGeometry/HawkRect2D.d \
./src/HawkGeometry/HawkRect3D.d \
./src/HawkGeometry/HawkSegment2D.d \
./src/HawkGeometry/HawkSegment3D.d \
./src/HawkGeometry/HawkSphere.d \
./src/HawkGeometry/HawkTransform.d \
./src/HawkGeometry/HawkTriangle2D.d \
./src/HawkGeometry/HawkTriangle3D.d \
./src/HawkGeometry/HawkVector2D.d \
./src/HawkGeometry/HawkVector3D.d \
./src/HawkGeometry/HawkVector4D.d 


# Each subdirectory must supply rules for building sources it contributes
src/HawkGeometry/%.o: ../src/HawkGeometry/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


