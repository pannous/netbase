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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/console.o \
	${OBJECTDIR}/src/export.o \
	${OBJECTDIR}/src/import.o \
	${OBJECTDIR}/src/init.o \
	${OBJECTDIR}/src/md5.o \
	${OBJECTDIR}/src/netbase.o \
	${OBJECTDIR}/src/query.o \
	${OBJECTDIR}/src/reflection.o \
	${OBJECTDIR}/src/relations.o \
	${OBJECTDIR}/src/tests.o \
	${OBJECTDIR}/src/util.o \
	${OBJECTDIR}/src/webserver.o


# C Compiler Flags
CFLAGS=-m64 -g

# CC Compiler Flags
CCFLAGS=-m64 -g
CXXFLAGS=-m64 -g

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=-arch x86_64

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk netbase

netbase: ${OBJECTFILES}
	${LINK.cc} -o netbase ${OBJECTFILES} ${LDLIBSOPTIONS} -lreadline -g

${OBJECTDIR}/src/console.o: nbproject/Makefile-${CND_CONF}.mk src/console.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/console.o src/console.cpp

${OBJECTDIR}/src/export.o: nbproject/Makefile-${CND_CONF}.mk src/export.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/export.o src/export.cpp

${OBJECTDIR}/src/import.o: nbproject/Makefile-${CND_CONF}.mk src/import.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/import.o src/import.cpp

${OBJECTDIR}/src/init.o: nbproject/Makefile-${CND_CONF}.mk src/init.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/init.o src/init.cpp

${OBJECTDIR}/src/md5.o: nbproject/Makefile-${CND_CONF}.mk src/md5.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/md5.o src/md5.cpp

${OBJECTDIR}/src/netbase.o: nbproject/Makefile-${CND_CONF}.mk src/netbase.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/netbase.o src/netbase.cpp

${OBJECTDIR}/src/query.o: nbproject/Makefile-${CND_CONF}.mk src/query.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/query.o src/query.cpp

${OBJECTDIR}/src/reflection.o: nbproject/Makefile-${CND_CONF}.mk src/reflection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/reflection.o src/reflection.cpp

${OBJECTDIR}/src/relations.o: nbproject/Makefile-${CND_CONF}.mk src/relations.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/relations.o src/relations.cpp

${OBJECTDIR}/src/tests.o: nbproject/Makefile-${CND_CONF}.mk src/tests.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tests.o src/tests.cpp

${OBJECTDIR}/src/util.o: nbproject/Makefile-${CND_CONF}.mk src/util.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/util.o src/util.cpp

${OBJECTDIR}/src/webserver.o: nbproject/Makefile-${CND_CONF}.mk src/webserver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -w -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include/darwin -I/Library/Java/JavaVirtualMachines/1.7.0_45.jdk/Contents/Home/include -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/webserver.o src/webserver.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} netbase

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
