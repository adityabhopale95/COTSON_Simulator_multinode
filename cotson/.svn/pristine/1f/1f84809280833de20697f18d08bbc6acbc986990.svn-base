.PHONY: clean distclean all
subdirs = src data
RM = /bin/rm -rf

build: $(addprefix build_, $(subdirs))
install: $(addprefix install_, $(subdirs))
clean: $(addprefix clean_, $(subdirs))
distclean: $(addprefix distclean_, $(subdirs)) distclean_web
	$(RM) etc/cotson etc/regression-test etc/daemon-start \
	      src/Make.conf sbin/*

build_%:
	@echo "########## Build $* #############"; \
	$(MAKE) --directory=./$* all

install_%: build_%
	@echo "########## Install $* #############"; \
	$(MAKE) --directory=./$* install

clean_%:
	@echo "########## Clean $* #############"; \
	$(MAKE) --directory=./$* clean

distclean_%:
	@echo "########## Distclean $* #############"; \
	$(MAKE) --directory=./$* distclean

distclean_web:
	@echo "########## Distclean $* #############"; \
	$(RM) web/tmp web/log web/etc/local-defaults; \
	$(RM) web/db/development.cotson.sqlite3; \
	$(RM) web/config/database_cotson.yml; \

release:
	$(MAKE) OPT=release install

debug:
	$(MAKE) OPT=debug install

src/Make.conf: src/Make.conf.in 
	if test -e config.status; then \
	    ./config.status; \
	else \
		touch src/Make.conf; \
	fi

regression: build
	cd src && make regression

run_examples: build
	cd src/examples && make run

run_example_regression: build
	cd src/examples && ./example-regression.sh
