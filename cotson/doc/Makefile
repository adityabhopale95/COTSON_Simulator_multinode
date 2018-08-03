.PHONY: all

all: COTSON_USER_GUIDE-v4.pdf

COTSON_USER_GUIDE-v4.pdf: COTSON_USER_GUIDE-v4.tex
	pdflatex $^

clean:
	rm -f COTSON_USER_GUIDE-v4.pdf *.aux *.log *.lof *.lot *.toc *.out
