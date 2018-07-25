/* Extended precision integer calculator
 * Implements +, *, and ^ (power of) operations
 *
 * Skeleton code written by Jianzhong Qi, March 2018
 * Functions implemented by Michelle Anggana (931735), April 2018
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INT_SIZE	100	/* max number of digits per integer value */
#define LINE_LEN	103	/* maximum length of any input line */
#define NUM_VARS	10	/* number of different huge int "variables" */

#define ASN_OP		'='	/* assignment operator */
#define ECH_OP		'?'	/* echo operator */
#define ADD_OP		'+'	/* addition operator */
#define MUL_OP		'*'	/* multiplication operator */
#define POW_OP		'^'	/* power of operator */

#define OPR1_POS	1	/* position of the first operand */
#define OPR2_POS	3	/* position of the second operand */
#define OP_POS		2	/* position of the operator */

#define CH_ZERO		'0'	/* character 0 */

#define EXIT_CMD 	"exit"	/* command to exit */
#define PROMPT		"> "	/* command prompt */
#define CMT_FLAG	'%'		/* indicator for comment line */

typedef int digit_t;				/* a decimal digit */
typedef digit_t huge_t[INT_SIZE];	/* one huge int "variable" */

/* add your constant and type definitions here */
#define CH_NULL     '\0'    /* null byte */
#define VAR_PRE     'n'     /* variable prefix */
#define MAX_DIGIT   9       /* largest possible single digit value */
#define MIN_LEN     1       /* minimum length of a number */

/****************************************************************/

/* function prototypes */
void read_line(char *line, int max_len);
void init(huge_t vars[], int lens[]);
void echo(huge_t vars[], int lens[], int opr1_index);
void assign(huge_t vars[], int lens[], int opr1_index, char *opr2_str);
void add(huge_t vars[], int lens[], int opr1_index, char *opr2_str);
void multiply(huge_t vars[], int lens[], int opr1_index, char *opr2_str);
void power(huge_t vars[], int lens[], int opr1_index, char *opr2_str);

/* add your function prototypes here */
int check_replace_count(huge_t vars[], int lens[], char *opr2_str);
void char_to_int(huge_t var, int len, char *var_str);
void int_to_char(huge_t var, int len, char *var_str);
void carry_over(huge_t var, int *len, int max_len);
void zero_handler(huge_t var, int *len);
int equality(huge_t var1, huge_t var2, int len_var1, int len_var2);
/****************************************************************/

/* main function controls all the action, do NOT modify this function */
int
main(int argc, char *argv[]) {
    char line[LINE_LEN+1];		/* to hold the input line */
    huge_t vars[NUM_VARS];		/* to hold 10 huge integers */
    int lens[NUM_VARS];			/* to hold the length of the 10 vars */

    int opr1_index;				/* index of the first operand in command */
    char op;					/* operator in command */

    init(vars, lens);

    while (1) {
        printf(PROMPT);						/* print prompt */
        read_line(line, LINE_LEN);			/* read one line of command */

        if (line[0] == CMT_FLAG) {			/* print comment in the test data */
            printf("%s\n", line);			/* used to simplify marking */
            continue;
        }

        if (strcmp(line, EXIT_CMD) == 0) {	/* see if command is "exit" */
            return 0;
        }

        opr1_index = line[OPR1_POS] - CH_ZERO;/* first var number at line[1] */
        op = line[OP_POS];					/* operator at line[2] */

        if (op == ECH_OP) {					/* print out the variable */
            echo(vars, lens, opr1_index);
            continue;
        }

        /* do the calculation, second operand starts at line[3] */
        if (op == ASN_OP) {
            assign(vars, lens, opr1_index, line+OPR2_POS);
        } else if (op == ADD_OP) {
            add(vars, lens, opr1_index, line+OPR2_POS);
        } else if (op == MUL_OP) {
            multiply(vars, lens, opr1_index, line+OPR2_POS);
        } else if (op == POW_OP) {
            power(vars, lens, opr1_index, line+OPR2_POS);
        }
    }

    /* all done; take some rest */
    return 0;
}

