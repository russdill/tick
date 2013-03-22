GEN_SYM=\
	symbols/generated/beaglebone-p8-1.sym \
	symbols/generated/beaglebone-p9-1.sym \
	symbols/generated/beaglebone-j1-1.sym \
	symbols/generated/beaglebone-mhole-1.sym \
	symbols/generated/ft2232h-1.sym \
	symbols/generated/usb2412-1.sym \
	symbols/generated/lm3671-1.sym \
	symbols/generated/74245-1.sym \
	symbols/generated/743257-1.sym \
	symbols/generated/rclamp0514m-1.sym \
	symbols/generated/jtag-1.sym \
	symbols/generated/rclamp0502a-1.sym \
	symbols/generated/tdp4e002-1.sym

PCB=~/src/pcb/_install/bin/pcb
PCB_BATCH=~/src/pcb/_install/bin/pcb
GSCH2PCB=~/src/pcb/gsch2pcb

SCHEMATICS=tick_1.sch tick_2.sch tick_3.sch

all: tick_front.png tick_back.png tick_schematic.pdf tick.pdf gerber

tick_schematic.pdf: tick_1.ps tick_2.ps tick_3.ps
	gs -q -dQUIET -dBATCH -dNOPAUSE \
		-sDEVICE=pdfwrite -sOutputFile="$@" $^

project: tick.project $(SCHEMATICS)
	$(GSCH2PCB) -s -f -d ./packages $<

symbols: $(GEN_SYM)

%.sym: %.symdef
	djboxsym "$<" > "$@"

%.ps: %.sch
	gschem -p -o$@ -s/usr/share/gEDA/scheme/print.scm $<

#tick.zip: tick.pcb Makefile
#	tmp=$(shell mktemp -d); cd $$tmp; \
#	$(PCB) -x gerber --verbose --all-layers $(PWD)/$<

tick.zip: tick_gerber Makefile
	zip $@ tick.{{bottom{,mask,silk},top{,mask,silk},group[1-2],outline,}.gbr,plated-drill.cnc}

oshpark.zip: tick_gerber
	@-rm -rf oshpark/
	mkdir oshpark
	cp tick.top.gbr oshpark/Top\ Layer.ger
	cp tick.bottom.gbr oshpark/Bottom\ Layer.ger
	cp tick.topmask.gbr oshpark/Top\ Solder\ Mask.ger
	cp tick.bottommask.gbr oshpark/Bottom\ Solder\ Mask.ger
	cp tick.outline.gbr oshpark/Board\ Outline.ger
	./merge_drill.sh tick.{un,}plated-drill.cnc > oshpark/Drills.xln
	-rm oshpark.zip
	cd oshpark && zip ../oshpark.zip *.ger *.xln

.PHONY: tick_gerber
tick_gerber: tick.pcb
	$(PCB) -x gerber --verbose --all-layers $<

.PHONY: png
png: tick_front.png tick_back.png

%.ps: %.pcb Makefile
	$(PCB) -x ps --ps-color --align-marks --media Letter --psfade .5 --psfile $@ $<

%.pdf: %.ps
	ps2pdf $< $@

DPI=500
COLOR=purple
tick_front.png: tick.pcb Makefile
	$(PCB_BATCH) -x png --outfile $@ --dpi $(DPI) --use-alpha --photo-mode --photo-mask-colour $(COLOR) --photo-plating gold $<

tick_back.png: tick.pcb Makefile
	$(PCB_BATCH) -x png --outfile $@ --dpi $(DPI) --use-alpha --photo-mode --photo-mask-colour $(COLOR) --photo-plating gold --photo-flip-x $<


clean:
	-rm -f *.ps *.pdf *.gbr *.cnc *.png

