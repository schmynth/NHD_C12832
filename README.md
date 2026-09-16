# Template for C++ projects

This repository serves as a starting point for new C++-Projects in VS Code and as a knowledge database for structuring projects.

## Folder Structure

The folder structure is as follows:

```
.vscode/    # VS Code config, build and task files
include/    # Header files (.h)
src/        # source files (.cpp)
bin/        # compiled binaries, excluded via .gitignore
LICENSE
Makefile    
README.md
.gitignore
```

## Files

I included the include/*.h files and their src/*.cpp counterparts as an example how to set up header files.

## Header Guards

Header guards ensure that functions are not defined multiple times, which leads to linker errors.
They should be applied in .h-Files.

```
#ifndef CHEF_H
#define CHEF_H

// declarations here

#endif
```
