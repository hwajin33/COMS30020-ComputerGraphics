## Introduction and Orientation
### <a href='https://uob.sharepoint.com/:f:/r/teams/UnitTeams-COMS30020-2022-23-TB-1-A/Shared%20Documents/General/Recordings/View%20Only?csf=1&web=1&e=aTmZb2' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction


The aim of this introductory workbook is to ease you gently into the first week of term. The main objective is to make sure you are able to build and run the code template that is the starting point for all of the work in this unit. Getting this working _might_ take some time and perseverance to achieve (depending on the operating system you intend to use for this unit). It is **ESSENTIAL** that you achieve everything in this workbook during the first week of term. If you are unable to build and run applications, you will find yourself falling behind on this unit.

If you didn't manage to join the introduction briefing session, make sure you have watched the "Weekly Briefing" video linked to at the top of this page before attempting the tasks in this workbook !  


#
### Task 2: Language and Libraries


The implementation language for this unit will be C++. Everybody will have their own preferred language for writing code, however C++ is very much the standard for low-level graphics rendering applications. If we didn't use this as the primary language for this unit, potential employers would think it very unusual.

The aim of this unit is to develop a fundamental understanding of graphics rendering and as such, we will NOT be making use of any existing rendering frameworks (but rather, building our own !). That said, the practical exercises will make use of the following two external libraries:
- <a href="http://glm.g-truc.net" target="_blank">GLM</a> for general purpose mathematical operations (mostly for manipulating vectors and matrices)
- <a href="http://www.libsdl.org" target="_blank">SDL2</a> for drawing to the screen (mostly setting the colour of individual pixels !)

Remember: The aim of this unit is to learn to build things from the ground-up, so don't use anything other than SDL2 and GLM when implementing the practical exercises.  

In order to make things a little tidier, we have provided a "wrapper" object around SDL2 called `DrawingWindow` . This wrapper initialises SDL, opens up a window for drawing to the screen and then provides various methods for drawing and interacting with the user. The `DrawingWindow` object has the following methods:  

- `DrawingWindow` constructor for the drawing window that takes 3 parameters:
    - Horizontal resolution for the window (integer width)
    - Vertical resolution for the window (integer height)
    - Whether or not to show the window in full-screen mode (boolean)
- `setPixelColour` sets an individual pixel (indicated by x and y coordinates) to a specified colour (a packed ARGB value)
- `getPixelColour` returns the colour of the pixel indicated by x and y coordinates
- `renderFrame` draws all pixels to the screen (until you call this, all pixel changes are just in a memory buffer)
- `savePPM` saves the currently rendered content of the window to a .ppm image file
- `saveBMP` saves the currently rendered content of the window to a .bmp image file
- `clearPixels` clears all pixel colours (from the whole window)
- `pollForInputEvents` checks to see if there are any events waiting to be processed (returns true if there are !)

Examples of how to call the methods of the `DrawingWindow` class are included in the template project introduced later in this workbook.



#
### Task 3: Using Your Own Computer with an Existing OS


We have tried to use a programming language and libraries that are agnostic to the underlying architecture and operating system (so it should be _fairly_ straight-forward to get things running on most machines !). Note that we will test and mark the coursework assignment on the lab machines, so you should not use any platform-specific features.

The template project we have provided assumes that you have the `clang` complier installed (which many of you may already have !). If you do not currently have `clang`, you can either install it now or alternatively (if you have a different compiler already installed) update the `COMPILER` variable in the project Makefile depending on what you have installed (so that it uses `g++` or `cl` for example).

You can use an IDE for development if you wish (CLion, Visual Studio, Eclipse, XCode etc). We have provided a CMake file inside the project root folder to help you configure and build your project. If you do use an IDE make sure that your project compiles without it (i.e. by providing a Make file that works on the lab machines).

Installation of SDL2 can sometimes be non-trivial (due to the fact that it must link with native graphics libraries). For this reason, the recommended way to install it is using a package manager (e.g. apt, rpm, yum, brew, ports, pacman etc). The name of the required SDL2 package will vary depending on your package manager. Here are some likely candidates for a range of different package managers:
```
apt install libsdl2-dev
rpm -i SDL2-devel
yum install SDL2-devel
brew install sdl2
port install libsdl2
pacman -S sdl2
```

If you can't get your package manager to install SDL2 (or if you don't have a package manager at all !) then you will have to install it manually. Various useful resources are available online for achieving this, but you might like to start by taking a look at <a href="https://lazyfoo.net/tutorials/SDL/01_hello_SDL/" target="_blank"> this tutorial</a>. If you are really struggling to install SDL on your existing operating system, then the following sections provide some alternative approaches you might like to consider.  


**Hints & Tips:**  
The code template (that we will introduce later in this workbook) comes bundled with the GLM library built-in, so there is no need for you to install this yourselves !



#
### Task 4: Using Your Own Computer with a Fresh OS


If you are having difficulties installing SDL2 on your existing operating system, you might find things easier using a fresh operating system install. We have created a <a href="Vagrantfile" target="_blank">Vagrant Box configuration file</a> that can be used to bring up a virtual machine with all of the required tools and libraries installed. See the comments at the top of the configuration file for instructions on how to get started with Vagrant.

Alternatively (if you are confident to do so) it might be worth considering a "fresh" install of your preferred linux distribution. It should be relatively straight-forward to install SDL2 on a clean linux install using a decent package manager. You might even like to consider dual booting your computer (if you are brave enough to do so !)  


