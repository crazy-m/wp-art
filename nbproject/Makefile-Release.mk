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
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ini.o \
	${OBJECTDIR}/dlgAbout.o \
	${OBJECTDIR}/settings.o \
	${OBJECTDIR}/commport.o \
	${OBJECTDIR}/dlgSettings.o \
	${OBJECTDIR}/device.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=-Wl,--subsystem,windows -mwindows -D__WINDOWS__ -D_WIN32_WINNT=0x0501

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=resource.o /c/windows/system32/comctl32.dll

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/art.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/art.exe: /c/windows/system32/comctl32.dll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/art.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -lm -lstdc++ -lgcc -lodbc32 -lwsock32 -lwinspool -lwinmm -lshell32 -lcomctl32 -lctl3d32 -lodbc32 -ladvapi32 -lodbc32 -lwsock32 -lopengl32 -lglu32 -lole32 -loleaut32 -luuid -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/art ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/ini.o: ini.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/ini.o ini.c

${OBJECTDIR}/dlgAbout.o: dlgAbout.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/dlgAbout.o dlgAbout.c

${OBJECTDIR}/settings.o: settings.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/settings.o settings.c

${OBJECTDIR}/commport.o: commport.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/commport.o commport.c

${OBJECTDIR}/dlgSettings.o: dlgSettings.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/dlgSettings.o dlgSettings.c

${OBJECTDIR}/device.o: device.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/device.o device.c

resource.o: resource.rc resource.h
	@echo Compiling resources…
	windres.exe resource.rc resource.o

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/art.exe
	${RM} resource.o

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
