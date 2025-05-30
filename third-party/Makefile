ifndef CHPL_MAKE_HOME
export CHPL_MAKE_HOME=$(shell pwd)/..
endif
include $(CHPL_MAKE_HOME)/make/Makefile.base

ifneq (, $(call isTrue, $(CHPL_DEVELOPER)))
DEBUG=1
export DEBUG
WARNINGS=1
export WARNINGS
else
OPTIMIZE=1
export OPTIMIZE
endif

default: all

all: test-venv chpldoc-venv \
     gasnet gmp hwloc jemalloc mimalloc libfabric libunwind qthread llvm

clean: FORCE
	cd chpl-venv && $(MAKE) clean
	cd gasnet && $(MAKE) clean
	cd gmp && $(MAKE) clean
	cd hwloc && $(MAKE) clean
	cd jemalloc && $(MAKE) clean
	cd mimalloc && $(MAKE) clean
	cd libfabric && $(MAKE) clean
	cd libunwind && $(MAKE) clean
	cd llvm && $(MAKE) clean
	cd qthread && $(MAKE) clean
	cd re2 && $(MAKE) clean

cleanall: FORCE
	cd chpl-venv && $(MAKE) cleanall
	cd gasnet && $(MAKE) cleanall
	cd gmp && $(MAKE) cleanall
	cd hwloc && $(MAKE) cleanall
	cd jemalloc && $(MAKE) cleanall
	cd mimalloc && $(MAKE) cleanall
	cd libfabric && $(MAKE) cleanall
	cd libunwind && $(MAKE) cleanall
	cd llvm && $(MAKE) cleanall
	cd qthread && $(MAKE) cleanall
	cd re2 && $(MAKE) cleanall

clobber: FORCE
	cd chpl-venv && $(MAKE) clobber
	cd gasnet && $(MAKE) clobber
	cd gmp && $(MAKE) clobber
	cd hwloc && $(MAKE) clobber
	cd jemalloc && $(MAKE) clobber
	cd mimalloc && $(MAKE) clobber
	cd libfabric && $(MAKE) clobber
	cd libunwind && $(MAKE) clobber
	cd llvm && $(MAKE) clobber
	cd qthread && $(MAKE) clobber
	cd re2 && $(MAKE) clobber

depend:

# Third-party dependencies. If any of these files (our 3rd party README or
# Makefiles) change, then rebuild the 3rd party lib. This allows us to
# automatically rebuild on 3rd party version upgrades or if we specify
# different options in the Makefile. Note that this requires us to update the
# version number and list patches in our README file. We used to use
# third-party source files as dependencies too, but that introduced problems
# with some files getting rebuilt because of built-in make rules.

# Find 3rd party dependencies -- arg 1 is the top-level dir with Makefiles/README
find_3p_depend = $(1)/Makefile* $(1)/README

GASNET_DEPEND         := $(call find_3p_depend, $(GASNET_DIR))
GMP_DEPEND            := $(call find_3p_depend, $(GMP_DIR))
HWLOC_DEPEND          := $(call find_3p_depend, $(HWLOC_DIR))
JEMALLOC_DEPEND       := $(call find_3p_depend, $(JEMALLOC_DIR))
MIMALLOC_DEPEND       := $(call find_3p_depend, $(MIMALLOC_DIR))
LIBFABRIC_DEPEND      := $(call find_3p_depend, $(LIBFABRIC_DIR))
LIBUNWIND_DEPEND      := $(call find_3p_depend, $(LIBUNWIND_DIR))
LLVM_DEPEND           := $(call find_3p_depend, $(LLVM_DIR))
QTHREAD_DEPEND        := $(call find_3p_depend, $(QTHREAD_DIR))
RE2_DEPEND            := $(call find_3p_depend, $(RE2_DIR))


test-venv: FORCE
	cd chpl-venv && $(MAKE) test-venv

chpldoc-venv: FORCE
	cd chpl-venv && $(MAKE) chpldoc-venv

c2chapel-venv: FORCE
	@if [ -z "$$CHPL_DONT_BUILD_C2CHAPEL_VENV" ]; then \
	cd chpl-venv && $(MAKE) c2chapel-venv; \
	fi

chapel-py-venv: FORCE
	cd chpl-venv && $(MAKE) chapel-py-venv

cls-test-venv: FORCE
	cd chpl-venv && $(MAKE) cls-test-venv

chplcheck-venv: FORCE
	cd chpl-venv && $(MAKE) chplcheck-venv

chpl-language-server-venv: FORCE
	cd chpl-venv && $(MAKE) chpl-language-server-venv

# See gasnet/Makefile for explanation of the post-install step
gasnet: $(GASNET_INSTALL_DIR)
$(GASNET_INSTALL_DIR): $(GASNET_DEPEND)
	rm -rf $(GASNET_INSTALL_DIR) && rm -rf $(GASNET_BUILD_DIR)
	cd gasnet && $(MAKE) && $(MAKE) post-install

try-gmp: FORCE
  ifneq ($(CHPL_MAKE_GMP_IS_OVERRIDDEN), True)
    ifeq ($(wildcard $(GMP_BUILD_DIR)),)
	@echo "Speculatively attempting to build gmp"
	-@$(MAKE) GMP_SPECULATIVE=yes gmp
    else ifeq ($(wildcard $(GMP_H_FILE)),)
	$(info Speculative build of gmp squashed due to previous failures.)
    endif
  endif

