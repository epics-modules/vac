TOP=../..
include $(TOP)/configure/CONFIG

PROD_NAME = vacExApp
PROD_IOC = $(PROD_NAME)

DBD += $(PROD_NAME).dbd

$(PROD_NAME)_DBD += base.dbd
$(PROD_NAME)_DBD += $(VAC_IOC_DBDS)

# asyn does not have cfg-deps yet; list explicitly
$(PROD_NAME)_DBD += asyn.dbd
$(PROD_NAME)_DBD += drvAsynIPPort.dbd
$(PROD_NAME)_DBD += drvAsynSerialPort.dbd

$(PROD_NAME)_SRCS += $(PROD_NAME)_registerRecordDeviceDriver.cpp
$(PROD_NAME)_SRCS_DEFAULT += $(PROD_NAME)Main.cpp
$(PROD_NAME)_SRCS_vxWorks += -nil-

$(PROD_NAME)_LIBS += $(VAC_IOC_LIBS)
$(PROD_NAME)_LIBS += asyn

$(PROD_NAME)_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
