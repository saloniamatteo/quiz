# CC: can be gcc, clang, or a compiler of your choice

# Cross-compiler
#CC = aarch64-linux-gnu-gcc

# Normal compiler
CC = gcc

# Statically linked builds must be made using musl-gcc,
# because the static libraries (readline, ncurses) do not
# work with glibc
STATIC_CC = musl-gcc

# Optimizations. Can be -O0, -O1, -O2, -O3, -Os, -Ofast
OPTS = -Ofast

# Architecture, Tune. Set to 1 to disable.
NOARCHTUNE = 0

# Disable ARCH & TUNE if using cross-compiler,
# or if compiling on ARM.
ifeq ($(CC), aarch64-linux-gnu-gcc)
NOARCHTUNE = 1
else ifeq ($(shell uname -m), "aarch64")
NOARCHTUNE = 1
endif

# Check if NOARCHTUNE is 0 (enable ARCH & TUNE)
ifeq (NOARCHTUNE, 0)
	ARCH = -march=native
	TUNE = -mtune=native
endif

# CFLAGS: additional compiler flags
CFLAGS = -Wall
# LINKER: choose a linker to use; can be bfd, gold, lld
# comment to use the default linker, uncomment to use a custom linker
#LINKER = -fuse-ld=gold
# CSTD: which C revision to use
CSTD = -std=c99

# Dynamic Linking
quiz: quiz.o
	@$(CC) $^ -o quiz $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(CSTD) $(LDFLAGS)
	@echo "CC $<"

quiz.o: quiz.c
	@$(CC) -c $< -o $@ $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE)
	@echo "CC $<"

# Static Linking 
static: quiz-static.o
	@$(STATIC_CC) $^ -o quiz --static $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(CSTD) $(INCLUDE_PATHS)
	@echo "CC $<"

quiz-static.o: quiz.c
	@echo "[STATIC]"
	@$(STATIC_CC) -c $< -o $@ -static $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(INCLUDE_PATHS)
	@echo "CC $<"

# Generate tags with cscope and ctags
gentags:
	@echo "Generating tags..."
	$(shell cscope -bkq)
	$(shell find . -type f -name "*.[chsS]" -print > cscope.files)
	$(shell ctags -L cscope.files)
	@echo "Done! "

clean:
	rm -f *.o quiz cscope* tags

.PHONY = clean gentags static quiz
