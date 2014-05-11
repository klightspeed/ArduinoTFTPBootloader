SUBDIRS = bootloader updater bootsafe libetherten

.PHONY: clean all $(SUBDIRS)

all: libetherten bootloader updater bootsafe

$(SUBDIRS):
	$(MAKE) -C $@

updater: libetherten bootloader bootsafe

bootloader: libetherten bootsafe

clean:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
