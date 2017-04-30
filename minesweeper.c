#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conio.h"
#include <stdbool.h>
#define MAX 10

	/// background color
#define BRED  "\x1B[41m"
#define BGRN  "\x1B[42m"
#define BYEL  "\x1B[43m"
#define BBLU  "\x1B[44m"
#define BMAG  "\x1B[45m"
#define BCYN  "\x1B[46m"
#define BWHT  "\x1B[47m"

	/// text color
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

	/// bit operation
#define BIT(x) (0x01 << (x))
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLEP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) (((x) & (y)) == (y))

	///seting bit
const unsigned int FLAG_MASK = BIT(4);
const unsigned int UNCOVERED_MASK = BIT(5);
const unsigned int MINE_MASK = BIT(6);
const unsigned int NUM8 = BIT(3);
const unsigned int NUM4 = BIT(2);
const unsigned int NUM2 = BIT(1);
const unsigned int NUM1 = BIT(0);

	///bit operation function
inline _bool has_mine(unsigned int cell){	/// for mine
	return BITMASK_CHECK(cell, MINE_MASK);
}
inline void put_mine(unsigned int cell){
	BITMASK_SET(cell, MINE_MASK);
}
inline _bool check_uncovered(unsigned int cell){	/// for uncover
	return BITMASK_CHECK(cell, UNCOVERED_MASK);
}
inline void set_uncovered(unsigned int cell){	
	BITMASK_SET(cell, UNCOVERED_MASK);
}
inline void set_covered(unsigned int cell){
	BITMASK_CLEAR(cell, UNCOVERED_MASK);
}
inline _bool check_flaged(unsigned int cell){	///for flag 
	return BITMASK_CHECK(cell, FLAG_MASK);
}
inline void set_flaged(unsigned int cell){
	BITMASK_SET(cell, FLAG_MASK);
}
inline void set_unflaged(unsigned int cell){
	BITMASK_CLEAR(cell,FLAG_MASK);
}
inline unsigned int check_mine_surrounded(unsigned int cell){	///for counting mines
	unsigned int total = 0;
	if(BITMASK_CHECK(cell, NUM8)){
		total += 8;
	}
	if(BITMASK_CHECK(cell, NUM4)){
		total += 4;
	}
	if(BITMASK_CHECK(cell, NUM2)){
		total += 2;
	}
	if(BITMASK_CHECK(cell, NUM1)){
		total += 1;
	}
	return total;
}
	/// global variables
	/// game table
unsigned char table_array[MAX][MAX];
	/// location of cursor
int x = 0, y = 0;
	/// flag: input mode = 0, flag mode = 1, check mode = 2
int game_mode = 0;

/**
* This is a recursive function which uncovers blank cells while they are adjacent
*/
int uncover_blank_cell(int row, int col) {
	int value,surrounded_mines_num, rows[8], columns[8], i;
	
	if (table_array[row][col] != 0)
		return 0; // error

	set_uncovered(table_array[row][col]); // uncover current cell

	// Get position of adjacent cells of current cell
	rows[0] = row - 1;
	columns[0] = col + 1;
	rows[1] = row;
	columns[1] = col + 1;
	rows[2] = row + 1;
	columns[2] = col + 1;
	rows[3] = row - 1;
	columns[3] = col;
	rows[4] = row + 1;
	columns[4] = col;
	rows[5] = row - 1;
	columns[5] = col - 1;
	rows[6] = row;
	columns[6] = col - 1;
	rows[7] = row + 1;
	columns[7] = col - 1;

	for (i = 0; i < 8; i++) {
		value = table_array[rows[i]][columns[i]];
		surrounded_mines_num = check_mine_surrounded(table_array[rows[i]][columns[i]]);
		bool isValid = (rows[i] >= 0 && rows[i] < MAX) && (columns[i] >= 0 && columns[i] < MAX);
		if (isValid) {	// to prevent negative index and out of bounds
			if ( !check_uncovered(value) && surrounded_mines_num > 0 && surrounded_mines_num <=8 )
				set_uncovered(table_array[rows[i]][columns[i]]);										// it is a cell with 1-8 number so we need to uncover
			else if (surrounded_mines_num == 0 && !check_uncovered(value))
				uncover_blank_cell(rows[i], columns[i]);
		}

	}

	return 1; // success!
}

