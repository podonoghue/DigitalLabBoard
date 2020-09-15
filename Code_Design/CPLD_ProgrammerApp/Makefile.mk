include ../Common.mk

TARGET = CPLD_ProgrammerApp
MODULE = module

EXE_DEFS = -DUSE_ICON

$(TARGET):
	@echo --
	@echo  Building $@ ${ARCH}
	@echo ================================================================
	$(MAKE) exe -f Target.mk ARCH=$(ARCH) BUILDDIR=$@$(BUILDDIR_SUFFIX) MODULE=$(MODULE) TARGET=$@ CDEFS='$(EXE_DEFS)'

# Copy external DLLs
addExternalFiles:
ifeq ($(UNAME_S),Windows)
	$(MKDIR) $(TARGET_LIBDIR)
	$(MKDIR) $(TARGET_BINDIR)
	$(CP) $(SHARED_LIBDIRS)/* $(TARGET_LIBDIR)
endif

all: addExternalFiles $(TARGET)

clean: 
	${RMDIR} $(TARGET)$(BUILDDIR_SUFFIX)

.PHONY: all clean addExternalFiles
.PHONY: $(TARGET)