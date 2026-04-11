.PHONY: all clean upload
.DELETE_ON_ERROR:

all: | .pio
	platformio run

.pio:
	-btrfs subvolume create .pio
	mkdir -p .pio

clean:
	rm -rf .pio/*

upload: | .pio
	platformio run -t upload
