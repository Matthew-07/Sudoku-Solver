# Sudoku-Solver
A simple recursive sudoku solver with a GUI to enter Sudokus.

This is quite a simple program. It uses a few simple methods to try and solve cells in the sudoku, if it has not successfully solved any then it makes a guess and runs recursively on a copy of the sudoku with that guess. If it at some point discovers it has made a mistake, it will go back and make a different guess.

The main flaw with the code is not letting the user choose the size of the sudoku at runtime, instead the sizes are controlled by macros.
