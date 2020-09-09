include ../Common.mk

TARGET = KinetisBootloaderApp
MODULE = module

EXE_DEFS = -DUSE_ICON

$(TARGET):
	@echo ''
	@echo  Building $@ ${ARCH}
	@echo "================================================================"
	$(MAKE) exe -f Target.mk ARCH=$(ARCH) BUILDDIR=$@$(BUILDDIR_SUFFIX) MODULE=$(MODULE) TARGET=$@ CDEFS='$(EXE_DEFS)'

all: $(TARGET)

clean: 
	${RMDIR} $(TARGET)$(BUILDDIR_SUFFIX)

.PHONY: all clean 
.PHONY: $(TARGET)