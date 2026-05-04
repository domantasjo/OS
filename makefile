%:
	@$(MAKE) -f make/makefile $@

.DEFAULT_GOAL := all
all:
	@$(MAKE) -f make/makefile all
