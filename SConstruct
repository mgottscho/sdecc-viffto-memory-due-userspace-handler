# Author: Mark Gottscho <mgottscho@ucla.edu>

import os

env = Environment(ENV = {'PATH': os.environ['PATH']})
env.Replace(CC = 'riscv64-unknown-elf-gcc')
env.Replace(AR = 'riscv64-unknown-elf-ar')
env.Append(CPPFLAGS = '-O3')
sources = ['memory_due.c', 'minipk.c']
env.StaticLibrary(target = 'sdecc', source = sources)
