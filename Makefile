#!/usr/bin/make -f
# Makefile for DPF STK Plugins #
# ---------------------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

all: plugins gen

# --------------------------------------------------------------

plugins:
	$(MAKE) all -C plugins/Flute

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/Flute
	rm -rf bin build

# --------------------------------------------------------------

.PHONY: plugins
