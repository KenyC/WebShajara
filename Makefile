H_FILES=$(wildcard src/c/*.h)
C_FILES=$(wildcard src/c/*.c)
O_FILES=$(C_FILES:%.c=%.o)

EM_FLAGS=-O2 -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap", "allocate", "intArrayFromString"]'
GCC_FLAGS=-g -O0
# GCC_FLAGS=-O1
main: build/main.html build/main.js build/main.css


build/%: src/html/%
	cp -f $< $@  

build/main.js: $(O_FILES)
	emcc $(filter %.o,$^) $(EM_FLAGS) -o build/main.js

rebuild: clean build/main.html

############  GENERAL ###################################

%.o: %.c $(H_FILES) 
	emcc -c $< $(EM_FLAGS) -o $@


############  TEST ###################################
TEST_TREE_H_FILES=$(wildcard test/tree_src/*.h)
TEST_TREE_C_FILES=$(wildcard test/tree_src/*.c)
FILES_TO_TEST=tree.c geometry.c
TEST_TREE_O_FILES=$(TEST_TREE_C_FILES:%.c=%.o) test/build/tree.o test/build/geometry.o
TEST_TREE_FLAGS=-Isrc/c/
test/tree_src/%.o: test/tree_src/%.c $(TEST_TREE_H_FILES)
	gcc $(GCC_FLAGS) $(TEST_TREE_FLAGS) -c $< -o $@

test/build/%.o: src/c/%.c
	gcc $(GCC_FLAGS) $(TEST_TREE_FLAGS) -c $< -o $@

test/tree_src/cairo_canvas.o: test/tree_src/cairo_canvas.c $(TEST_TREE_H_FILES)
	gcc $(GCC_FLAGS) $(TEST_TREE_FLAGS) -c $< -o $@ -I/usr/include/cairo/ 

test/tree: $(TEST_TREE_O_FILES)  
	gcc $(GCC_FLAGS) $(TEST_TREE_O_FILES) -o $@ -l cairo



test_tree: test/tree
	valgrind --leak-check=yes $<

##########################################################

view:
	firefox build/main.html

emsdk:
	@echo Run "source ~/Programs/emsdk/emsdk_env.sh"

clean:
	rm -f build/*
	rm -f src/*.o