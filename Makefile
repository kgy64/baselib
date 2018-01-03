# Makefile for my basic library
#

export OPERATING_SYSTEM        ?=   unix

export BASELIB_ROOT             =   $(PWD)

# =======================================================================================

-include makesys/scripts/make.root

libdir                         ?=   /usr/local/lib
includedir                     ?=   /usr/local/include

makesys/scripts/make.root:
	git submodule update --init --recursive

install:
	install -d ${base_prefix}${includedir}/baselib
	install -d ${base_prefix}${libdir}
	cd bin; \
	for file in *.a; \
	do \
		install -m 644 $$file ${base_prefix}${libdir}/$$file; \
	done
	cd bin; \
	for file in *.so*; \
	do \
		cp -a $$file ${base_prefix}${libdir}/$$file; \
	done
	cd include/$(OPERATING_SYSTEM); \
	for file in `find -L . -name "*.h"`; \
	do \
		install -d "${base_prefix}${includedir}/baselib/$$(dirname "$$file")"; \
		cp "$$file" "${base_prefix}${includedir}/baselib/$$(dirname "$$file")"; \
	done
	cd opsys/$(OPERATING_SYSTEM); \
	for file in `find . -name "*.h"`; \
	do \
		install -d "${base_prefix}${includedir}/baselib/$$(dirname "$$file")"; \
		cp "$$file" "${base_prefix}${includedir}/baselib/$$(dirname "$$file")"; \
	done

