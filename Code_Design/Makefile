
BITNESS=64

include Common.mk

# Built on both Windows and Linux
DIRS = \
  CPLD_ProgrammerApp      \
  KinetisBootloaderApp 
  
export TARGET_BINDIR TARGET_LIBDIR BUILDDIR_SUFFIX VSUFFIX
export BITNESS TARGET_BINDIR TARGET_LIBDIR BUILDDIR_SUFFIX

BUILD_DIRS = $(DIRS:%=build-%)
CLEAN_DIRS = $(DIRS:%=clean-%)

all: addExternalFiles $(BUILD_DIRS)

clean: cleanTargetFiles $(CLEAN_DIRS)

cleanTargetFiles:
	$(RMDIR) $(DUMMY_CHILD)/$(TARGET_LIBDIR)
	$(RMDIR) $(DUMMY_CHILD)/$(TARGET_BINDIR)

# Copy external DLLs
addExternalFiles:
ifeq ($(UNAME_S),Windows)
	$(MKDIR) $(DUMMY_CHILD)/$(TARGET_LIBDIR)
	$(MKDIR) $(DUMMY_CHILD)/$(TARGET_BINDIR)
endif

$(BUILD_DIRS):
	$(MAKE) -C $(@:build-%=%) all -f Makefile

$(CLEAN_DIRS):
	$(MAKE) -C $(@:clean-%=%) clean -f Makefile 

.PHONY: $(CLEAN_DIRS)
.PHONY: $(BUILD_DIRS)
.PHONY: clean all cleanTargetFiles addExternalFiles

