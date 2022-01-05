include ../Common.mk

TARGET = KinetisBootloaderApp
MODULE = module

EXE_DEFS = -DUSE_ICON

$(TARGET):
	@echo --
	@echo -- Building $@
	@echo ================================================================
	$(MAKE) exe -f Target.mk BUILDDIR=$@$(BUILDDIR_SUFFIX) MODULE=$(MODULE) TARGET=$@ CDEFS='$(EXE_DEFS)'

$(TARGET)-debug:
	@echo --
	@echo -- Building $@
	@echo ================================================================
	$(MAKE) exe -f Target.mk BUILDDIR=$@$(BUILDDIR_SUFFIX) MODULE=$(MODULE) TARGET=$@ CDEFS='$(EXE_DEFS)'

all: commonFiles $(TARGET) $(TARGET)-debug

clean: 
	${RMDIR} $(TARGET)$(BUILDDIR_SUFFIX)

.PHONY: all clean
.PHONY: $(TARGET)