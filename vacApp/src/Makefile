TOP=../..

include $(TOP)/configure/CONFIG

# Build an IOC support library
LIBRARY_IOC += vac

DBDINC += vsRecord
DBDINC += digitelRecord

DBD += vacSupport.dbd
DBD += devDigitelPump.dbd
DBD += devVacSen.dbd

USR_CFLAGS += -DUSE_TYPED_RSET -DUSE_TYPED_DSET

vac_SRCS += vsRecord.c
vac_SRCS += digitelRecord.c

vac_SRCS += devDigitelPump.c
vac_SRCS += devVacSen.c

vac_LIBS += asyn
vac_LIBS += $(EPICS_BASE_IOC_LIBS)


#=============================================================================
# Build an ioc application <name>
# Don't need for /APSshare --tmm 10/16/2007
PROD_IOC = vacApp

#-----------------------------------------------------------------------------
# <name>.dbd and <name>VX.dbd will be created and installed
DBD += vacApp.dbd
#DBD += vacAppVX.dbd

#-----------------------------------------------------------------------------
# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
# <name>VX_registerRecordDeviceDriver.cpp will be created from <name>VX.dbd
vacApp_SRCS_DEFAULT += vacApp_registerRecordDeviceDriver.cpp
vacApp_SRCS_vxWorks += vacAppVX_registerRecordDeviceDriver.cpp

#-----------------------------------------------------------------------------
# Include application sources
vacApp_SRCS_DEFAULT += vacAppMain.cpp
vacApp_SRCS_vxWorks += -nil-

#-----------------------------------------------------------------------------
# Include support from base/src/vxWorks
vacApp_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#-----------------------------------------------------------------------------
# Include libraries
vacApp_Common_LIBS  += vac asyn
vacApp_Common_LIBS  += $(EPICS_BASE_IOC_LIBS)
#vacApp_vxWorks_LIBS += TyGSOctal Ipac

vacApp_LIBS_DEFAULT += $(vacApp_Common_LIBS)
vacApp_LIBS_vxWorks += $(vacApp_Common_LIBS) $(vacApp_vxWorks_LIBS)
#
#==============================================================================

include $(TOP)/configure/RULES
#------------------------------------------------------------------------------
#  ADD RULES AFTER THIS LINE

# Parallel build dependencies
digitelRecord$(OBJ): $(COMMON_DIR)/digitelRecord.h
devDigitelPump$(OBJ): $(COMMON_DIR)/digitelRecord.h
vsRecord$(OBJ): $(COMMON_DIR)/vsRecord.h
devVacSen$(OBJ): $(COMMON_DIR)/vsRecord.h