gmp: $(GMP_H_FILE)
$(GMP_H_FILE): $(GMP_DEPEND)
	rm -rf $(GMP_INSTALL_DIR) && rm -rf $(GMP_BUILD_DIR)
	cd gmp && $(MAKE)

hwloc: $(HWLOC_INSTALL_DIR)
$(HWLOC_INSTALL_DIR): $(HWLOC_DEPEND)
	rm -rf $(HWLOC_INSTALL_DIR) && rm -rf $(HWLOC_BUILD_DIR)
	cd hwloc && $(MAKE)

jemalloc: $(JEMALLOC_INSTALL_DIR)
$(JEMALLOC_INSTALL_DIR): $(JEMALLOC_DEPEND)
	rm -rf $(JEMALLOC_INSTALL_DIR) && rm -rf $(JEMALLOC_BUILD_DIR)
	cd jemalloc && $(MAKE)

mimalloc: $(MIMALLOC_INSTALL_DIR)
$(MIMALLOC_INSTALL_DIR): $(MIMALLOC_DEPEND)
	rm -rf $(MIMALLOC_INSTALL_DIR) && rm -rf $(MIMALLOC_BUILD_DIR)
	cd mimalloc && $(MAKE)

libfabric: $(LIBFABRIC_INSTALL_DIR)
$(LIBFABRIC_INSTALL_DIR): $(LIBFABRIC_DEPEND)
	rm -rf $(LIBFABRIC_INSTALL_DIR) && rm -rf $(LIBFABRIC_BUILD_DIR)
	cd libfabric && $(MAKE)

libunwind: $(LIBUNWIND_INSTALL_DIR)
$(LIBUNWIND_INSTALL_DIR): $(LIBUNWIND_DEPEND)
	rm -rf $(LIBUNWIND_INSTALL_DIR) && rm -rf $(LIBUNWIND_BUILD_DIR)
	cd libunwind && $(MAKE)

ifeq ($(CHPL_MAKE_LLVM), unset)
# Print the error about needing to set CHPL_LLVM
llvm: FORCE
	cd llvm && $(MAKE) unset

else ifeq ($(CHPL_MAKE_LLVM), bundled)
# build the bundled LLVM
llvm: $(LLVM_INSTALL_DIR)
$(LLVM_INSTALL_DIR): $(LLVM_DEPEND)
	rm -rf $(LLVM_INSTALL_DIR)
	cd llvm && $(MAKE) llvm

else ifeq ($(CHPL_MAKE_LLVM_SUPPORT), bundled)
# build the bundled LLVM support library
llvm: $(LLVM_SUPPORT_INSTALL_DIR)
$(LLVM_SUPPORT_INSTALL_DIR): $(LLVM_DEPEND)
	rm -rf $(LLVM_SUPPORT_INSTALL_DIR)
	cd llvm && $(MAKE) llvm-support

else

# nothing to do for CHPL_LLVM=system / CHPL_LLVM_SUPPORT=system
llvm:
endif

# Qthreads may use hwloc and/or jemalloc. Ensure they are built first.
ifeq ($(CHPL_MAKE_HWLOC), bundled)
QTHREAD_OTHER_INSTALL_DIR_DEPS += $(HWLOC_INSTALL_DIR)
endif
ifeq ($(CHPL_MAKE_TARGET_JEMALLOC), bundled)
QTHREAD_OTHER_INSTALL_DIR_DEPS += $(JEMALLOC_INSTALL_DIR)
endif
# Qthreads uses our memory allocator, trigger rebuilds if our interface changes
QTHREAD_OTHER_INSTALL_DIR_DEPS += $(CHPL_MAKE_HOME)/runtime/include/chpl-mem.h
QTHREAD_OTHER_INSTALL_DIR_DEPS += $(CHPL_MAKE_HOME)/runtime/include/mem/$(CHPL_MAKE_TARGET_MEM)/chpl-mem-impl.h

qthread: $(QTHREAD_INSTALL_DIR)

$(QTHREAD_INSTALL_DIR): $(QTHREAD_DEPEND) $(QTHREAD_OTHER_INSTALL_DIR_DEPS)
	rm -rf $(QTHREAD_INSTALL_DIR) && rm -rf $(QTHREAD_BUILD_DIR)
	cd qthread && $(MAKE)
$(QTHREAD_ALIASES): qthread

try-re2: FORCE
  ifneq ($(CHPL_MAKE_RE2_IS_OVERRIDDEN), True)
    ifeq ($(wildcard $(RE2_BUILD_DIR)),)
	@echo "Speculatively attempting to build re2"
	-@$(MAKE) re2
    else ifeq ($(wildcard $(RE2_H_FILE)),)
	$(info Speculative build of re2 squashed due to previous failures.)
    endif
  endif

re2: $(RE2_H_FILE)
$(RE2_H_FILE): $(RE2_DEPEND)
	rm -rf $(RE2_INSTALL_DIR) && rm -rf $(RE2_BUILD_DIR)
	cd re2 && $(MAKE)

-include Makefile.devel

FORCE:
