## Section 0: introduction

This tutorial describes how to set up C/C++ projects in Visual Studio Code. We will use a linux operating system (Ubuntu 24.04) and thus also the linux terminal for executing commands.

We will use the apt package manager to install the necessary tools. If a mistake was made, you can run

```shell
sudo apt-get purge packagename
sudo apt-get autoremove
```

## Section 1: the basics

Let's start by creating a folder `project01` with a very simple `main.cpp` file:

```c
#include <iostream>

int main() {
    std::cout << "Hello World!";
    return 0;
}
```

How do we execute this code? Well, it's complicated. The process of converting a C file to an executable involves the following steps:

1. **Preprocessing**: The preprocessor handles directives like `#include` and `#define`, expanding macros and including headers.

2. **Compilation**: The compiler translates the preprocessed C code into assembly code specific to the target architecture.

3. **Assembly**: The assembler converts the assembly code into machine code, producing an object file (`.o`).

4. **Linking**: The linker combines object files and libraries to resolve symbols and produce the final executable.

We refer to this whole process as **building**. We will use the CMake build system with the g++ compiler (gcc is for C only, but g++ works for both C and C++ files). Installing CMake will automatically install gcc, but not g++.

```shell
sudo apt-get install cmake g++
```

You can check the versions using

```shell
cmake --version
gcc --version
g++ --version
```

CMake uses a *CMakeLists.txt* file to build.

```cmake
cmake_minimum_required(VERSION 3.28) # Check CMake version
project(project01)


# Check GCC version
find_program(GCC_VERSION gcc)

if(NOT GCC_VERSION)
    message(FATAL_ERROR "GCC not found.")
endif()

execute_process(COMMAND ${GCC_VERSION} --version OUTPUT_VARIABLE GCC_VERSION_OUTPUT)
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" GCC_VERSION_MATCH ${GCC_VERSION_OUTPUT})
if(${GCC_VERSION_MATCH} VERSION_LESS "13.2")
    message(FATAL_ERROR "GCC version 13.2 or higher is required.")
endif()


# Check G++ version
find_program(GPP_VERSION g++)

if(NOT GPP_VERSION)
    message(FATAL_ERROR "G++ not found.")
endif()

execute_process(COMMAND ${GPP_VERSION} --version OUTPUT_VARIABLE GPP_VERSION_OUTPUT)
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" GPP_VERSION_MATCH ${GPP_VERSION_OUTPUT})
if(${GPP_VERSION_MATCH} VERSION_LESS "13.2")
    message(FATAL_ERROR "G++ version 13.2 or higher is required.")
endif()


add_executable(project01 main.cpp)
```

Finally, we specify a folder where we want to output the build and run CMake:

```shell
mkdir build
cd build 
cmake ..
```

But oh no, what are all those files and folders in our build? Shouldn't it just be a single executable? Yes, you're right. CMake actually doesn't build anything, but is just a widely accepted way to generate build files. Thanks to CMake, we don't need to worry about platform-specific details. Just provide the source code and the CMakeLists.txt file and you are all set. Interested readers can find more information [here](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Why%20CMake.html).

So how do we get an executable? Well, we just run the make command in the build folder:

```shell
make
```

If make is not installed yet, you can do so by running

```shell
sudo apt-get install make
```

And then finally, we run the executable

```shell
./project01
```

## Section 2: project structure & external libraries

We will adapt following structure:

```
project01
├── bin
├── build
├── include
│ ├── .h
├── libs
│ ├── external_library/
├── out
├── src
│ ├── .c
│ ├── .cpp
├── CMakeLists.txt
└── README.md
```

This means, of course, that we need to change our CMakeLists.txt file, but we will do that later. For now, we will focus on external libraries. There are a few types: **header-only**, **static** and **shared**. Every type has kind of the same structure, in generally an `include` folder and a `lib` folder. The include folder contains the header files. If the type is header-only, the file will be located here. If the type is static or shared, the lib folder will also contain some `.a` or `.so` files.

As example, we look at the *mysql* library.

