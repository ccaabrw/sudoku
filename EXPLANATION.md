# Sudoku.c Detailed Explanation

## Overview

This is a Sudoku solver written in C that uses logical deduction rules to solve puzzles. Unlike brute-force solvers that try every possible combination, this solver applies human-like reasoning rules repeatedly until the puzzle is solved or no further progress can be made.

The program was originally written to solve a 16x16 Sudoku puzzle that appeared in The Independent newspaper in December 2009.

## Grid Size Configuration

The solver is configurable for different Sudoku grid sizes:

```c
#define STEP 3
#define MAX (STEP*STEP)
```

- `STEP`: The size of each sub-grid (typically 3 for standard 9x9 Sudoku)
- `MAX`: The total grid dimension (STEP × STEP = 9 for standard puzzles)

For a standard 9×9 Sudoku, STEP=3 creates 3×3 sub-grids with a 9×9 overall grid.

## Data Structures

### Main Grid: `q[MAX][MAX][MAX+1]`

This 3-dimensional array is the core data structure:

- **First dimension (i)**: Row index (0 to MAX-1)
- **Second dimension (j)**: Column index (0 to MAX-1)  
- **Third dimension (k)**: Stores two types of information:
  - `q[i][j][0]`: The assigned digit for cell (i,j), or 0 if unassigned
  - `q[i][j][1..MAX]`: Boolean flags indicating which digits are still possible for this cell

**Example**: For cell (3,5):
- `q[3][5][0] = 7` means the cell contains the digit 7
- `q[3][5][0] = 0` means the cell is empty
- `q[3][5][4] = 1` means digit 4 is still possible for this cell
- `q[3][5][4] = 0` means digit 4 has been eliminated as impossible

### Global Variable: `notdone`

Tracks the number of cells still unsolved. Starts at MAX×MAX and decrements each time a cell is assigned a value.

## Key Functions

### Utility Functions

#### `char num2char(int n)`
Converts a digit to its printable character representation:
- Digits 1-9 → '1'-'9'
- Digits 10+ → 'A', 'B', 'C', etc.

#### `char2num(char c)`
Converts an input character to a digit:
- '1'-'9' → 1-9
- 'A'-'Z' or 'a'-'z' → 10-35
- '.' or ' ' or '0' → 0 (empty cell)

### Core Solving Functions

#### `set(int x, int y, int n)`
Assigns digit `n` to cell (x,y) and eliminates it as a possibility from related cells:

1. **Row elimination**: Removes `n` from all cells in row x
2. **Column elimination**: Removes `n` from all cells in column y
3. **Sub-grid elimination**: Removes `n` from all cells in the same 3×3 sub-grid
4. **Sets the value**: `q[x][y][0] = n` and `q[x][y][n] = 1`
5. **Decrements** `notdone` counter

#### `read_input()`
Reads the initial puzzle from standard input:
- Accepts digits, letters (for larger grids), dots (.), and spaces as empty cells
- Ignores other characters like newlines
- Uses `set()` to place the given digits

### Solving Rules

The solver applies six logical deduction rules repeatedly:

#### 1. `rule_only_digit_in_cell()`
**"Naked Single"** - If a cell has only one possible digit remaining, assign it.

For each empty cell:
- Count how many digits are still possible
- If only one digit remains possible, set that digit

#### 2. `rule_only_place_in_row()`
**"Hidden Single in Row"** - If a digit can only go in one cell in a row, place it there.

For each row:
- For each digit 1-MAX, count how many cells in the row can still contain it
- If only one cell can contain the digit, set it there

#### 3. `rule_only_place_in_column()`
**"Hidden Single in Column"** - If a digit can only go in one cell in a column, place it there.

Similar to row rule, but operates on columns.

#### 4. `rule_only_in_one_row_in_group()`
**"Pointing Pairs/Triples - Row"** - If a digit in a sub-grid can only appear in one row within that sub-grid, eliminate it from that row outside the sub-grid.

For each 3×3 sub-grid:
- For each digit, check if it only appears in one row within the sub-grid
- If so, eliminate that digit from the rest of that row (outside the sub-grid)

#### 5. `rule_only_in_one_column_in_group()`
**"Pointing Pairs/Triples - Column"** - Similar to above, but for columns.

