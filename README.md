# WinNWT5

**NOTE**: this code is not stable and is not guaranteed to work flawlessly on your machine. Please read [Ownership](#ownership) section.

### About

WinNWT5 is a control program for NWT series of network analyzers designed by Helmut Knechtel DL1ALT, such as NWT7, NWT160, NWT500, etc..
The original program called **WinNWT4 / LinNWT4** is developed by Andreas Lindenau DL4JAL, who publishes source code on https://www.dl4jal.eu/. Back in the day WinNWT4 was based on Qt4. At around 2013, Andrey Sobol UB3TAF started a project to port it to Qt5, named the result **WinNWT5** and published it on http://www.asobol.ru/software/winnwt5. The last version published by him is from the end of 2017, so the project is not actively maintained at the moment.

I've acquired a piece of old NWT500 hardware, which is not supported by modern WinNWT4 (aka NWT4000Win). WinNWT5, on the other hand, works well with it, as it's based on a much older version of WinNWT4. Thus, I've fixed Linux support in UB3TAF's code, made a few more minor changes, and published the code in this repo.

### Ownership

I am **not** the owner of this project, and I do not plan to actively maintain it. If someone is interested, please feel free to create a fork ;-)

### Authors

Original WinNWT4 (c) **Andreas Lindenau DL4JAL**. WinNWT5 (c) **Andrey Sobol UB3TAF**.

### License

**GNU GPL version 2** (original code does not include any sort of LICENSE.txt, but most of the files mention GPL version 2 in the header).

### Dependencies

1. CMake 3.13+
2. Decent C/C++ toolchain (GCC 9.2.0 is confirmed to work)
3. Qt5 (version 5.13.2 is confirmed to work). You'll need following components: Core, Widgets, LinguistTools, PrintSupport and SerialPort

### Building

Make sure you have all the necessary dependencies, and run following commands:
```bash
# To generate makefiles
mkdir build
cd build
cmake ..

# To build
make

# To run
./winnwt5
```
