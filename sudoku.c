#include <stdio.h>

/*
 * Choose size of sub-grid - ie: 2, 3, 4 or 5
 * [or six but then we need to use zero!]
 *
 */

#define STEP 3
#define MAX (STEP*STEP)

/*
 * Global vars to store state
 *
 * q[MAX][MAX][MAX+1]: 3D array storing the grid state
 *   - q[i][j][0]: The assigned digit for cell (i,j), or 0 if unassigned
 *   - q[i][j][1..MAX]: Boolean flags indicating if digit k is still possible for cell (i,j)
 *                       1 = possible, 0 = eliminated
 *
 * notdone: Counter of cells that still need to be solved (starts at MAX*MAX)
 */

int q[MAX][MAX][MAX+1];
int notdone;

/*
 * Convert digit number to printable char
 *
 */

char num2char(int n) {
	char c;

	/* For n=1-9: use '1'-'9', for n>=10: use 'A', 'B', 'C', etc. */
	c = n > 9 ? 'A' - 10 : '0';
	c += n;
	return(c);
}

/*
 * Convert an input char to digit number
 *
 */

int char2num(char c) {
	int n;

	/* Convert '1'-'9' to 1-9 */
	if (c >= '1' && c <= '9') {
	    n = c - '0';
	/* Convert 'A'-'Z' to 10-35 (for larger grids) */
	} else if (c >= 'A' && c <= 'Z') {
	    n = c - 'A' + 10;
	/* Convert 'a'-'z' to 10-35 (case insensitive) */
	} else if (c >= 'a' && c <= 'z') {
	    n = c - 'a' + 10;
	/* Dots, spaces, and '0' represent empty cells */
	} else {
	    n = 0;
	}

	/* Validate digit is within grid bounds */
	if (n > MAX) {
	    n = 0;
	}

	return(n);
}

/*
 * Set the digit for a cell and clear other possibles
 * (will not overwrite - needs error-handling?)
 *
 */

int set(int x, int y, int n) {
	/* Only set if n is possible for this cell and cell is not already assigned */
	if (q[x][y][n] && !q[x][y][0]) {
	    int i,j;
	    int mx, my;

	    /* Eliminate digit n from all cells in the same row and column */
	    for (i=0; i<MAX; i++) {
		q[x][i][n] = 0;  /* Remove from row x */
		q[i][y][n] = 0;  /* Remove from column y */
	    }

	    /* Calculate top-left corner of the sub-grid containing (x,y) */
	    mx = x - (x % STEP);
	    my = y - (y % STEP);
	    /* Eliminate digit n from all cells in the same sub-grid */
	    for (i=mx; i<mx+STEP; i++) {
		for (j=my; j<my+STEP; j++) {
		    q[i][j][n] = 0;
		}
	    }

	    /* Assign the digit to this cell */
	    q[x][y][n] = 1;
	    q[x][y][0] = n;
	    
	    /* One less cell to solve */
	    notdone--;
	}
}

/*
 * Read in table of initial known digits
 * [could be adapted to detect size of input and hence grid ...]
 *
 */

