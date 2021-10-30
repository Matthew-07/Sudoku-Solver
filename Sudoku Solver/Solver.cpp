#include "Solver.h"

int Solve(Sudoku& s) {

	/*
	Algorithm

	The algorithm keep a table of all the values it thinks each cell could be. Each cell will start off able to be any value apart from givens.
	It will then  make use of three methods to reduce the number of different digits it believes cells can be.

	(1) - Take a known digit and eliminate that possibilty from all cells in the same box/row/column.

	(2) - Look at the digits that must still be placed in a box/row/column and work out where they can be placed.
	[a] If there remains just once cell that can contain the digit then the cell must contain that digit. Hence fill the digit and apply method (1)
	[b] If it is determined that a particular n digits must appear within an n sized region, that region must contain those digits and no others. Hence eliminate
	the other possibilities from the region.
	[c] If it is determined that a set of cells are restricted to a region in the box/row/column and that region also exists entirely within another box/row/column
	than those cells can exist outwidth the region in that other box/row/column hence eliminate those possibilities from the relevant cells.

	e.g. if it is determined that the top left cell of a box must be a two or a five and the middle left a two, five or seven but two and five may not appear 
	anywhere else in the box. The top left and middle left cells must contain the two and five so the middle left can't be a seven [b]. Similarly by applying rule
	one since the two and five also exist within the same column, neither two nor five can be placed anywhere else in the column so those possibilites can be eliminated [c].

	(3) - If there are any n cells in a box/row/column that each have a subset of n different possible digits i.e. two cells that each can contain a four or a six
	then those cells must contain those values and so those possibilities can be eliminated from all cells in the relevant box/row/column.


	Algorithm

	1 - Apply (1) to eliminate possibilties in the box/row/column of existing cells.
	1.1 Tiles solved whilst applying (1) to givens should also have (1) applied.

	2 - Apply (2) to eliminate further possibilites once (1) is unable to solve more tiles.
	2.1 Start with boxes/rows/columns that have the most known cells. i.e. attempting to apply rule (2a,b and c)
	2.2 When additional tiles are solved apply (1).

	3 - When neither (1) or (2) are sufficient to solve addition cells, apply (3)
	3.1 Start by checking cells in rows/columns that are also in the same box.
	3.2 Whenever additional tiles are solved apply (1) and move back to step 2.

	*/





	// I might want to copy the sudoku grid and only make changes to the original at certain time intervals and once the puzzle is solved.




	// Prepare possibility table
	PossibilityTable possibleDigits = new PossibilityArray[s.size];
	for (int x = 0; x < s.size; x++) {
		possibleDigits[x] = new std::vector<bool>[s.size];
		for (int y = 0; y < s.size; y++) {
			possibleDigits[x][y] = std::vector<bool>();
			// First bit -> whether cell has been "solved". All others represent whether the algorithm considers a digit as possible
			possibleDigits[x][y].push_back(false);
			for (int d = 0; d < s.size; d++) {
				possibleDigits[x][y].push_back(true);
			}
		}
	}

	// Create tables for boxes, rows and colums to keep track of how many cells remain unknown.
	// Boxes
	PossibilityTable boxTable = new PossibilityArray[s.boxHeight];

	// Number of boxes in x axis = height of boxes and number of boxes in y axis = width of boxes
	for (int x = 0; x < s.boxHeight; x++) {
		boxTable[x] = new std::vector<bool>[s.boxWidth];
		for (int y = 0; y < s.boxWidth; y++) {
			boxTable[x][y] = std::vector<bool>();
			// First bit -> whether box has been "solved". All others represent whether each digit has been placed in the box
			boxTable[x][y].push_back(false);
			for (int d = 0; d < s.size; d++) {
				boxTable[x][y].push_back(false);
			}
		}
	}

	PossibilityArray rowsTable = new std::vector<bool>[s.size];
	PossibilityArray columnsTable = new std::vector<bool>[s.size];

	for (int x = 0; x < s.size; x++) {
		rowsTable[x] = std::vector<bool>();
		columnsTable[x] = std::vector<bool>();

		// First bit -> whether row/column has been "solved". All others represent whether each digit has been placed in the row/column
		rowsTable[x].push_back(false);
		columnsTable[x].push_back(false);
		for (int d = 0; d < s.size; d++) {
			rowsTable[x].push_back(false);
			columnsTable[x].push_back(false);
		}
	}

	SolveData data;
	data.t = possibleDigits;
	data.b = boxTable;
	data.r = rowsTable;
	data.c = columnsTable;
	data.size = s.size;
	data.boxWidth = s.boxWidth;
	data.boxHeight = s.boxHeight;


	// Apply rule 1 to givens.
	for (int x = 0; x < s.size; x++) {
		for (int y = 0; y < s.size; y++) {
			if (s.grid[x][y] > 0) {
				resolve(s, data, x, y, s.grid[x][y]);
			}
		}
	}	

	//std::ofstream f("possibilityTable.txt");

	//for (int x = 0; x < s.size; x++) {
	//	for (int y = 0; y < s.size; y++) {
	//		f << x << " " << y << " : ";
	//		for (int d = 1; d < s.size+1; d++) {
	//			if (possibleDigits[x][y][d] == true) {
	//				f << d << " ";
	//			}
	//		}
	//		f << std::endl;
	//	}
	//}

	//f.close();
	return recursiveSolve(s,data);
}

