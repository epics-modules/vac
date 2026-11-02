TOP=../..

include $(TOP)/configure/CONFIG

# Build an IOC support library
LIBRARY_IOC += vac

DBDINC += vsRecord
DBDINC += digitelRecord

DBD += vacSupport.dbd
DBD += devDigitelPump.dbd
DBD += devVacSen.dbd

CFG += VAC_DEPS

USR_CFLAGS += -DUSE_TYPED_RSET -DUSE_TYPED_DSET

vac_SRCS += vsRecord.c
vac_SRCS += digitelRecord.c

vac_SRCS += devDigitelPump.c
vac_SRCS += devVacSen.c

vac_LIBS += asyn
vac_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#------------------------------------------------------------------------------
#  ADD RULES AFTER THIS LINE

# Parallel build dependencies
digitelRecord$(OBJ): $(COMMON_DIR)/digitelRecord.h
devDigitelPump$(OBJ): $(COMMON_DIR)/digitelRecord.h
vsRecord$(OBJ): $(COMMON_DIR)/vsRecord.h
devVacSen$(OBJ): $(COMMON_DIR)/vsRecord.h

