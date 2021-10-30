#pragma once
#include "Graphics.h"

typedef std::vector<bool>** PossibilityTable;
typedef std::vector<bool>* PossibilityArray;

struct SolveData {
	int size; // Sudoku size
	int boxWidth; // Sudoku Box Width
	int boxHeight; // Sudoku Box Height
	PossibilityTable t; // table
	PossibilityTable b; // boxes
	PossibilityArray r; // rows
	PossibilityArray c; // columns

	SolveData() {
		size = 0;
		boxWidth = 0;
		boxHeight = 0;
		t = NULL;
		b = NULL;
		r = NULL;
		c = NULL;
	}

	SolveData& copy(const SolveData& other) {

		size = other.size;
		boxWidth = other.boxWidth;
		boxHeight = other.boxHeight;

		t = new PossibilityArray[size];
		for (int x = 0; x < size; x++) {
			t[x] = new std::vector<bool>[size];
			for (int y = 0; y < size; y++) {
				t[x][y] = other.t[x][y];
			}
		}

		// Create tables for boxes, rows and colums to keep track of how many cells remain unknown.
		// Boxes
		b = new PossibilityArray[boxHeight];

		// Number of boxes in x axis = height of boxes and number of boxes in y axis = width of boxes
		for (int x = 0; x < boxHeight; x++) {
			b[x] = new std::vector<bool>[boxWidth];
			for (int y = 0; y < boxWidth; y++) {
				b[x][y] = other.b[x][y];
			}
		}

		r = new std::vector<bool>[size];
		c = new std::vector<bool>[size];

		for (int x = 0; x < size; x++) {
			r[x] = other.r[x];
			c[x] = other.c[x];
		}

		return *this;
	}

	SolveData& operator=(SolveData &other) {
		copy(other);
		return *this;
	}

	SolveData(SolveData& other) {
		copy(other);
	}

	~SolveData(){
		if (t != nullptr) {
			for (int x = 0; x < size; x++) {
				delete[] t[x];
			}
			delete[] t;
			t = nullptr;
		}
		if (b != nullptr) {
			for (int x = 0; x < boxHeight; x++) {
				delete[] b[x];
			}
			delete[] b;
			b = nullptr;
		}
		if (r != nullptr) {
			delete[] r;
			r = nullptr;
		}
		if (c != nullptr) {
			delete[] c;
			c = nullptr;
		}
	}
};

int Solve(Sudoku& s);
int recursiveSolve(Sudoku& s, SolveData& sd);

bool check(Sudoku& s, SolveData& sd, int& x, int& y);
void resolve(Sudoku& s, SolveData& sd, int& x, int& y, int& value);