void read_input() {
	int i,j,n;
	char c;

	i=0;
	j=notdone;  /* Total cells to read (MAX*MAX) */

	/* Read exactly MAX*MAX valid characters from stdin */
	while (i<j) {
	    c = getchar();
	    /* Accept only valid characters (digits, letters, dots, spaces) */
	    if (c == '.' || c == ' ' || (c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
		n = char2num(c);
		/* Convert linear position to (row, col) and set the digit */
		set(i/MAX, i%MAX, n);
		i++;
	    }
	    /* Ignore other characters like newlines */
	}
}

/*
 * RULE 1: Naked Single
 * Is a single digit the only possible one for a cell
 * that has not yet been assigned.
 *
 */

void rule_only_digit_in_cell() {
	int i,j,k;
	int c,n;

	/* Check every cell in the grid */
	for (i=0; i<MAX; i++) {
	    for (j=0; j<MAX; j++) {
		/* Only process cells that haven't been assigned yet */
		if (!q[i][j][0]) {
		    /* Count how many digits are still possible for this cell */
		    for (c=0, k=1; k<=MAX; k++) {
			if (q[i][j][k]) {
			    c++;   /* Increment count */
			    n=k;   /* Remember this digit */
			}
		    }
		    /* If only one digit is possible, assign it */
		    if (c == 1) {
			set(i,j,n);
		    }
		}
	    }
	}
}

/*
 * RULE 2: Hidden Single in Row
 * If a digit can only appear in one cell within a row, place it there
 */

void rule_only_place_in_row(){
	int i,j,k;
	int count[MAX+1], pos[MAX+1];

	/* Check each row */
	for (i=0; i<MAX; i++) {
	    /* Initialize counters for each digit */
	    for (k=1; k<=MAX; k++) {
		count[k]=0;
	    }
	    /* Count how many cells in this row can contain each digit */
	    for (j=0; j<MAX; j++) {
		if (!q[i][j][0]) {  /* Only unassigned cells */
		    for (k=1; k<=MAX; k++) {
			if (q[i][j][k]) {
			    count[k]++;      /* Increment count for digit k */
			    pos[k] = j;      /* Remember column position */
			}
		    }
		}
	    }
	    /* If a digit can only go in one cell, place it there */
	    for (k=1; k<=MAX; k++) {
		if (count[k] == 1) {
		    set(i, pos[k], k);
		}
	    }
	}
}


/*
 * RULE 3: Hidden Single in Column
 * If a digit can only appear in one cell within a column, place it there
 */

void rule_only_place_in_column(){
	int i,j,k;
	int count[MAX+1], pos[MAX+1];

	/* Check each column */
	for (j=0; j<MAX; j++) {
	    /* Initialize counters for each digit */
	    for (k=1; k<=MAX; k++) {
		count[k]=0;
	    }
	    /* Count how many cells in this column can contain each digit */
	    for (i=0; i<MAX; i++) {
		if (!q[i][j][0]) {  /* Only unassigned cells */
		    for (k=1; k<=MAX; k++) {
			if (q[i][j][k]) {
			    count[k]++;      /* Increment count for digit k */
			    pos[k] = i;      /* Remember row position */
			}
		    }
		}
	    }
	    /* If a digit can only go in one cell, place it there */
	    for (k=1; k<=MAX; k++) {
		if (count[k] == 1) {
		    set(pos[k], j, k);
		}
	    }
	}
}

/*
 * RULE 4: Pointing Pairs/Triples - Row
 * If a digit within a sub-grid can only appear in one row of that sub-grid,
 * eliminate it from the rest of that row (outside the sub-grid)
 */

void rule_only_in_one_row_in_group() {
	int i1,i2,j1,j2,i,j,k;
	int count[MAX+1], pos[MAX+1];

	/* Iterate over each STEP×STEP sub-grid */
	for (i1=0; i1<MAX; i1+=STEP) {
	  for (j1=0; j1<MAX; j1+=STEP) {
	    /* Initialize counters */
	    for (k=1; k<=MAX; k++) {
	      count[k]=0;    /* How many rows in sub-grid contain digit k */
	      pos[k]=-1;     /* Which row contains digit k */
	    }
	    /* Check each cell in the sub-grid */
	    for (i2=0; i2<STEP; i2++) {
	      for (j2=0; j2<STEP; j2++) {
		i = i1 + i2;
		j = j1 + j2;
		if (!q[i][j][0]) {
		  for (k=1; k<=MAX; k++) {
		    /* If this cell can contain k and it's a new row */
		    if (q[i][j][k] && pos[k] != i) {
		      count[k]++;   /* Digit k appears in another row */
		      pos[k] = i;   /* Remember the row */
		    }
		  }
		}
	      }
	    }
	    /* If digit k only appears in one row within the sub-grid */
	    for (k=1; k<=MAX; k++) {
	      if (count[k] == 1) {
		/* Eliminate k from that row outside the sub-grid (left side) */
		for (j2=0; j2<j1; j2++) {
		  q[pos[k]][j2][k] = 0;
		}
		/* Eliminate k from that row outside the sub-grid (right side) */
		for (j2=j1+STEP; j2<MAX; j2++) {
		  q[pos[k]][j2][k] = 0;
		}
	      }
	    }
	  }
	}
}

/*
 * RULE 5: Pointing Pairs/Triples - Column
 * If a digit within a sub-grid can only appear in one column of that sub-grid,
 * eliminate it from the rest of that column (outside the sub-grid)
 */

void rule_only_in_one_column_in_group() {
	int i1,i2,j1,j2,i,j,k;
	int count[MAX+1], pos[MAX+1];

	/* Iterate over each STEP×STEP sub-grid */
	for (i1=0; i1<MAX; i1+=STEP) {
	  for (j1=0; j1<MAX; j1+=STEP) {
	    /* Initialize counters */
	    for (k=1; k<=MAX; k++) {
	      count[k]=0;    /* How many columns in sub-grid contain digit k */
	      pos[k]=-1;     /* Which column contains digit k */
	    }
	    /* Check each cell in the sub-grid */
	    for (i2=0; i2<STEP; i2++) {
	      for (j2=0; j2<STEP; j2++) {
		i = i1 + i2;
		j = j1 + j2;
		if (!q[i][j][0]) {
		  for (k=1; k<=MAX; k++) {
		    /* If this cell can contain k and it's a new column */
		    if (q[i][j][k] && pos[k] != j) {
		      count[k]++;   /* Digit k appears in another column */
		      pos[k] = j;   /* Remember the column */
		    }
		  }
		}
	      }
	    }
	    /* If digit k only appears in one column within the sub-grid */
	    for (k=1; k<=MAX; k++) {
	      if (count[k] == 1) {
		/* Eliminate k from that column outside the sub-grid (top) */
		for (i2=0; i2<i1; i2++) {
		  q[i2][pos[k]][k] = 0;
		}
		/* Eliminate k from that column outside the sub-grid (bottom) */
		for (i2=i1+STEP; i2<MAX; i2++) {
		  q[i2][pos[k]][k] = 0;
		}
	      }
	    }
	  }
	}
}

/*
 * RULE 6: Hidden Single in Sub-grid
 * If a digit can only appear in one cell within a sub-grid, place it there
 */

void rule_only_in_one_sq_in_group () {
	int i1,i2,j1,j2,i,j,k;
	int posi[MAX+1], posj[MAX+1], count[MAX+1];

	/* Iterate over each STEP×STEP sub-grid */
	for (i1=0; i1<MAX; i1+=STEP) {
	  for (j1=0; j1<MAX; j1+=STEP) {
	    /* Initialize counters for each digit */
	    for (k=1; k<=MAX; k++) {
	      count[k]=0;
	    }
	    /* Count how many cells in sub-grid can contain each digit */
	    for (i2=0; i2<STEP; i2++) {
	      for (j2=0; j2<STEP; j2++) {
	        i = i1 + i2;
	        j = j1 + j2;
	        if (!q[i][j][0]) {  /* Only unassigned cells */
		  for (k=1; k<=MAX; k++) {
		    if (q[i][j][k]) {
		      count[k]++;    /* Increment count for digit k */
		      posi[k] = i;   /* Remember row position */
		      posj[k] = j;   /* Remember column position */
		    }
		  }
		}
	      }
	    }
	    /* If digit k can only go in one cell within sub-grid */
	    for (k=1; k<=MAX; k++) {
	      if (count[k] == 1) {
		set(posi[k], posj[k], k);
	      }
	    }
	  }
	}
}

/*
 * Apply all solving rules in sequence
 * Order matters: more restrictive rules first to reduce search space
 */

void apply_rules() {
	rule_only_in_one_row_in_group();    /* Pointing pairs/triples - rows */
	rule_only_in_one_column_in_group(); /* Pointing pairs/triples - columns */
	rule_only_in_one_sq_in_group();     /* Hidden singles in sub-grids */
	rule_only_place_in_row();           /* Hidden singles in rows */
	rule_only_place_in_column();        /* Hidden singles in columns */
	rule_only_digit_in_cell();          /* Naked singles */
}

/*
 * Print out the grid
 *
 */

void output_grid() {
	int i1,i2,j1,j2;
	char c;

	/* Print grid with sub-grids separated by spaces */
	for (i1=0; i1<MAX; i1+=STEP) {         /* For each row of sub-grids */
	  for (i2=0; i2<STEP; i2++) {          /* For each row within sub-grid */
	    for (j1=0; j1<MAX; j1+=STEP) {     /* For each column of sub-grids */
	      for (j2=0; j2<STEP; j2++) {      /* For each column within sub-grid */
		/* Display assigned digit or '.' for empty cell */
		c = q[i1+i2][j1+j2][0] ? num2char(q[i1+i2][j1+j2][0]) : '.';
		printf("%c", c);
	      }
	      printf(" ");  /* Space between sub-grids */
	    }
	    printf("\n");
	  }
	  printf("\n");  /* Blank line between rows of sub-grids */
	}
}

/*
 * Output unassigned cells and their remaining possible values
 * Used for debugging when puzzle cannot be fully solved
 */

void output_unset() {
	int i,j,k;
	char c;

	for (i=0; i<MAX; i++) {
	    for (j=0; j<MAX; j++) {
		/* Only show cells that haven't been assigned */
		if (!q[i][j][0]) {
		    printf("(%d,%d) =", i+1, j+1);  /* 1-indexed for readability */
		    /* List all digits still possible for this cell */
		    for (k=1; k<=MAX; k++) {
			if (q[i][j][k]) {
			    printf(" %d", k);
			}
		    }
		    printf("\n");
		}
	    }
	    printf("\n");
	}
}


/*
 * Calls everything else
 * (needs to handle not-solved condition?)
 *
 */

int main() {
	int i,j,k,p;

	/* Initialize the grid: all cells empty with all digits possible */
	for (i=0; i<MAX; i++) {
	    for (j=0; j<MAX; j++) {
		q[i][j][0]=0;              /* No digit assigned yet */
		for (k=1; k<=MAX; k++) {
		    q[i][j][k]=1;          /* All digits are possible */
		}
	    }
	}
	notdone = MAX*MAX;  /* Total cells that need solving */

	/* Read the initial puzzle from stdin */
	read_input();

	/* Main solving loop: apply rules until solved or stuck */
	while (notdone) {
/*	    output_grid();	*/  /* Uncomment to see progress */

	    p = notdone;  /* Save current unsolved count */
	    apply_rules();
	    /* If no progress was made */
	    if (p == notdone) {
		apply_rules();    /* Try one more time */
		output_unset();   /* Show remaining possibilities */
		break;            /* Give up - can't solve further */
	    }
	}

/*	for (i=0; i<MAX; i++)
	    printf("=");      Separator line (commented out) */
	printf("\n");

	/* Output the final grid (solved or partially solved) */
	output_grid();

	/* Indicate if puzzle wasn't fully solved */
	if (notdone) printf ("Not solved\n\n");

	return(0);
}