**Hints & Tips:**  
If using a virtual machine, you should avoid using display scaling (i.e. use VirtualBox in 100% "Unscaled Output" mode) otherwise you may experience a reduction in window manager performance.  


#
### Task 5: Using the Lab Machines


As an alternative to getting SDL2 installed on your own computer, you might choose to compile and run your code on the lab machines in MVB (which have all of the required software installed). The lab machines can either be used physically (whilst you are actually in the lab) or remotely (via remote desktop services). In order to gain remote access you will need to use the _X2Go_ client. Full instructions for installing and running the _X2Go_ client can be found in <a href="https://uob.sharepoint.com/sites/itservices/SitePages/fits-engineering-linux-x2go.aspx" target="_blank">this how-to guide</a>. It is worth noting however that the quality of your experience using this approach will vary depending on the quality of your internet connection.





**Hints & Tips:**  
For help resolving problems with remote access to the lab machines, check out the
<a href="https://uob.sharepoint.com/sites/itservices/SitePages/contacts.aspx" target="_blank">IT services help page</a>
in particular the rapid response teaching support helpline !

If you get a warning that `GLX is not supported` when remote logging in to the lab machines using X2Go then you will need to disable hardware acceleration in the `RedNoise` project. Open the `libs/sdw/DrawingWindow.cpp` file and change the line `uint32_t flags = SDL_WINDOW_OPENGL;` so that it now reads `uint32_t flags = SDL_WINDOW_FOREIGN;` This will disable hardware acceleration and _should_ solve the problem - it may run more slowly, but at least it will run !  


#
### Task 6: The Template Project


In order to help get you started, we have provided a template project called <a href="https://github.com/COMS30020/RedNoise" target="_blank">RedNoise</a>. This project provides the structure that you will need for the weekly workbooks, as well as illustrating the use of the `DrawingWindow` methods. In order to work with the template project, you must first **FORK** the <a href="https://github.com/COMS30020/RedNoise" target="_blank">repository</a> into **your own GitHub workspace**.

Once you have created a fork, clone it locally so that you then extend it with your own code. Periodically push the additions you had made to the project up onto your GitHub repository. It is **essential** that you create a fork first and don't just clone the original repository (because you don't have permission push changes into the COMS30020 GitHub organisation !) We will use your repository during the final marking process, so it is essential that you use GitHub throughout this unit.

In order to help you compile, link and run the `RedNoise` project, a Makefile has been provided for you inside the project root folder. This makefile contains multiple rules to build code for different purposes:

- debug: A development build rule that will compile and link your project for use with a debugger (gdb)
- diagnostic: A development build rule that includes extra memory checking and diagnosis flags.  
Note: this rule requires the <a href="https://clang.llvm.org/docs/AddressSanitizer.html" target="_blank">Address Sanitizer</a> library to be installed (which comes built-in with some C++ compilers)
- speedy: A build rule that will result in an optimised "high performance" executable (to make interaction testing quicker and easier)
- production: A build rule to make an executable without debug hooks (for release or distribution)

Just typing `make` on its own will build the project using the _debug_ rule and run the resultant executable.

When you have successfully built and run the `RedNoise` project you should see a window that looks like the below animation. Remember that the cleanest way to quit an SDL application is by pressing the ESC key ! If you have a window that you can't close (which does happen from time to time), you may need to kill off the process manually using Task Manager, Activity Monitor or the kill/killall command (depending on your platform).  


![](06%20The%20Template%20Project/images/RedNoise.gif)

**Hints & Tips:**  
On some platforms you may need to include `<cstring>` in the `DrawingWindow` class (if the compiler complains that it can't find `memset`).

On some platforms you might need to alter the `include` statement in the `DrawingWindow.h` file from `"SDL.h"` to `"SDL2/SDL.h"` to allow the compiler to find the SDL library.  


#
### Task 7: Understanding the Template


It is useful to spend a little time getting to know the structure of the <a href="https://github.com/COMS30020/RedNoise/blob/main/src/RedNoise.cpp" target="_blank">main class</a> in the template project. We are going to be using it a lot over the next few weeks, so it is going to be important. The main function in the template contains a "forever" loop which:

1. Polls the event queue for incoming mouse and keyboard events
2. Draws the scene to the screen memory buffer
3. Renders the memory buffer onto the SDL window

Drawing the scene onto the screen buffer basically involves looping through a pixel at a time, deciding what colour it should be, packing that colour into an integer and setting the relevant pixel to that colour. The `RedNoise` example just creates a random pattern of red pixels of varying brightness. This may seem a bit pointless, but it does allow us to make sure that the code in actually running. In later weeks we are going to do more sophisticated (and more useful) things with the pixels !

You will note from the `handleEvent` method that it is possible to respond to both mouse and keyboard events. In the `RedNoise` example, key presses simply print out messages to the terminal (by directing strings to the `cout` object). In later weeks we are going to use these event handlers to allow the user to navigate around a scene and switch between different rendering modes. For the time being however, just printing out messages is fine.

Clicking the mouse inside the SDL window will cause a screenshot of the current content of the window to be saved to the filesystem. You will notice that both PPM and BMP files are generated. The reason for this is that some platforms and image viewer can be fussy about the format of image files. Pick whichever file format works best on your platform and work with that.  


#
### End of workbook

Make sure you push all your work to your GitHub repository !<br>
Use of GitHub is a key development skill.<br>
It will allow us to keep track of your progress (and we will use it in the final marking process).
