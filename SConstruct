# Author: Mark Gottscho <mgottscho@ucla.edu>

import os

env = Environment(ENV = {'PATH': os.environ['PATH']})
env.Replace(CC = 'riscv64-unknown-elf-gcc')
env.Append(CPPFLAGS = '-g')
env.Append(LIBS = '-lm')
sources = ['hello.c', 'foo.c', 'handlers.c', 'memory_due.c', 'minipk.c']
env.Program(target = 'hello', source = sources)
