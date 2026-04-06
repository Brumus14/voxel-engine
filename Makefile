CC = clang

src = $(shell find src lib -type f -name "*.c")
lib = glfw3 GL m cglm pthread
lib_include = lib/GLFW/lib lib/cglm/lib
include = lib/GLFW/include lib/glad/include lib/cglm/include lib/stb/include lib/FastNoiseLite
additional_flags = -Wl,-rpath=./lib/cglm/lib
performance_flags = -O3 -flto
debug_flags = -g -O0 -Wall -Wextra
output = voxel-engine

run:
	./build/$(output)
all:
	@$(CC) $(addprefix -I,$(include)) $(addprefix -L,$(lib_include)) $(src) $(addprefix -l,$(lib)) $(additional_flags) -o build/$(output)
performance:
	@$(CC) $(addprefix -I,$(include)) $(addprefix -L,$(lib_include)) $(src) $(addprefix -l,$(lib)) $(additional_flags) -o build/$(output) $(performance_flags)
debug:
	@$(CC) $(addprefix -I,$(include)) $(addprefix -L,$(lib_include)) $(src) $(addprefix -l,$(lib)) $(additional_flags) -o build/$(output) $(debug_flags)
address:
	@$(CC) $(addprefix -I,$(include)) $(addprefix -L,$(lib_include)) $(src) $(addprefix -l,$(lib)) $(additional_flags) -o build/$(output) -fsanitize=address -fno-omit-frame-pointer -g -O1
thread:
	@$(CC) $(addprefix -I,$(include)) $(addprefix -L,$(lib_include)) $(src) $(addprefix -l,$(lib)) $(additional_flags) -o build/$(output) -fsanitize=thread -g -O1
