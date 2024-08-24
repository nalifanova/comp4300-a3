# Assignment 03 for COMP 4300 Course

--------------------------------------
<div align="center">

[![RPG](https://img.shields.io/badge/Assignment-A3-yellow.svg?logo=data:image/svg%2bxml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZlcnNpb249IjEiIHdpZHRoPSI2MDAiIGhlaWdodD0iNjAwIj48cGF0aCBkPSJNMTI5IDExMWMtNTUgNC05MyA2Ni05MyA3OEwwIDM5OGMtMiA3MCAzNiA5MiA2OSA5MWgxYzc5IDAgODctNTcgMTMwLTEyOGgyMDFjNDMgNzEgNTAgMTI4IDEyOSAxMjhoMWMzMyAxIDcxLTIxIDY5LTkxbC0zNi0yMDljMC0xMi00MC03OC05OC03OGgtMTBjLTYzIDAtOTIgMzUtOTIgNDJIMjM2YzAtNy0yOS00Mi05Mi00MmgtMTV6IiBmaWxsPSIjZmZmIi8+PC9zdmc+)]()
[![C++ - Version](https://img.shields.io/badge/C++-20-blue.svg?style=flat&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/compiler_support/20)
![GitHub last commit](https://img.shields.io/github/last-commit/nalifanova/comp4300-a3?display_timestamp=author&style=flat&logo=github)

</div>

## About the course
[Video Lectures on YouTube](https://www.youtube.com/playlist?list=PL_xRyXins848nDj2v-TJYahzvs-XW9sVV)
COMP 4300 - C++ Game Programming (2022-09) by Dave Churchill

## About the assignment

I should google all over to find some assets and (thanks GitHub!) there were some repositories
with textures. I'm not sure if it fits the current Assignment but I work with what I have.

For more information about assignment itself, please read the
[ASSIGNMENT.md](ASSIGNMENT03.md) file.


## Dev Notes
I'm using CLion as my IDE, and I try to follow a guideline from
[CLion:](https://www.jetbrains.com/help/clion/clangformat-as-alternative-formatter.html)
along with the style the lecturer provided in his examples.

If you like to take deeper into what and where, I recommend using this source
[lefticus](https://lefticus.gitbooks.io/cpp-best-practices/content/03-Style.html)

Nonetheless, I consult authoritative pillars such as the
([C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines.html))
and [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
to ensure that the code remains readable and consistent.

### Guide style
* Each row of text in your code should be at most 120 characters long;
* Use a .cpp suffix for code files and .hpp for interface files;
  [NL.27](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines.html#Rl-file-suffix)
* [Names and order of includes](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
  Related header, C system headers, C++ standard library headers, other headers;
* In a class I prefer to see public methods, private methods, public vars, private vars. Exactly
  in this order.