void print_table() {
	// clear screen
	system("clear");

	int i, j, value, surrounded_mines_num;
	for (i = 0; i < MAX; i++) {
		for (j = 0; j < MAX; j++) {
			if (x == j && y == i) {
				if (game_mode == 1) {
					printf("|%sF%s", BMAG, KNRM);
					continue;
				}
				else if (game_mode == 2) {
					printf("|%sC%s", BMAG, KNRM);
					continue;
				}
			}
			value = table_array[i][j];
			surrounded_mines_num = check_mine_surrounded(table_array[i][j]);
			if ( !(check_uncovered(value)) )
				printf("|X");
			else if ( check_uncovered(value) && surrounded_mines_num == 0 ) // clean area
				printf("|%s%d%s", KCYN, surrounded_mines_num , KNRM);
			else if ( check_uncovered(value) && surrounded_mines_num == 1) // the number of near mine is 1
				printf("|%s%d%s", KYEL, surrounded_mines_num , KNRM);
			else if ( check_uncovered(value) && surrounded_mines_num > 1 && surrounded_mines_num <= 8) // the number of near mine is greater than 1
				printf("|%s%d%s", KRED, surrounded_mines_num , KNRM);
			else if ( check_flaged(value) && !check_uncovered(value) )
				printf("|%sF%s", KGRN, KNRM);
			else
				printf("ERROR"); // test purposes

		}
		printf("|\n");
	}

	printf("cell values: 'X' unknown, '%s0%s' no mines close, '1-8' number of near mines, '%sF%s' flag in cell\n", KCYN, KNRM, KGRN, KNRM);
	if (game_mode == 0) {
		printf("f (put/remove Flag in cell), c (Check cell), n (New game), q (Exit game): ");
	}
	else if (game_mode == 1) {
		printf("Enter (select to put/remove Flag in cell), q (Exit selection): ");
	}
	else if (game_mode == 2) {
		printf("Enter (select to check cell), q (Exit selection): ");
	}
}