1. **Download MySQL Client Library**
   
   Go to the [official website](https://dev.mysql.com/downloads/mysql/) and download the development headers package. In my case, i chose:
   
   - Version: 8.4.2 LTS
   
   - OS: Ubuntu Linux
   
   - OS version: 24.04

2. **Extract the Archive**
   
   Extract the .deb file:
   
   ```shell
   dpkg-deb -x libmysqlclient-dev_8.4.2-1ubuntu22.04_amd64.deb .
   ```

3. **Select the necessary files**
   
   We need only the include and lib folder. Create `libs/mysql` and move them there.

4. **Include the library*
   
   To start using the library, you have to include it:
   
   ```c
   #include <mysql/mysql.h>
   ```

*<u>note</u>: we use ` #include "something.h"` when we use our project's own header files from the include folder.*

Now we can update the CMakeLists.txt file so that it automatically load the external libraries, ***if they have the correct structure!***:

```cmake
# Minimum CMake version
cmake_minimum_required(VERSION 3.28)

# Project name and version
project(project01 VERSION 1.0)

# C++ and C standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Minimum CGG version
find_program(GCC_VERSION gcc)
if(NOT GCC_VERSION)
    message(FATAL_ERROR "GCC not found.")
endif()
execute_process(COMMAND ${GCC_VERSION} --version OUTPUT_VARIABLE GCC_VERSION_OUTPUT)
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" GCC_VERSION_MATCH ${GCC_VERSION_OUTPUT})
if(${GCC_VERSION_MATCH} VERSION_LESS "13.2")
    message(FATAL_ERROR "GCC version 13.2 or higher is required.")
endif()

# Minimum G++ version
find_program(GPP_VERSION g++)
if(NOT GPP_VERSION)
    message(FATAL_ERROR "G++ not found.")
endif()
execute_process(COMMAND ${GPP_VERSION} --version OUTPUT_VARIABLE GPP_VERSION_OUTPUT)
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" GPP_VERSION_MATCH ${GPP_VERSION_OUTPUT})
if(${GPP_VERSION_MATCH} VERSION_LESS "13.2")
    message(FATAL_ERROR "G++ version 13.2 or higher is required.")
endif()

# Header files
include_directories(include)
file(GLOB_RECURSE INCLUDES "include/*")

# Source files
include_directories(src)
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
add_executable("${PROJECT_NAME}" ${SOURCES})

#= External Libraries =======================================================================#
set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")

function(add_library_from_dir libname)
    set(LIB_DIR "${LIBS_DIR}/${libname}")
    if (EXISTS "${LIB_DIR}")
        message(STATUS "Found library: ${libname}")
        include_directories("${LIB_DIR}/include")
        file(GLOB_RECURSE LIB_SOURCES "${LIB_DIR}/lib/*.a")
        if (LIB_SOURCES)
            target_link_libraries("${PROJECT_NAME}" ${LIB_SOURCES})
        endif()
    else()
        message(WARNING "Library directory ${LIB_DIR} not found.")
    endif()
endfunction()

file(GLOB directories LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/libs/*")
foreach(dir ${directories})
    if(IS_DIRECTORY ${dir})
        get_filename_component(dir_name ${dir} NAME)
        add_library_from_dir("${dir_name}")
    endif()
endforeach()
#============================================================================================#
```

*<u>note</u>: we have to do use `mv project01 ../bin/` to make sure the executable ends up in the bin folder.*

## Section 3: extensions and tasks

The <mark>C/C++ extension</mark> (ms-vscode.cpptools) adds:

- **IntelliSense**: Provides smart code completions based on variable types, function definitions, and imported modules.

- **Code Navigation**: Allows you to navigate through your codebase with features like Go to Definition, Find References, and Peek Definition.

- **Error and Warning Squiggles**: Highlights errors and warnings in the code, helping to catch issues early.

- **Code Formatting**: Automatically formats your C/C++ code based on the configuration you set.

- **Debugging**: Integrates with the debugger to allow you to set breakpoints, step through code, and inspect variables.

- **Configuration Support**: Helps manage configuration files for compiling and debugging, such as tasks.json and launch.json.

- ...

The <mark>CMake extension</mark> (twxs.cmake) adds equivalent tools for CMakeLists files.

To make our life easier, we use tasks in VSCode. Create a folder `.vscode` with a `tasks.json` file in it.

```json
{
    "version": "2.0.0",
    "tasks": [
        // Build
        {
            "label": "build (debug)",
            "type": "shell",
            "command": "cd ${workspaceFolder}/build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && mv project01 ../bin/ && cd .. && rm -rf build/*",
            "problemMatcher": ["$gcc"],
        },
        {
            "label": "build (release)",
            "type": "shell",
            "command": "cd ${workspaceFolder}/build && cmake -DCMAKE_BUILD_TYPE=Release .. && make && mv project01 ../bin/ && cd .. && rm -rf build/*",
            "problemMatcher": ["$gcc"],
        },

        // Run
        {
            "label": "run",
            "dependsOn": ["build (release)"],
            "dependsOrder": "sequence",
            "type": "shell",
            "command": "./bin/project01",
            "problemMatcher": []
        },
    ]
}
```

*<u>note</u>: We delete the contents of build after getting our executable to prevent issues with caching when we use gdb for example.*

*<u>note</u>: More information about the different build types will be specified later. For now, just assume we build for release only.*

## Section 4: debugging

As an example, we will rewrite our main.cpp file to have a bug!

```cpp
#include <mysql/mysql.h>
#include <iostream>

int main() {
    std::cout << "Hello World!" << std::endl;

    int d = 2;
    scanf("%d", d);
    printf("You gave me: %d", d);

    return 0;
}
```

Normally, when we compile our source code, we don't include any debug data. We can indicate that we actually do want this by including adding some specific flags in CMakeLists.txt. We will do this for 2 different build types: **Release** and **Debug**.

```cmake
# Build Type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3")
endif()
```

We leave the understanding of gbd and it's commands to the reader. For completeness sake, the reader can try to find the bug using these commands. (hint: gcc already gives us basically all the bug info when building)

- **gdb ./bin/project01**
  
  - Starts gdb.

- **lay next**
  
  - Puts the C code and the Assembly code next to each other for a nice visual overview.  Other combinations are possible as well. It might be possible that you have to press enter to load the correct view.

- **break main**
  
  - Puts a breakpoint at the tag main. There is only one function so we know it has to be there. 

- **run**
  
  - Starts running the program and stops at a breakpoint. It might be possible that you have to select whether or not you want to use debuginfod. You can just press yes. This is not necessary for rr, as it is a replay.

- **next**
  
  - Go to the next C code line without going into function calls (step over).
  
  - The nexti variant does the same for an Assembly line.

- **step**
  
  - Same as next but if it's possible, we step into a function call.

- **ref** 
  
  - Refresh GDB in case it has some problems. Don't worry, you will stay at the same line and everything will stay intact.

- **x/i $pc** 
  
  - Examine the instruction at program counter. This is especially usefull when something went wrong and we want more details about what Assembly line when we stepped over a call.

- **info registers**
  
  - See the data in the registers. Especially usefull in combination with *x/i $pc*.

- **continue (c)**
  
  - Go on until error or next breakpoint.

Now, gdb is all nice, but what would be really usefull is reverse debugging. Basically going back some lines. rr is a tool that records an execution (+ the failure) so as to achieve this goal. You can install it [here](https://rr-project.org/). 

- **rr record ./bin/project01**

- while rr record ./bin/project01 ; do ; done

- **rr replay**

After rr replay, we just use it normally like gdb, except we can now do things like:

- **reverse-next**

- **reverse-step**

For convenience sake (and not having to remember all the commands), we will create these VSCode tasks:

```json
{
    "label": "debug",
    "type": "shell",
    "command": "rr record ./bin/project01 && rr replay -- -q",
    "problemMatcher": []
},
{
    "label": "build + debug",
    "dependsOn": ["build", "debug"],
    "dependsOrder": "sequence",
    "problemMatcher": []
},
```

*<u>note</u>: To prevent the popup for pagination and debuginfod, you can create a file `~/.gdbinit` with following contents:*

```
set pagination off
set debuginfod enabled off
```

*A problem that might remain is that the copyright banner of gdb is displayed before actually loading .gdbinit, which can cause a popup for pagination. That is also why we use `rr replay -- -q` where -q is a gdb specific option to be quiet and not show the banner.*

## Section 5: Memory leaks

To detect memory leaks we use Valgrind.

```shell
sudo apt install valgrind
```

Let's introduce a memory leak in our program:

```cpp
#include <mysql/mysql.h>
#include <iostream>

int main() {
    std::cout << "Hello World!" << std::endl;

    int d = 2;
    scanf("%d", d);
    printf("You gave me: %d", d);

    int *ptr = (int *) malloc(sizeof(int));  
    *ptr = 10;  
    printf("Value of ptr: %d\n", *ptr);  

    return 0;
}
```

After building, we can use the command

```shell
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=out/valgrind-out.txt ./bin/project01
```

The option verbose might give a lot of bloat, so can be omitted if necessary. Here, we write the output to a `valgrind-out.txt` but ommitting this option will just print to the terminal.

VSCode task:

```json
// Detect leaks
{
    "label": "detect leaks",
    "dependsOn": ["build (debug)"],
    "dependsOrder": "sequence",
    "type": "shell",
    "command": "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt ./bin/project01",
    "problemMatcher": []
},
```

## Section 6: Profiling

#### Memory

For profiling memory, we will again use valgrind.

```shell
valgrind --tool=massif --massif-out-file=out/massif.out ./bin/project01
```

To interpret this output data, we'll look at two tools:

1. **ms_print** for a textual view
   
   ```shell
   ms_print out/massif.out > out/massif_report.txt
   ```

2. **massif-visualizer** for a graphical view
   
   ```shell
   sudo apt install massif-visualizer
   
   massif-visualizer out/massif.out
   ```

VSCode task:

```json
// Profile memory
{
    "label": "profile memory",
    "dependsOn": ["build (debug)"],
    "dependsOrder": "sequence",
    "type": "shell",
    "command": "valgrind --tool=massif --massif-out-file=out/massif.out ./bin/project01 && massif-visualizer out/massif.out",
    "problemMatcher": []
},
```

#### Execution Time

For profiling execution time, we will use perf.

```shell
sudo apt-get install linux-tools-common linux-tools-generic linux-tools-$(uname -r)

sudo mkdir -p /usr/share/doc/perf-tip 
sudo wget https://raw.githubusercontent.com/torvalds/linux/master/tools/perf/Documentation/tips.txt -O /usr/share/doc/perf-tip/tips.txt
```

Again, we will have two different ways:

1. **perf report** for a textual view
   
   ```shell
   perf record -o out/perf.data ./bin/project01
   perf report -i out/perf.data > out/perf_report.txt
   ```

2. **FlameGraph**  for a graphical view
   
   ```shell
   git clone https://github.com/brendangregg/FlameGraph out/FlameGraph
   
   perf record -o out/perf.data -g ./bin/project01
   perf script -i out/perf.data | ./out/FlameGraph/stackcollapse-perf.pl > out/out.perf-folded
   ./out/FlameGraph/flamegraph.pl out/out.perf-folded > out/perf.svg
   brave-browser out/perf.svg # or chrome, firefox, etc.
   ```

VSCode task:

```json
// Profile execution time
{
    "label": "profile time",
    "dependsOn": ["build (debug)"],
    "dependsOrder": "sequence",
    "type": "shell",
    "command": "perf record -o out/perf.data -g ./bin/project01 && perf script -i out/perf.data | ./out/FlameGraph/stackcollapse-perf.pl > out/out.perf-folded && ./out/FlameGraph/flamegraph.pl out/out.perf-folded > out/perf.svg && brave-browser out/perf.svg",
    "problemMatcher": []
},
```
