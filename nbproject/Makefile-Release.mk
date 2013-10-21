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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1881278108/init.o \
	${OBJECTDIR}/_ext/1881278108/webserver.o \
	${OBJECTDIR}/src/console.o \
	${OBJECTDIR}/src/export.o \
	${OBJECTDIR}/src/import.o \
	${OBJECTDIR}/src/jni/NetbaseJNI.o \
	${OBJECTDIR}/src/md5.o \
	${OBJECTDIR}/src/netbase.o \
	${OBJECTDIR}/src/query.o \
	${OBJECTDIR}/src/reflection.o \
	${OBJECTDIR}/src/relations.o \
	${OBJECTDIR}/src/tests.o \
	${OBJECTDIR}/src/util.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbase

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbase: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbase ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1881278108/init.o: /Users/me/dev/cpp/netbase/src/init.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1881278108
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1881278108/init.o /Users/me/dev/cpp/netbase/src/init.cpp

${OBJECTDIR}/_ext/1881278108/webserver.o: /Users/me/dev/cpp/netbase/src/webserver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1881278108
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1881278108/webserver.o /Users/me/dev/cpp/netbase/src/webserver.cpp

${OBJECTDIR}/src/console.o: src/console.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/console.o src/console.cpp

${OBJECTDIR}/src/export.o: src/export.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/export.o src/export.cpp

${OBJECTDIR}/src/import.o: src/import.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/import.o src/import.cpp

${OBJECTDIR}/src/jni/NetbaseJNI.o: src/jni/NetbaseJNI.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/jni
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/jni/NetbaseJNI.o src/jni/NetbaseJNI.cpp

${OBJECTDIR}/src/md5.o: src/md5.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/md5.o src/md5.cpp

${OBJECTDIR}/src/netbase.o: src/netbase.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/netbase.o src/netbase.cpp

${OBJECTDIR}/src/query.o: src/query.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/query.o src/query.cpp

${OBJECTDIR}/src/reflection.o: src/reflection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/reflection.o src/reflection.cpp

${OBJECTDIR}/src/relations.o: src/relations.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/relations.o src/relations.cpp

${OBJECTDIR}/src/tests.o: src/tests.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tests.o src/tests.cpp

${OBJECTDIR}/src/util.o: src/util.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/util.o src/util.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbase

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