/* read a line of input into the array passed as argument */
void
read_line(char *line, int max_len) {
    int i = 0, c;
    while (((c = getchar()) != EOF) && (c != '\n') && (c != '\r')) {
        if (i < max_len) {
            line[i++] = c;
        } else {
            printf("Invalid input line, toooooooo long.\n");
            exit(0);
        }
    }
    line[i] = '\0';
}

/* print out a huge integer */
void echo(huge_t vars[], int lens[], int opr1_index) {
    int i;

    /* print the digits in a reverse order */
    for (i = lens[opr1_index]-1; i >= 0; i--) {
        printf("%d", vars[opr1_index][i]);
    }

    printf("\n");
}


/****************************************************************/

/* add code below to complete the function bodies,
 * add more functions where appropriate.
 */

/* set the vars array to zeros */
void
init(huge_t vars[], int lens[]) {
    int i, j;
    for (i = 0; i < NUM_VARS; i++) {
        for (j = 0; j < INT_SIZE; j++) {
            vars[i][j] = 0;
        }
        lens[i] = MIN_LEN;
    }

}

/* process the '=' operator */
void
assign(huge_t vars[], int lens[], int opr1_index, char *opr2_str) {
    int i;

    /* reset the variable to 100 zeros */
    for (i = 0; i < lens[opr1_index]; i++) {
        vars[opr1_index][i] = 0;
    }

    /* updates new length and reassign the new value to the corresponding
     * variable
     */
    lens[opr1_index] = check_replace_count(vars, lens, opr2_str);

    /* converts the characters to their equivalent integers before storing */
    char_to_int(vars[opr1_index], lens[opr1_index], opr2_str);
}

/* process the '+' operator */
void
add(huge_t vars[], int lens[], int opr1_index, char *opr2_str) {
    int opr1_len, opr2_len, i = 0, j = 0;
    opr1_len = lens[opr1_index];
    opr2_len = check_replace_count(vars, lens, opr2_str);

    /* finds the longer number and update the length of opr1 to
     * that of the longer number
     */
    lens[opr1_index] = (opr1_len > opr2_len) ? opr1_len : opr2_len;

    /* converts the characters to their equivalent integers before adding */
    for (i = opr2_len - 1; i >= 0; i--) {
        vars[opr1_index][i] += opr2_str[j] - CH_ZERO;
        j++;
    }

    carry_over(vars[opr1_index], &lens[opr1_index], lens[opr1_index]);
}

/* process the '*' operator */
void
multiply(huge_t vars[], int lens[], int opr1_index, char *opr2_str) {
    int opr1_len, opr2_len, i, j, count = 0;
    huge_t total = {0};
    opr1_len = lens[opr1_index];
    opr2_len = check_replace_count(vars, lens, opr2_str);

    /* do long multiplication for every position in opr2 against opr1 */
    for (i = opr2_len - 1; i >= 0; i--) {
        for (j = 0; j < opr1_len; j++) {
            /* guard to avoid adding beyond 100 digits */
            if (j + count == INT_SIZE) {
                break;
            }
            /* add multiplication result to total */
            total[j + count] += (opr2_str[i] - CH_ZERO) * vars[opr1_index][j];
        }
        count++;
    }

    /* replace opr1 with the total sum */
    for (i = 0; i < INT_SIZE; i++) {
        vars[opr1_index][i] = total[i];
    }

    carry_over(vars[opr1_index], &lens[opr1_index], INT_SIZE);
}

