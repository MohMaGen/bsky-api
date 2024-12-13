sources := $(wildcard *.c)
targets := $(patsubst %.c, ../build/probe_%, $(sources))

probes: mkdirs $(targets)

mkdirs:
	@mkdir -p ../build

$(targets): ../build/probe_%: %.c
	clang -o $@ $< -lm -lcurl -g -Wall