#### 6. `rule_only_in_one_sq_in_group()`
**"Hidden Single in Sub-grid"** - If a digit can only go in one cell within a sub-grid, place it there.

For each 3×3 sub-grid:
- For each digit, count how many cells in the sub-grid can contain it
- If only one cell can contain it, set it there

#### `apply_rules()`
Applies all six rules in sequence.

### Output Functions

#### `output_grid()`
Prints the solved (or partially solved) grid in a formatted layout:
- Groups cells into 3×3 sub-grids with spacing
- Shows assigned digits as characters
- Shows unassigned cells as '.'

#### `output_unset()`
For debugging: prints all unassigned cells and their remaining possible values.

Example output:
```
(2,3) = 4 7 9
(2,7) = 3 4 9
```

## Main Algorithm

The `main()` function orchestrates the solving process:

### 1. Initialization
```c
// Initialize all cells as empty with all digits possible
for (i=0; i<MAX; i++) {
    for (j=0; j<MAX; j++) {
        q[i][j][0] = 0;           // No digit assigned
        for (k=1; k<=MAX; k++) {
            q[i][j][k] = 1;       // All digits possible
        }
    }
}
notdone = MAX*MAX;  // All cells need solving
```

### 2. Read Input
```c
read_input();
```
Reads the initial puzzle configuration from stdin.

### 3. Iterative Solving Loop
```c
while (notdone) {
    p = notdone;
    apply_rules();
    if (p == notdone) {
        // No progress made, apply rules once more
        apply_rules();
        output_unset();  // Show remaining possibilities
        break;
    }
}
```

The loop:
- Saves the current `notdone` count
- Applies all solving rules
- If no cells were solved (notdone unchanged), tries one more time
- If still no progress, gives up and shows what's left

### 4. Output Results
```c
output_grid();
if (notdone) printf("Not solved\n\n");
```

## Input Format

The program reads from standard input and expects:
- 81 characters for a 9×9 puzzle (MAX×MAX characters)
- Digits '1'-'9' for given values
- '.' or space for empty cells
- Letters 'A'-'Z' for grids larger than 9×9
- Newlines and other characters are ignored

### Example Input (9×9):
```
53..7....
6..195...
.98....6.
8...6...3
4..8.3..1
7...2...6
.6....28.
...419..5
....8..79
```

## Output Format

The output shows the grid with 3×3 sub-grids separated by spaces and blank lines:

```
534 678 912
672 195 348
198 342 567

859 761 423
426 853 791
713 924 856

961 537 284
287 419 635
345 286 179
```

## Limitations

1. **Not Exhaustive**: This solver only uses logical deduction rules. It cannot solve puzzles that require:
   - Trial and error (backtracking)
   - More advanced techniques (X-Wing, Swordfish, etc.)

2. **No Error Handling**: The code assumes valid input and doesn't check for:
   - Contradictory initial conditions
   - Invalid puzzle configurations

3. **Fixed Grid Structure**: While configurable via STEP, the code assumes square sub-grids.

## Solving Strategy

The solver uses a **constraint propagation** approach:

1. Start with all digits possible in all empty cells
2. Use given digits to eliminate possibilities
3. Repeatedly apply logical rules to:
   - Assign digits when only one possibility remains
   - Assign digits when they can only go in one place
   - Eliminate possibilities based on sub-grid constraints
4. Continue until solved or stuck

This approach works well for easy to medium difficulty puzzles but may fail on harder puzzles that require more sophisticated techniques or backtracking.

## Technical Notes

- **No Dynamic Memory**: Uses only static arrays, making it fast and simple
- **No Recursion**: Uses iterative rule application instead
- **Performance**: O(MAX²) per rule application, efficient for standard puzzles
- **Portability**: Standard C with no external dependencies

## Usage Example

Compile and run:
```bash
gcc -o sudoku sudoku.c
./sudoku < puzzle.txt
```

Or pipe directly:
```bash
echo "53..7....6..195....98....6.8...6...34..8.3..17...2...6.6....28....419..5....8..79" | ./sudoku
```

## Historical Context

Written in December 2009 to solve a 16×16 Sudoku puzzle from The Independent newspaper. The configurable STEP parameter allows it to handle:
- 4×4 puzzles (STEP=2)
- 9×9 puzzles (STEP=3) - standard
- 16×16 puzzles (STEP=4)
- 25×25 puzzles (STEP=5)