/* process the '^' operator */
void
power(huge_t vars[], int lens[], int opr1_index, char *opr2_str) {
    int i, counter_len = MIN_LEN, opr2_len;
    char opr1_str[INT_SIZE];
    huge_t counter = {0}, opr2_int = {0};
    opr2_len = check_replace_count(vars, lens, opr2_str);

    /* guard for power to 0 to reset the variable to value 1 and length 1 */
    if (opr2_str[0] == CH_ZERO) {
        for (i = 0; i < INT_SIZE; i++) {
            vars[opr1_index][i] = 0;
        }
        vars[opr1_index][0] = 1;
        lens[opr1_index] = MIN_LEN;
        return;
    }

    /* guard for 0 to the power of a value to return 0 without multiplication */
    if (equality(vars[opr1_index], counter, lens[opr1_index], MIN_LEN)) {
        return;
    }

    /* store opr2_str as huge_t for equality check*/
    char_to_int(opr2_int, opr2_len, opr2_str);
    /* converts opr1 to string to pass to the multiply function */
    int_to_char(vars[opr1_index], lens[opr1_index], opr1_str);

    /* prevent extra multiplication from being performed by initializing counter
     * to 1
     */
    counter[0] = 1;

    /* performs multiplication of opr1 by itself while the counter is not equal
     * to value specified by opr2
     */
    while (!equality(counter, opr2_int, counter_len, opr2_len)) {
        multiply(vars, lens, opr1_index, opr1_str);
        counter[0]++;
        carry_over(counter, &counter_len, counter_len);
    }
}

/* checks if opr2 is a stored variable and replaces it with the character
 * equivalent then counts and returns the length of the character array
 */
int
check_replace_count(huge_t vars[], int lens[], char *opr2_str) {
    int i, var_num, count = 0;
    /* if a variable is given as the second operator, replace it with a string
     * of the value stored in the variable
     */
    if (opr2_str[0] == VAR_PRE) {
        var_num = opr2_str[1] - CH_ZERO;
        int_to_char(vars[var_num], lens[var_num], opr2_str);
    }
    i = 0;
    /* return the length of the string */
    while (opr2_str[i] != CH_NULL) {
        count++;
        i++;
    }
    return count;
}

/* converts a string of numbers to a corresponding array of integers */
void
char_to_int(huge_t var, int len, char *var_str){
    int i, j = 0;
    for (i = len - 1; i >= 0; i--) {
        var[j] = var_str[i] - CH_ZERO;
        j++;
    }
}

/* converts an integer array to a corresponding string  */
void
int_to_char(huge_t var, int len, char *var_str){
    int i, j = 0;
    for (i = len - 1; i >= 0; i--) {
        var_str[j] = (char) (var[i] + CH_ZERO);
        j++;
    }
    /* null terminate character array */
    var_str[j] = CH_NULL;
}

/* takes an array of unprocessed integers and carry over any digit
 * that exceeds 9
 */
void
carry_over(huge_t var, int *len, int max_len) {
    int i;
    for (i = 0; i < max_len; i++) {
        /* keeps count of how much to carry over by reducing the current
         * integer by 10 until it is less than 10, then adding the count to the
         * next digit
         */
        while (var[i] > MAX_DIGIT) {
            /* avoid carry over beyond 100 digits */
            if (i == INT_SIZE - 1) {
                break;
            }
            var[i + 1] += 1;
            var[i] -= 10;

            /* if the length of current int is max_len and digit > 9,
             * increment current length by 1 due to carry over, unless
             * length is already maximum
             */
            if (i + 1 == max_len && max_len != INT_SIZE) {
                *len = max_len + 1;
            }
        }
    }
    /* if all 100 digits are used, check for leading zeros */
    if (max_len == INT_SIZE) {
        zero_handler(var, len);
    }
}

/* takes an array of integers and check for leading zeros to remove them */
void
zero_handler(huge_t var, int *len) {
    int count_0 = 0, i = INT_SIZE - 1;
    /* handle digit 100 to reduce it until it is less than 10 */
    while (var[INT_SIZE - 1] > MAX_DIGIT) {
        var[INT_SIZE - 1] -= 10;
    }
    /* count the number of leading zeros and reduce length by the count
     * to avoid printing them
     */
    while (var[i] == 0) {
        count_0++;
        i--;
        /* guard for 100 0s to print the last zero */
        if (count_0 == INT_SIZE - 1) {
            break;
        }
    }
    *len = INT_SIZE - count_0;
}

/* checks for whether two huge_t variables are equivalent */
int
equality(huge_t var1, huge_t var2, int len_var1, int len_var2) {
    int i;
    /* check for length before checking value by value to break out early if
     * length does not match
     */
    if (len_var1 == len_var2) {
        for (i = len_var1; i >= 0; i--) {
            if (var1[i] != var2[i]) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

/* Algorithms are fun!!! */