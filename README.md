# clox
A bytecode virtual machine for the Lox language

Based off the book, Crafting Interpreters by Bob Nystrom, clox is a bytecode virtual machine for the Lox language.

### About Lox (the language)
Lox is a programming language used in the book for the main purpose of introducing compilers and interpreters to novices.

Lox is a fully-fledged programming language in the OOP vein of languages. Lox is dynamically typed (think JavaScript and Python), not statically typed (think Java).
Being an OOP language, Lox supports classes, which can contain state and behaviour in the form of fields and methods respectively.
Lox classes can also support constructors/initializers. Besides this, Lox supports all other typical language constructs, including logical operators, if-else blocks,
while and for loops, and first-class functions.

Without dumping the entire documentation of Lox here, the complete syntax and lexical grammar of the Lox language written in Backus-Naur form (BNF)
can be found [here](https://craftinginterpreters.com/appendix-i.html).

### What's this?
clox is the sequel to jlox, a tree-walk interpreter for the Lox language written in Java. You can take a look at jlox [here](https://github.com/Silvernitro/jlox).

Following the book's progression, jlox is but one half of the Lox journey. jlox is **painfully** slow. As such, the next step is *clox*, a bytecode compiler written 
in C. I'll also go along with the book and write a VM to execute the bytecode on.

