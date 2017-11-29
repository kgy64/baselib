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
	install -d ${includedir}/baselib
	install -d ${libdir}
	cd bin; \
	for file in *.a; \
	do \
		test -e $$file || continue; \
		install -m 644 $$file ${libdir}/$$file; \
	done
	cd bin; \
	for file in *.so; \
	do \
		test -e $$file || continue; \
		install -m 644 $$file ${libdir}/$$file; \
	done
	cd include/$(OPERATING_SYSTEM); \
	for file in `find -L . -name "*.h"`; \
	do \
		install -d "${includedir}/baselib/$$(dirname "$$file")"; \
		cp "$$file" "${includedir}/baselib/$$(dirname "$$file")"; \
	done