int main(int argc, char *argv[]) {

	char ch;
	int number_Mines; // the number of the remaining mines
	int i, j, r, c, value, surrounded_mines_num, rows[8], columns[8];
	

	while (1) {
		// the number of mines
		number_Mines = 10;
		if (argc == 2) {
			number_Mines = atoi(argv[1]);
		}
		srand(time(NULL));						// random seed
												// setting cursor
		x = 0;
		y = 0;
		// set all cells to 0
		for (i = 0; i < 10; i++)
			for (j = 0; j < 10; j++)
				table_array[i][j] = 0;

		for (i = 0; i < number_Mines; i++) {
			/* initialize random seed: */

			r = rand() % 10;					// it generates a integer in the range 0 to 9
			c = rand() % 10;

			// put mines
			if (!has_mine(table_array[r][c])) {
				put_mine(table_array[r][c]);

				// Get position of adjacent cells of current cell
				rows[0] = r - 1;
				columns[0] = c + 1;
				rows[1] = r;
				columns[1] = c + 1;
				rows[2] = r + 1;
				columns[2] = c + 1;
				rows[3] = r - 1;
				columns[3] = c;
				rows[4] = r + 1;
				columns[4] = c;
				rows[5] = r - 1;
				columns[5] = c - 1;
				rows[6] = r;
				columns[6] = c - 1;
				rows[7] = r + 1;
				columns[7] = c - 1;

				for (j = 0; j < 8; j++) {
					value = table_array[rows[j]][columns[j]];
					bool isValid = (rows[j] >= 0 && rows[j] < MAX) && (columns[j] >= 0 && columns[j] < MAX);
					if (isValid) {	// to prevent negative index and out of bounds
						if (!has_mine(value))																// to prevent remove mines
							table_array[rows[j]][columns[j]] += 1;									// sums 1 to each adjacent cell
					}
				}
			}
			else {							// to make sure that there are the properly number of mines in table
				i--;
				continue;
			}
		}

		//
		while (number_Mines != 0) {			// when nMines becomes 0 you will win the game
			print_table();

			ch = getch();
			// cursor direction
			char direction;
			switch (ch) {

				// flag mode
			case 'f':
			case 'F':
				game_mode = 1;
				do {
					print_table();
					direction = getch();
					// arrow direction
					if (direction == '8') {
						// up
						--y;
						y = (MAX + y) % MAX;
					}
					else if (direction == '2') {
						// down
						y++;
						y = y % MAX;
					}
					else if (direction == '4') {
						x--;
						x = (MAX + x) % MAX;
					}
					else if (direction == '6') {
						x++;
						x = x % MAX;
					}
					else if (direction == 'c' || direction == 'C') {
						//goto check_mode;
						game_mode = 2;
						do {
							print_table();
							direction = getch();

							// arrow direction
							if (direction == '8') {
								// up
								y--;
								y = (MAX + y) % MAX;
							}
							else if (direction == '2') {
								// down
								y++;
								y = y % MAX;
							}
							else if (direction == '4') {
								//left
								x--;
								x = (MAX + x) % MAX;
							}
							else if (direction == '6') {
								//right
								x++;
								x = x % MAX;
							}
							else if (direction == 'f' || direction == 'F') {
								break;
							}

							else if (direction == '\n') {
								value = table_array[y][x];
								surrounded_mines_num = check_mine_surrounded(table_array[y][x]);
								if (value == 0) {
									// blank case
									uncover_blank_cell(y, x);
								}
								else if (has_mine(value)) {		// mine case
									game_mode = 0;
									print_table();
									printf("\nGAME OVER\n");

									if (number_Mines == 0)
										printf("you won!!!!\n");

									else
										printf("BOOM! you LOOSE!\n");

									do {
										printf("Are you sure to exit? (y or n)? ");
										ch = getch();
										putchar('\n');
										if (ch == 'y' || ch == 'Y') {
											break;
										}
										else if (ch == 'n' || ch == 'N') {
											continue;
										}
										printf("Please answer y or n\n");
									} while (1);
									printf("See you next time!\n");

									fflush(stdin);

									return 0;
								}
								else if (value > 0 && value <= 8) {	// number case (the next cell is a mine)
									set_uncovered(table_array[y][x]);
								}
								//	break;
							}
						} while (direction != 'q' && direction != 'Q');

						if (direction == 'f' || direction == 'F') {
							game_mode = 1;
							continue;
						}

						game_mode = 0;

						break;
					}
					else if (direction == '\n') {
						value = table_array[y][x];

						if (has_mine(value)) {				// mine case
							set_flaged(table_array[y][x]);
							number_Mines -= 1;				// mine found
						}
						else if (value >= 0 && value <= 8) {	// number of mines case (the next cell is a mine)
							set_flaged(table_array[y][x]);
						}
						else if ( check_flaged(value) && surrounded_mines_num >= 0 && surrounded_mines_num <= 8) {
							set_unflaged(table_array[y][x]);
						}

						if (number_Mines == 0)
							break;
					}
				} while (direction != 'q' && direction != 'Q');
				game_mode = 0;
				break;

				// check cell
			case 'c':
			case 'C':
				game_mode = 2;
				do {
					print_table();
					direction = getch();

					// arrow direction
					if (direction == '8') {
						// up
						y--;	
						y = (MAX + y) % MAX;
					}
					else if (direction == '2') {
						// down
						y++;
						y = y % MAX;
					}
					else if (direction == '4') {
						// left
						x--;
						x = (MAX + x) % MAX;
					}
					else if (direction == '6') {
						// right
						x++;
						x = x % MAX;
					}
					else if (direction == 'f' || direction == 'F') {
						game_mode = 1;
						do {
							print_table();
							direction = getch();
							// arrow direction
							if (direction == '8') {
								// up
								--y;
								y = (MAX + y) % MAX;
							}
							else if (direction == '2') {
								// down
								y++;
								y = y % MAX;
							}
							else if (direction == '4') {
								// left
								x--;
								x = (MAX + x) % MAX;
							}
							else if (direction == '6') {
								// right
								x++;
								x = x % MAX;
							}
							else if (direction == 'c' || direction == 'C') {
								break;
							}
							else if (direction == '\n') {
								value = table_array[y][x];

								if (has_mine(value)) {				// mine case
									set_flaged(table_array[y][x]);
									number_Mines -= 1;				// mine found
								}
								else if (value >= 0 && value <= 8) {	// number of mines case (the next cell is a mine)
									set_flaged(table_array[y][x]);
								}
								else if ( check_flaged(value) && surrounded_mines_num >= 0 && surrounded_mines_num <= 8) {
									set_unflaged(table_array[y][x]);
								}

								if (number_Mines == 0)
									break;
							}
						} while (direction != 'q' && direction != 'Q');
						if (direction == 'c' || direction == 'C') {
							game_mode = 2;
							continue;
						}
						game_mode = 0;
						break;
					}

					else if (direction == '\n') {
						value = table_array[y][x];
						if (value == 0) {
							// blank case
							uncover_blank_cell(y, x);
						}
						else if (has_mine(value)) {		// mine case
							game_mode = 0;
							print_table();
							printf("\nGAME OVER\n");

							if (number_Mines == 0)
								printf("you won!!!!\n");

							else
								printf("BOOM! you LOOSE!\n");

							do {
								printf("Are you sure to exit? (y or n)? ");
								ch = getch();
								putchar('\n');
								if (ch == 'y' || ch == 'Y') {
									break;
								}
								else if (ch == 'n' || ch == 'N') {
									continue;
								}
								printf("Please answer y or n\n");
							} while (1);
							printf("See you next time!\n");

							fflush(stdin);

							return 0;
						}
						else if (value > 0 && value <= 8) {	// number case (the next cell is a mine)
							set_uncovered(table_array[y][x]);
						}
						//	break;
					}
				} while (direction != 'q' && direction != 'Q');
				game_mode = 0;

				break;

				// jump to a new game
			case 'n':
			case 'N':
				continue;

				// exit
			case 'q':
			case 'Q':
				game_mode = 0;
				print_table();
				printf("\nGAME OVER\n");

				if (number_Mines == 0)
					printf("you won!!!!\n");

				else
					printf("BOOM! you LOOSE!\n");

				do {
					printf("Are you sure to exit? (y or n)? ");
					ch = getch();
					putchar('\n');
					if (ch == 'y' || ch == 'Y') {
						break;
					}
					else if (ch == 'n' || ch == 'N') {
						continue;
					}
					printf("Please answer y or n\n");
				} while (1);
				printf("See you next time!\n");

				fflush(stdin);

				return 0;

			default:
				break;
			}
		}
	}
	return 0;
}
