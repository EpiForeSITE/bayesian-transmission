build:
	R CMD build .

install:
	R CMD INSTALL .

check: build
	R CMD check --as-cran *.tar.gz

docs:
	Rscript -e 'devtools::document()'