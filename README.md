# muslib

Music library manager

## run
```
```

## build
```
python -m build -n -w     # wheel only, no venv
```

Customizations: 
- pyproject.toml: build backend in buildr/testbackend
- buildr/testbackend.py: custom version of setuptools/setuptools/build_meta.py

In directory buildr.

- Pre-process C files with embedded python.
- Make system to build C projects, with dependencies automatically determined from source code.

This can be used for standalone C projects or combined Python and C projects. The goal is to
programmatically generate where possible C "boilerplate" code in general, Python bindings for C
code in particular, as well as Python code where helpful, e.g. to deal with data shared between
C and Python like enumerations.

Relation to other projects:
- cffi: this should greatly reduce the need for cffi code in python.

- cython: honestly, I just found out about cython, but my first thought is that a combo of this
  and cython could be quite useful, giving cython the ability to parse and analyze existing C code.

This project is against either C or Python being the "dominant" paradigm. In some fantastical
ideal of mine, C, Python, and all other languages could be combined as naturally as kids who
immigrate at age 4 combine their pre- and post-immigration languages, confused when 20 years
later you ask them what their native language is. If this implies parsing all such languages
into a common AST, a super-language that supports all the features of each of these languages
that can be compiled to machine code as much as possible, compiler and interpreter as library,
with all internals exposed, so be it.
