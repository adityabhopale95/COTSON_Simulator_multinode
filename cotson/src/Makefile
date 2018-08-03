HERE = $(shell pwd)
subdirs = libluabind slirp disksim distorm common abaeterno network tools examples mcpat

## toplevel tools
all: build_abaeterno build_network build_tools build_examples build_mcpat

## libraries
libs=build_libluabind build_slirp build_disksim build_distorm build_common

build: $(addprefix build_, $(subdirs))
install: $(addprefix install_, $(subdirs))
dep: $(addprefix dep_, $(subdirs))
clean: $(addprefix clean_, $(subdirs))
	$(RM) test_*.log test.regression/update.sh
distclean: $(addprefix distclean_, $(subdirs)) clean

regression: test_@all

regression-update: test_@all.log
	cd test.regression; \
	./test-update < ../test_@all.log > update.sh; \
	sh update.sh

include Make.conf

.PHONY: build_% install_% dep_% clean_% distclean_% test_% all test libs
.PHONY: regression regression-update

build_%:
	@echo "########## Build $* #############"; \
	$(MAKE) --directory=./$* all

install_%: build_%
	@echo "########## Install $* #############"; \
	$(MAKE) --directory=./$* install

dep_%:
	@echo "########## Dep $* #############"; \
	$(MAKE) --directory=./$* dep

clean_%:
	@echo "########## Clean $* #############"; \
	$(MAKE) --directory=./$* clean

distclean_%:
	@echo "########## Distclean $* #############"; \
	$(MAKE) --directory=./$* distclean

test_%:
	echo "########## Regression Tests #############"; \
	../bin/regression-test $* \
	    data_dir='"$(DATA_DIR)"' \
	    simnow_dir='"$(SIMNOW_DIR)"' \
		abaeterno_so='"$(HERE)/abaeterno/abaeterno.so"' \
		injector='"$(HERE)/abaeterno/aaInjector"' \
	| tee test_$*.log 2>&1

## Disksim has problems with parallel builds...
build_disksim:
	@echo "########## Build disksim #############"; \
	$(MAKE) -j1 --directory=./disksim all

build_abaeterno: $(libs)

build_network: $(libs)

build_common: build_libluabind

build_examples: install_abaeterno

build_mcpat: install_abaeterno
	$(MAKE) -C mcpat

release:
	$(MAKE) OPT=release install

debug:
	$(MAKE) OPT=debug install

# DO NOT DELETE
