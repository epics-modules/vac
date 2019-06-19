TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# This can also be used for the QPC communications if desired
# but uses the digitelRecord rather than individual records.
# It also does not use a streamDevice proto file but rather the
# same old EPICS asyn driver.
DB += digitelPump.db

# This is used for Televac vacuum guages MM200 and MX200
DB += vs.db

# QPCstreams.db can be used for Ethernet or serial
DB += QPCstreams.db
DB += QPC-eth.proto
DB += QPC-serial.proto

# QPC Modbus
DB += QPCmodbus.db

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

ifndef BASE_3_15
# Rule needed for 3.14 to install .proto files
$(INSTALL_DB)/%: ../%
	$(ECHO) "Installing $@"
	@$(INSTALL) -d -m $(INSTALL_PERMISSIONS) $< $(@D)
endif