int recursiveSolve(Sudoku& s, SolveData& sd) {

	/*std::ofstream f("possibilitytable.txt");

	for (int x = 0; x < s.size; x++) {
		for (int y = 0; y < s.size; y++) {
			f << x << " " << y << " : ";
			for (int d = 1; d < s.size+1; d++) {
				if (sd.t[x][y][d] == true) {
					f << d << " ";
				}
			}
			f << std::endl;
		}
	}

	f << std::endl << "Rows" << std::endl;
	for (int y = 0; y < s.size; y++) {
		if (sd.r[y][0]) {
			f << "True" << std::endl;
		}
		else {
			f << "False" << std::endl;
		}
	}

	f.close();*/

	bool progressMade;

	do {
		progressMade = false;



		// Rule 2


		// Apply to Boxes
		for (int a = 0; a < s.boxHeight; a++) {
			for (int b = 0; b < s.boxWidth; b++) {
				if (sd.b[a][b][0] == false) {
					for (int d = 1; d < s.size + 1; d++) {
						if (sd.b[a][b][d] == false) {
							std::vector<xy> possibleCells;
							for (int x = a * s.boxWidth; x < (a + 1) * s.boxWidth; x++) {
								for (int y = b * s.boxHeight; y < (b + 1) * s.boxHeight; y++) {
									if (!sd.t[x][y][0] && sd.t[x][y][d]) {
										possibleCells.push_back(xy(x, y));
									}
								}
							}
							if (possibleCells.size() == 1) {								
								resolve(s, sd, possibleCells[0].x, possibleCells[0].y, d);
								progressMade = true;
							}
							else {

								if (possibleCells.size() == 0) {
									// Something was invalid
									return 0;
								}

								bool sharedColumn = true;
								int columnNumber = possibleCells[0].x;
								for (int cellNum = 1; cellNum < possibleCells.size(); cellNum++) {
									if (columnNumber != possibleCells[cellNum].x) {
										sharedColumn = false;
										break;
									}
								}
								if (sharedColumn) {
									// possibleCells will be from left to right and top to bottom
									for (int i = 0; i < s.size; i++) {
										if (i < possibleCells[0].y || i > possibleCells.back().y) {
											sd.t[columnNumber][i][d] = false;
											if (check(s, sd, columnNumber, i)) progressMade = true;
										}
									}
								}
								else {

									bool sharedRow = true;
									int rowNumber = possibleCells[0].y;
									for (int cellNum = 1; cellNum < possibleCells.size(); cellNum++) {
										if (rowNumber != possibleCells[cellNum].y) {
											sharedRow = false;
											break;
										}
									}
									if (sharedRow) {
										// possibleCells will be from left to right and top to bottom
										for (int i = 0; i < s.size; i++) {
											if (i < possibleCells[0].x || i > possibleCells.back().x) {
												sd.t[i][rowNumber][d] = false;
												if (check(s, sd, i, rowNumber)) progressMade = true;
											}
										}
									}
								}

								//Rule 2b - boxes
								for (int d2 = 1; d2 < s.size + 1; d2++) {
									if (sd.b[a][b][d2] == true) continue;
									std::vector<int> sharedDigits = { d };
									if (d != d2) {
										bool sharedRegion = true;
										std::vector<xy>::iterator it;
										it = possibleCells.begin();
										for (int bx = a * s.boxWidth; bx < (a + 1) * s.boxWidth; bx++) {
											for (int by = b * s.boxHeight; by < (b + 1) * s.boxHeight; by++) {
												if (sd.t[bx][by][0] == false && sd.t[bx][by][d2] == true) {
													bool cellInRegion = false;
													while (it != possibleCells.end()) {
														if (it->x == bx && it->y == by) {
															cellInRegion = true;
															break; // Cell appeared, move on
														}
														else {
															++it;
														}
													}
													if (!cellInRegion) {
														sharedRegion = false;
														break;
													}
												}
											}
										}
										if (sharedRegion) {
											sharedDigits.push_back(d2);
											if (sharedDigits.size() == possibleCells.size()) {
												// n sized region of n different digits has been found. No other digits may exist in this region
												std::vector<int>::iterator it;
												it = sharedDigits.begin();
												for (int digit = 1; digit < s.size + 1; digit++) {
													bool digitAllowed = false;
													for (int& allowedDigit : sharedDigits) {
														if (allowedDigit == digit) {
															digitAllowed = true;
															break;
														}
													}
													if (!digitAllowed) {
														for (xy& cell : possibleCells) {
															sd.t[cell.x][cell.y][digit] = false;
														}
													}
												}
												for (xy& cell : possibleCells) {
													if (check(s, sd, cell.x, cell.y)) progressMade = true;
												}
												break;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		// Apply to Rows + Columns
		for (int i = 0; i < s.size; i++) {
			// Rows
			if (sd.r[i][0] == false) {
				for (int d = 1; d < s.size + 1; d++) {
					if (sd.r[i][d] == false) {
						std::vector<int> possibleCells;
						for (int x = 0; x < s.size; x++) {
							if (!sd.t[x][i][0] && sd.t[x][i][d]) {
								possibleCells.push_back(x);
							}
						}
						if (possibleCells.size() == 1) {
							progressMade = true;
							resolve(s, sd, possibleCells[0], i, d);
						}
						else {

							if (possibleCells.size() == 0) {
								// Something was invalid
								return 0;
							}

							int boxX = possibleCells[0] / s.boxWidth;
							if (possibleCells.back() / s.boxWidth == boxX) {
								int boxY = i / s.boxHeight;
								// possibleCells will be from left to right
								for (int x = boxX * s.boxWidth; x < (boxX + 1) * s.boxWidth; x++) {
									for (int y = boxY * s.boxHeight; y < (boxY + 1) * s.boxHeight; y++) {
										if (x < possibleCells[0] || x > possibleCells.back() || y != i) {
											sd.t[x][y][d] = false;
											if (check(s, sd, x, y)) progressMade = true;
										}
									}
								}
							}
							// Rule 2b - rows, I think it might be possible to ommit the rule 2b code for rows and columns and only apply it to boxes.
							for (int d2 = 1; d2 < s.size + 1; d2++) {
								if (sd.r[i][d2] == true) continue;
								std::vector<int> sharedDigits = { d };
								if (d != d2) {
									bool sharedRegion = true;
									std::vector<int>::iterator it;
									it = possibleCells.begin();
									for (int bx = 0; bx < s.size; bx++) {
										if (sd.t[bx][i][0] == false && sd.t[bx][i][d2] == true) {
											bool cellInRegion = false;
											while (it != possibleCells.end()) {
												if (*it == bx) {
													cellInRegion = true;
													break; // Cell appeared, move on
												}
												else {
													++it;
												}
											}
											if (!cellInRegion) {
												sharedRegion = false;
												break;
											}
										}
									}
									if (sharedRegion) {
										sharedDigits.push_back(d2);
										if (sharedDigits.size() == possibleCells.size()) {
											// n sized region of n different digits has been found. No other digits may exist in this region
											std::vector<int>::iterator it;
											it = sharedDigits.begin();
											for (int digit = 1; digit < s.size + 1; digit++) {
												bool digitAllowed = false;
												for (int& allowedDigit : sharedDigits) {
													if (allowedDigit == digit) {
														digitAllowed = true;
														break;
													}
												}
												if (!digitAllowed) {
													for (int& cell : possibleCells) {
														sd.t[cell][i][digit] = false;
													}
												}
											}
											for (int& cell : possibleCells) {
												if (check(s, sd, cell, i)) progressMade = true;
											}
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			// Columns
			if (sd.c[i][0] == false) {
				for (int d = 1; d < s.size + 1; d++) {
					if (sd.c[i][d] == false) {
						std::vector<int> possibleCells;
						for (int y = 0; y < s.size; y++) {
							if (!sd.t[i][y][0] && sd.t[i][y][d]) {
								possibleCells.push_back(y);
							}
						}
						if (possibleCells.size() == 1) {
							progressMade = true;
							resolve(s, sd, i, possibleCells[0], d);
						}
						else {

							if (possibleCells.size() == 0) {
								// Something was invalid
								return 0;
							}

							int boxY = possibleCells[0] / s.boxHeight;
							if (possibleCells.back() / s.boxHeight == boxY) {
								int boxX = i / s.boxWidth;
								// possibleCells will be from top to bottom
								for (int x = boxX * s.boxWidth; x < (boxX + 1) * s.boxWidth; x++) {
									for (int y = boxY * s.boxHeight; y < (boxY + 1) * s.boxHeight; y++) {
										if (y < possibleCells[0] || y > possibleCells.back() || x != i) {
											sd.t[x][y][d] = false;
											if (check(s, sd, x, y)) progressMade = true;
										}
									}
								}

							}

							// Rule 2b - columns, I think it might be possible to ommit the rule 2b code for rows and columns and only apply it to boxes.
							for (int d2 = 1; d2 < s.size + 1; d2++) {
								if (sd.c[i][d2] == true) continue;
								std::vector<int> sharedDigits = { d };
								if (d != d2) {
									bool sharedRegion = true;
									std::vector<int>::iterator it;
									it = possibleCells.begin();
									for (int by = 0; by < s.size; by++) {
										if (sd.t[i][by][0] == false && sd.t[i][by][d2] == true) {
											bool cellInRegion = false;
											while (it != possibleCells.end()) {
												if (*it == by) {
													cellInRegion = true;
													break; // Cell appeared, move on
												}
												else {
													++it;
												}
											}
											if (!cellInRegion) {
												sharedRegion = false;
												break;
											}
										}
									}
									if (sharedRegion) {
										sharedDigits.push_back(d2);
										if (sharedDigits.size() == possibleCells.size()) {
											// n sized region of n different digits has been found. No other digits may exist in this region
											std::vector<int>::iterator it;
											it = sharedDigits.begin();
											for (int digit = 1; digit < s.size + 1; digit++) {
												bool digitAllowed = false;
												for (int& allowedDigit : sharedDigits) {
													if (allowedDigit == digit) {
														digitAllowed = true;
														break;
													}
												}
												if (!digitAllowed) {
													for (int& cell : possibleCells) {
														sd.t[i][cell][digit] = false;
													}
												}
											}
											for (int& cell : possibleCells) {
												if (check(s, sd, i, cell)) progressMade = true;
											}
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	while (progressMade);
	
	// Check if sudoku solved;
	bool solved = true;
	for (int i = 0; i < s.size; i++) {
		if (!sd.r[i][0]) {
			solved = false;
			break;
		}
	}
	if (solved) return 1;
	else {
		// When all else fails, try filling in a cell with a digit and see if it can then be solved

		// First check that all the clues in the sudoku meet the rules, if not a previous "guess" must have been wrong.
		std::vector<bool> digitsFound;
		for (int d = 0; d < s.size; d++) {
			digitsFound.push_back(false);
		}
		// Check rows and columns
		for (int i = 0; i < s.size; i++) {
			// Rows
			for (int x = 0; x < s.size; x++) {

				for (int d = 0; d < s.size; d++) {
					digitsFound[d] = false;
				}

				if (sd.t[x][i][0]) {
					if (!digitsFound[(long long) s.grid[x][i]-1]) {
						digitsFound[(long long) s.grid[x][i]-1] = true;
					}
					else {
						return 0;
					}
				}
			}

			// Columns
			for (int y = 0; y < s.size; y++) {
				for (int d = 0; d < s.size; d++) {
					digitsFound[d] = false;
				}

				if (sd.t[i][y][0]) {
					if (!digitsFound[(long long)s.grid[i][y]-1]) {
						digitsFound[(long long)s.grid[i][y]-1] = true;
					}
					else {
						return 0;
					}
				}
			}
		}

		// Check Boxes;
		for (int a = 0; a < s.boxHeight; a++) {
			for (int b = 0; b < s.boxWidth; b++) {
				for (int d = 0; d < s.size; d++) {
					digitsFound[d] = false;
				}

				for (int x = a * s.boxWidth; x < (a + 1) * s.boxWidth; x++) {
					for (int y = b * s.boxHeight; y < (b + 1) * s.boxHeight; y++) {
						if (sd.t[x][y][0]) {
							if (!digitsFound[(long long) s.grid[x][y]-1]) {
								digitsFound[(long long) s.grid[x][y]-1] = true;
							}
							else {
								return 0;
							}
						}
					}
				}
			}
		}
		
		// Copy sudoku and solve data
		Sudoku s2 = s;
		SolveData sd2 = sd;

		// Pick the first cell with the lowest number of possibilites and start from there.
		for (int numberOfPossibilities = 2; numberOfPossibilities < s.size; numberOfPossibilities++) {
			for (int x = 0; x < s.size; x++) {
				for (int y = 0; y < s.size; y++) {
					if (!sd.t[x][y][0]) {
						int numPosDigits = 0;
						for (int d = 1; d < s.size + 1; d++) {
							if (sd.t[x][y][d]) numPosDigits++;
						}
						if (numPosDigits == numberOfPossibilities) {
							int numberOfSolutions = 0;
							Sudoku solution = s;
							for (int d = 1; d < s.size + 1; d++) {
								if (sd.t[x][y][d]){									
									resolve(s2, sd2, x, y, d);
									int solutions = recursiveSolve(s2, sd2);
									numberOfSolutions += solutions;
									if (solutions == 1) {
										solution = s2;
									}
									s2 = s;
									sd2 = sd;

									if (numberOfSolutions > 1) {
										return 2;
									}
								}
							}
							if (numberOfSolutions == 1) {
								s = solution;
							}
							return numberOfSolutions;
						}
					}
				}
			}
		}
	}
	return 0;
}




bool check(Sudoku& s, SolveData& sd, int& x, int& y) {
	// Check if the cell has already been resolved
	if (sd.t[x][y][0]) return 0;

	// Check if only one possibility is true so the cell can be resolved.
	int value = -1;
	bool found = false;
	bool update = false;
	for (int p = 1; p-1 < s.size; p++) {
		if (sd.t[x][y][p]) {
			if (!found) {
				value = p;
				update = true;
				found = true;
			}
			else {
				update = false;
			}
		}
	}
	if (update) {
		resolve(s, sd, x, y, value);
		return 1;
	}
	return 0;
}


void resolve(Sudoku& s, SolveData& sd, int& x, int& y, int& value) {
	// Set the cell to the correct value and apply rule one
	s.grid[x][y] = value;
	sd.t[x][y][0] = true;

	// Update box, row and column
	bool boxSolved = true, rowSolved = true, columnSolved = true;
	sd.b[x / s.boxWidth][y / s.boxHeight][value] = true;
	sd.c[x][value] = true;
	sd.r[y][value] = true;

	for (int i = 1; i < s.size+1; i++) {
		if (!sd.r[y][i]) rowSolved = false;
		if (!sd.c[x][i]) columnSolved = false;
		if (!sd.b[x / s.boxWidth][y / s.boxHeight][i]) boxSolved = false;
	}
	if (rowSolved) sd.r[y][0] = true;
	if (columnSolved) sd.c[x][0] = true;
	if (boxSolved) sd.b[x / s.boxWidth][y / s.boxHeight][0] = true;

	// Check row
	for (int a = 0; a < s.size; a++) {
		if (a != x) {
			sd.t[a][y][value] = false;
			check(s, sd, a, y);
		}
	}

	// Check column
	for (int b = 0; b < s.size; b++) {
		if (b != y) {
			sd.t[x][b][value] = false;
			check(s, sd, x, b);
		}
	}

	// Check box
	int startX = floor(x / s.boxWidth) * s.boxWidth;
	int startY = floor(y / s.boxHeight) * s.boxHeight;

	for (int a = startX; a < startX + s.boxWidth; a++) {
		for (int b = startY; b < startY + s.boxHeight; b++) {
			if (x != a || y != b) {
				sd.t[a][b][value] = false;
				check(s, sd, a, b);
			}
		}
	}
}