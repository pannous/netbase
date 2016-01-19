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
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbase ${OBJECTFILES} ${LDLIBSOPTIONS} -lreadline -lz -s -Wl,-S

${OBJECTDIR}/src/console.o: src/console.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/console.o src/console.cpp

${OBJECTDIR}/src/export.o: src/export.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/export.o src/export.cpp

${OBJECTDIR}/src/import.o: src/import.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/import.o src/import.cpp

${OBJECTDIR}/src/utf8.o: src/utf8.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/utf8.o src/utf8.cpp

${OBJECTDIR}/src/init.o: src/init.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/init.o src/init.cpp

${OBJECTDIR}/src/md5.o: src/md5.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/md5.o src/md5.cpp

${OBJECTDIR}/src/netbase.o: src/netbase.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/netbase.o src/netbase.cpp

${OBJECTDIR}/src/query.o: src/query.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/query.o src/query.cpp

${OBJECTDIR}/src/reflection.o: src/reflection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/reflection.o src/reflection.cpp

${OBJECTDIR}/src/relations.o: src/relations.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/relations.o src/relations.cpp

${OBJECTDIR}/src/tests.o: src/tests.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/tests.o src/tests.cpp

${OBJECTDIR}/src/util.o: src/util.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/util.o src/util.cpp

${OBJECTDIR}/src/webserver.o: src/webserver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/src/webserver.o src/webserver.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbase

# Subprojects
.clean-subprojects:
