TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this
#DB += dbExample1.db
#DB += dbExample2.db
#DB += dbSubExample.db

# This can also be used for the QPC communications if desired
# but uses the digitelRecord rather than individual records.
# It also does not use a streamDevice proto file but rather the
# same old EPICS asyn driver.
DB += digitelPump.db

# This is used for Televac vacuum guages MM200 and MX200
DB += vs.db

# QPCstreams.db can be used for Ethernet or serial
DB += QPCstreams.db

# QPC Modbus
DB += QPCmodbus.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

