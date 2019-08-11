#!/usr/bin/make -f
# Makefile for DPF STK Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = StkFlute

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	Flute.cpp \
	StkSources.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -I ../../stk/include

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif

TARGETS += dssi_dsp
TARGETS += lv2_sep
TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------
