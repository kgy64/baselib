# Makefile for my basic library
#

export OPERATING_SYSTEM        ?=   unix

export BASELIB_ROOT             =   $(PWD)

# =======================================================================================

-include makesys/scripts/make.root

makesys/scripts/make.root:
	git submodule update --init --recursive

