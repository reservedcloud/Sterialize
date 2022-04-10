ISO_IMAGE = sterialize.iso

.PHONY: all
all: $(ISO_IMAGE)

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1
	make -C limine

installhdd:
	limine/limine-install hdd.vhd --force-mbr 

.PHONY: src
src:
	$(MAKE) -C src

$(ISO_IMAGE): limine src
	rm -rf iso_root
	mkdir -p iso_root
	cp src/sterialkrnl.sys \
		src/limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(ISO_IMAGE)
	limine/limine-install $(ISO_IMAGE)
	rm -rf iso_root
	$(MAKE) -C src clean

.PHONY: clean
clean:
	
	$(MAKE) -C src clean

.PHONY: distclean
distclean: clean
	rm -rf limine
	$(MAKE) -C src distclean