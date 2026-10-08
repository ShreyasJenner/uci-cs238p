# Description
Repo that contains all the projects worked on in the cs238p UCI course

## P1
Design a program that accepts a mathematical expression (a string containing the symbols: +, -, *, /, (, ), value). This program will dynamically generate a C program based on the input string, invoke a C compiler to create an equivalent loadable module, and then load and execute the machine code equivalent of the C program to produce the evaluated result of the expression. You may use the following to bootstrap the project:

    Makefile
    system.h, system.c
    lexer.h, lexer.c
    parser.h, parser.c
    jitc.h, jitc.c
    main.c

This project earns you 15% of your class credit. You can earn 2% extra-credit if you implement the sigmoid(double x) function that resides in your main program but is called from the generated C code to transform the final value of the expression (i.e., sigmoid(expression-value)).