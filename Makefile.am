# Written by Salonia Matteo on 16/05/2021

# Which compiler to use
CC = gcc

# LINKER: choose a linker to use; can be bfd, gold, lld
# comment to use the default linker, uncomment to use a custom linker
#LINKER = -fuse-ld=gold

# CSTD: which C revision to use
CSTD = -std=c99

# Optimizations. Can be -O0, -O1, -O2, -O3, -Os, -Ofast
OPTS = -Ofast

# CPU-Specific Optimizations
CPUOPTS = -march=native -mtune=native

# CFLAGS: additional compiler flags
override CFLAGS = -Wall $(CSTD) $(LINKER) $(OPTS) $(CPUOPTS)

# Executable name
bin_PROGRAMS = quiz

# Source code location
quiz_SOURCES = src/quiz.c

# Data installation directory
docdir = /usr/local/share/quiz

# Various Documentation & Data files
doc_DATA = README.md quiz.db

# Clean the project directory
cleandir:
	make mostlyclean
	make clean
	make distclean
	rm -rf autom4te.cache aclocal.m4 *.tar.gz

version:
	@echo ${VERSION}

info:
	@echo "${PACKAGE_STRING} made by ${AUTHOR}."
	@echo "Report any bugs to ${PACKAGE_BUGREPORT}."
	@echo "Homepage: ${PACKAGE_URL}"
	@echo "Support this project: ${PACKAGE_DONATE}"

# Alias "uninstall-docDATA" to "uninstall-doc"
uninstall-doc: uninstall-docDATA
