PDF = pdflatex
BIB = biber
PDFFLAGS = -synctex=1 -interaction=nonstopmode -file-line-error -shell-escape

FILE = report
FIGURES = $(shell echo ../fig/*/*)

.PHONY: clean

$(FILE).pdf: $(FILE).tex $(FIGURES)
		$(PDF) $(PDFFLAGS) $<
		$(BIB) $(FILE)
		$(PDF) $(PDFFLAGS) $<
		$(PDF) $(PDFFLAGS) $<
		make clean

clean:
		rm -rf *.out 
		rm -rf *.bbl 
		rm -rf *.log
		rm -rf *.ind
		rm -rf *.ilg
		rm -rf *.lot
		rm -rf *.lof
		rm -rf *.ind
		rm -rf *.idx
		rm -rf *.aux
		rm -rf *.toc

