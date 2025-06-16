PROJECT=PiCoaster2040

TOOLCHAIN_PREFIX=arm-none-eabi-

BOARD=pico

build/$(PROJECT).uf2:

flash : build/$(PROJECT).uf2
	picotool load $<
	picotool reboot

build/$(PROJECT).uf2 build/$(PROJECT).elf: build FORCE
	$(MAKE) -C build
	$(TOOLCHAIN_PREFIX)size build/$(PROJECT).elf
	$(TOOLCHAIN_PREFIX)nm --print-size --size-sort --radix=d build/$(PROJECT).elf | awk '{printf("%5d %s\n", $$2, $$4);}' | sort -nr | head -20

disasm: build/$(PROJECT).elf
	$(TOOLCHAIN_PREFIX)objdump -C -S build/$(PROJECT).elf

build:
	cmake -B build -DCMAKE_VERBOSE_MAKEFILE=ON -DPICO_BOARD=$(BOARD)

clean:
	rm -rf build

FORCE:
