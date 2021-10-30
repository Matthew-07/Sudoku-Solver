#pragma once

#include "framework.h"

#include "CompileTimeSettings.h"

struct Sudoku {
private:
	Sudoku& copy(const Sudoku& other) {
		size = other.size;
		boxWidth = other.boxWidth;
		boxHeight = other.boxHeight;

		grid = new int* [size];
		for (int x = 0; x < size; x++) {
			grid[x] = new int[size];
			for (int y = 0; y < size; y++) {
				grid[x][y] = other.grid[x][y];
			}
		}

		return *this;
	}

public:

	Sudoku(int bW, int bH){
		size = bW * bH;
		boxWidth = bW;
		boxHeight = bH;
		grid = new int* [size];
		for (int x = 0; x < size; x++) {
			grid[x] = new int[size];
			for (int y = 0; y < size; y++) {
				grid[x][y] = -1; // -1 = blank empty cell, -2 = cell coloured black
				//grid[x][y] = rand() % SUDOKU_SIZE;
			}
		}
	}

	Sudoku& operator=(const Sudoku& other) {
		copy(other);
		return *this;
	}

	Sudoku(const Sudoku& other) {
		copy(other);
	}

	~Sudoku() {
		if (grid != nullptr) {
			for (int x = 0; x < size; x++) {
				delete[] grid[x];
			}
			delete[] grid;
			grid = nullptr;
		}
	}

	int size, boxWidth, boxHeight;
	int** grid;
};

struct xy {
	int x, y;
	xy(int a, int b) {
		x = a;
		y = b;
	}

	xy() {
		x = -1;
		y = -1;
	}
};

class Graphics {
public:

	void init(HINSTANCE hInstance);
	void onPaint();
	void onResize();

	void createResources();
	void destroyResources();

	Sudoku* grid;

	xy currentCell;
	RECT winRect;

	HWND m_hWnd = NULL;

private:
	void drawSudoku(const Sudoku& s) const;

	HINSTANCE m_hInstance;

	ID2D1Factory*			pFactory;
	IDWriteFactory*			pWriteFactory;
	
	ID2D1HwndRenderTarget*	pRenderTarget;

	ID2D1SolidColorBrush*	pBlack;
	ID2D1SolidColorBrush*	pGrey;
	ID2D1SolidColorBrush*	pBlue;
	ID2D1SolidColorBrush*	pBackground;
	ID2D1SolidColorBrush*	pHighlight;

	IDWriteTextFormat*		pText;
};