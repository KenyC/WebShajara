H_FILES=$(wildcard src/c/*.h)
C_FILES=$(wildcard src/c/*.c)
O_FILES=$(C_FILES:%.c=%.o)

EM_FLAGS=-O2 -DWEB -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap", "allocate", "intArrayFromString"]'
# EM_FLAGS+= -DDEBUG
GCC_FLAGS=-g -O0 -DDEBUG
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


############  TEST TREE ###################################
TEST_TREE_H_FILES=$(wildcard test/tree_src/*.h)
TEST_TREE_C_FILES=$(wildcard test/tree_src/*.c)
FILES_TO_TEST_TREE=tree.c  tree_output.c tree_parse.c geometry.c
TEST_TREE_O_FILES=$(TEST_TREE_C_FILES:%.c=%.o) $(FILES_TO_TEST_TREE:%.c=test/build/%.o)
TEST_TREE_FLAGS=-Isrc/c/
test/tree_src/%.o: test/tree_src/%.c $(TEST_TREE_H_FILES)
	gcc $(GCC_FLAGS) $(TEST_TREE_FLAGS) -c $< -o $@

test/build/%.o: src/c/%.c
	gcc $(GCC_FLAGS) $(TEST_TREE_FLAGS) -c $< -o $@

test/tree_src/cairo_canvas.o: test/tree_src/cairo_canvas.c $(TEST_TREE_H_FILES)
	gcc $(GCC_FLAGS) $(TEST_TREE_FLAGS) -c $< -o $@ -I/usr/include/cairo/ 

test/tree_src/data/%.txt: test/tree_src/data/%
	python scripts/sanitize_tree.py $<

DATA_FILES=$(wildcard test/tree_src/data/*.data)
test/tree_src/output.o: test/tree_src/output.c $(TEST_TREE_H_FILES) $(DATA_FILES:%=%.txt)
	gcc $(GCC_FLAGS) $(TEST_TREE_FLAGS) -c $< -o $@ 

test/tree: $(TEST_TREE_O_FILES)  
	gcc $(GCC_FLAGS) $(TEST_TREE_O_FILES) -o $@ -l cairo

test_tree: test/tree
	valgrind --leak-check=yes $<


############  TEST GEOMETRY ###################################
TEST_GEOMETRY_H_FILES=$(wildcard test/geometry_src/*.h)
TEST_GEOMETRY_C_FILES=$(wildcard test/geometry_src/*.c)
FILES_TO_TEST_GEOMETRY=geometry.c
TEST_GEOMETRY_O_FILES=$(TEST_GEOMETRY_C_FILES:%.c=%.o) $(FILES_TO_TEST_GEOMETRY:%.c=test/build/%.o)
TEST_GEOMETRY_FLAGS=-Isrc/c/
test/geometry_src/%.o: test/geometry_src/%.c $(TEST_GEOMETRY_H_FILES)
	gcc $(GCC_FLAGS) $(TEST_GEOMETRY_FLAGS) -c $< -o $@

test/build/%.o: src/c/%.c
	gcc $(GCC_FLAGS) $(TEST_GEOMETRY_FLAGS) -c $< -o $@

test/geometry_src/cairo_canvas.o: test/geometry_src/cairo_canvas.c $(TEST_GEOMETRY_H_FILES)
	gcc $(GCC_FLAGS) $(TEST_GEOMETRY_FLAGS) -c $< -o $@ -I/usr/include/cairo/ 

test/geometry: $(TEST_GEOMETRY_O_FILES)  
	gcc $(GCC_FLAGS) $(TEST_GEOMETRY_O_FILES) -o $@ -l cairo

test_geometry: test/tree
	valgrind --leak-check=yes $<



##########################################################

view:
	firefox http://0.0.0.0:8000/build/main.html

emsdk:
	@echo Run "source ~/Programs/emsdk/emsdk_env.sh"

clean:
	rm -f build/*
	rm -f src/*.o