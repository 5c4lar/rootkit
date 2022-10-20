cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project("Linux Kernel Module with CLion IDE support / CMake" VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find kernel headers
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(KernelHeaders REQUIRED)


# find MODULE_LICENSE("GPL"), MODULE_AUTHOR() etc.
# thanks to "merseyviking" from stack overflow
add_definitions(-D__KERNEL__ -DMODULE)

# this is needed in order for CLion IDE to provide syntax highlightning
# this is independent from the actual kernel object that is built
add_executable(dummy
        # add all *.h and *.c files here that # CLion should cover
        main.c
        greet.c
        example.c
        ftrace_helper.h hook_random.c hook_random.h setroot.c setroot.h hide_module.c hide_module.h hook_kill.c hook_kill.h reveal_ftrace.c reveal_ftrace.h hide_port.c hide_port.h hide_process.c hide_process.h kill.c log_clean.c log_clean.h hide_driver.c hide_driver.h hook_mkdir.c hook_mkdir.h)

# CLion IDE will find symbols from <linux/*>
target_include_directories("dummy" PRIVATE ${KERNELHEADERS_INCLUDE_DIRS})

# Include current dir
target_include_directories("dummy" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})