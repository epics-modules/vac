#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG

DIRS := configure

DIRS += vacApp
vacApp_DEPEND_DIRS  = configure

DIRS += iocBoot
iocBoot_DEPEND_DIRS = vacApp

include $(TOP)/configure/RULES_TOP
