# Laboratory work №1. <br/> Introduction to parallel computing. <br /> OpenMP technology.

## Description

The lab contains a program written in C that searches for the maximum element in an array using OpenMP (A detailed description of the task is contained in the file [lab1.pdf](https://github.com/GektorPestarzt/MEPHI_parallel/blob/main/lab1/lab1.pdf)).
There are two versions of program implemented in one file using macros: **linear** and **parallel**.
To build and run these two versions use script **run** that call Makefile.
Upon completion, two files will appear in the folder **log_files**. In each will be contained the execution time
of the program which were calculated with different numbers of threads.

## Results

Results presented in file [report.pdf](https://github.com/GektorPestarzt/MEPHI_parallel/blob/main/lab1/report.pdf). These were obtained on a device with 4 cores, searching in array of 10.000.000 element and with 100 attempts. More details in the file.

