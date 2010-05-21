#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/kmeansDemo.o \
	${OBJECTDIR}/pyrSegmentationDemo.o \
	${OBJECTDIR}/segmentAlgo.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/imageSegment.o \
	${OBJECTDIR}/PyrMeanShiftSegmentationDemo.o \
	${OBJECTDIR}/watershedDemo.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk ../test/segm

../test/segm: ${OBJECTFILES}
	${MKDIR} -p ../test
	${LINK.cc} -lcxcore -lcv -lhighgui -lcvaux -lml -o ../test/segm ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/kmeansDemo.o: nbproject/Makefile-${CND_CONF}.mk kmeansDemo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/kmeansDemo.o kmeansDemo.cpp

${OBJECTDIR}/pyrSegmentationDemo.o: nbproject/Makefile-${CND_CONF}.mk pyrSegmentationDemo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/pyrSegmentationDemo.o pyrSegmentationDemo.cpp

${OBJECTDIR}/segmentAlgo.o: nbproject/Makefile-${CND_CONF}.mk segmentAlgo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/segmentAlgo.o segmentAlgo.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/imageSegment.o: nbproject/Makefile-${CND_CONF}.mk imageSegment.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/imageSegment.o imageSegment.cpp

${OBJECTDIR}/PyrMeanShiftSegmentationDemo.o: nbproject/Makefile-${CND_CONF}.mk PyrMeanShiftSegmentationDemo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/PyrMeanShiftSegmentationDemo.o PyrMeanShiftSegmentationDemo.cpp

${OBJECTDIR}/watershedDemo.o: nbproject/Makefile-${CND_CONF}.mk watershedDemo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/watershedDemo.o watershedDemo.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../test/segm

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
