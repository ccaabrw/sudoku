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
 */

int q[MAX][MAX][MAX+1];
int notdone;

/*
 * Convert digit number to printable char
 *
 */

char num2char(int n) {
	char c;

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

	if (c >= '1' && c <= '9') {
	    n = c - '0';
	} else if (c >= 'A' && c <= 'Z') {
	    n = c - 'A' + 10;
	} else if (c >= 'a' && c <= 'z') {
	    n = c - 'a' + 10;
	} else {
	    n = 0;
	}

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
	if (q[x][y][n] && !q[x][y][0]) {
	    int i,j;
	    int mx, my;

	    for (i=0; i<MAX; i++) {
		q[x][i][n] = 0;
		q[i][y][n] = 0;
	    }

	    mx = x - (x % STEP);
	    my = y - (y % STEP);
	    for (i=mx; i<mx+STEP; i++) {
		for (j=my; j<my+STEP; j++) {
		    q[i][j][n] = 0;
		}
	    }

	    q[x][y][n] = 1;
	    q[x][y][0] = n;
	    
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
	j=notdone;

	while (i<j) {
	    c = getchar();
	    if (c == '.' || c == ' ' || (c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
		n = char2num(c);
		set(i/MAX, i%MAX, n);
		i++;
	    }
	}
}

/*
 * Is a single digit the only possible one for a cell
 * that has not yet been assigned.
 *
 */

void rule_only_digit_in_cell() {
	int i,j,k;
	int c,n;

	for (i=0; i<MAX; i++) {
	    for (j=0; j<MAX; j++) {
		if (!q[i][j][0]) {
		    for (c=0, k=1; k<=MAX; k++) {
			if (q[i][j][k]) {
			    c++;
			    n=k;
			}
		    }
		    if (c == 1) {
			set(i,j,n);
		    }
		}
	    }
	}
}

void rule_only_place_in_row(){
	int i,j,k;
	int count[MAX+1], pos[MAX+1];

	for (i=0; i<MAX; i++) {
	    for (k=1; k<=MAX; k++) {
		count[k]=0;
	    }
	    for (j=0; j<MAX; j++) {
		if (!q[i][j][0]) {
		    for (k=1; k<=MAX; k++) {
			if (q[i][j][k]) {
			    count[k]++;
			    pos[k] = j;
			}
		    }
		}
	    }
	    for (k=1; k<=MAX; k++) {
		if (count[k] == 1) {
		    set(i, pos[k], k);
		}
	    }
	}
}


void rule_only_place_in_column(){
	int i,j,k;
	int count[MAX+1], pos[MAX+1];

	for (j=0; j<MAX; j++) {
	    for (k=1; k<=MAX; k++) {
		count[k]=0;
	    }
	    for (i=0; i<MAX; i++) {
		if (!q[i][j][0]) {
		    for (k=1; k<=MAX; k++) {
			if (q[i][j][k]) {
			    count[k]++;
			    pos[k] = i;
			}
		    }
		}
	    }
	    for (k=1; k<=MAX; k++) {
		if (count[k] == 1) {
		    set(pos[k], j, k);
		}
	    }
	}
}

void rule_only_in_one_row_in_group() {
	int i1,i2,j1,j2,i,j,k;
	int count[MAX+1], pos[MAX+1];

	for (i1=0; i1<MAX; i1+=STEP) {
	  for (j1=0; j1<MAX; j1+=STEP) {
	    for (k=1; k<=MAX; k++) {
	      count[k]=0;
	      pos[k]=-1;
	    }
	    for (i2=0; i2<STEP; i2++) {
	      for (j2=0; j2<STEP; j2++) {
		i = i1 + i2;
		j = j1 + j2;
		if (!q[i][j][0]) {
		  for (k=1; k<=MAX; k++) {
		    if (q[i][j][k] && pos[k] != i) {
		      count[k]++;
		      pos[k] = i;
		    }
		  }
		}
	      }
	    }
	    for (k=1; k<=MAX; k++) {
	      if (count[k] == 1) {
		for (j2=0; j2<j1; j2++) {
		  q[pos[k]][j2][k] = 0;
		}
		for (j2=j1+STEP; j2<MAX; j2++) {
		  q[pos[k]][j2][k] = 0;
		}
	      }
	    }
	  }
	}
}

void rule_only_in_one_column_in_group() {
	int i1,i2,j1,j2,i,j,k;
	int count[MAX+1], pos[MAX+1];

	for (i1=0; i1<MAX; i1+=STEP) {
	  for (j1=0; j1<MAX; j1+=STEP) {
	    for (k=1; k<=MAX; k++) {
	      count[k]=0;
	      pos[k]=-1;
	    }
	    for (i2=0; i2<STEP; i2++) {
	      for (j2=0; j2<STEP; j2++) {
		i = i1 + i2;
		j = j1 + j2;
		if (!q[i][j][0]) {
		  for (k=1; k<=MAX; k++) {
		    if (q[i][j][k] && pos[k] != j) {
		      count[k]++;
		      pos[k] = j;
		    }
		  }
		}
	      }
	    }
	    for (k=1; k<=MAX; k++) {
	      if (count[k] == 1) {
		for (i2=0; i2<i1; i2++) {
		  q[i2][pos[k]][k] = 0;
		}
		for (i2=i1+STEP; i2<MAX; i2++) {
		  q[i2][pos[k]][k] = 0;
		}
	      }
	    }
	  }
	}
}

void rule_only_in_one_sq_in_group () {
	int i1,i2,j1,j2,i,j,k;
	int posi[MAX+1], posj[MAX+1], count[MAX+1];

	for (i1=0; i1<MAX; i1+=STEP) {
	  for (j1=0; j1<MAX; j1+=STEP) {
	    for (k=1; k<=MAX; k++) {
	      count[k]=0;
	    }
	    for (i2=0; i2<STEP; i2++) {
	      for (j2=0; j2<STEP; j2++) {
	        i = i1 + i2;
	        j = j1 + j2;
	        if (!q[i][j][0]) {
		  for (k=1; k<=MAX; k++) {
		    if (q[i][j][k]) {
		      count[k]++;
		      posi[k] = i;
		      posj[k] = j;
		    }
		  }
		}
	      }
	    }
	    for (k=1; k<=MAX; k++) {
	      if (count[k] == 1) {
		set(posi[k], posj[k], k);
	      }
	    }
	  }
	}
}

void apply_rules() {
	rule_only_in_one_row_in_group();
	rule_only_in_one_column_in_group();
	rule_only_in_one_sq_in_group();
	rule_only_place_in_row();
	rule_only_place_in_column();
	rule_only_digit_in_cell();
}

/*
 * Print out the grid
 *
 */

void output_grid() {
	int i1,i2,j1,j2;
	char c;

	for (i1=0; i1<MAX; i1+=STEP) {
	  for (i2=0; i2<STEP; i2++) {
	    for (j1=0; j1<MAX; j1+=STEP) {
	      for (j2=0; j2<STEP; j2++) {
		c = q[i1+i2][j1+j2][0] ? num2char(q[i1+i2][j1+j2][0]) : '.';
		printf("%c", c);
	      }
	      printf(" ");
	    }
	    printf("\n");
	  }
	  printf("\n");
	}
}

void output_unset() {
	int i,j,k;
	char c;

	for (i=0; i<MAX; i++) {
	    for (j=0; j<MAX; j++) {
		if (!q[i][j][0]) {
		    printf("(%d,%d) =", i+1, j+1);
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

	for (i=0; i<MAX; i++) {
	    for (j=0; j<MAX; j++) {
		q[i][j][0]=0;
		for (k=1; k<=MAX; k++) {
		    q[i][j][k]=1;
		}
	    }
	}
	notdone = MAX*MAX;

	read_input();

	while (notdone) {
/*	    output_grid();	*/

	    p = notdone;
	    apply_rules();
	    if (p == notdone) {
		apply_rules();
		output_unset();
		break;
	    }
	}

/*	for (i=0; i<MAX; i++)
	    printf("=");
*/	printf("\n");

	output_grid();

	if (notdone) printf ("Not solved\n\n");

	return(0);
}

