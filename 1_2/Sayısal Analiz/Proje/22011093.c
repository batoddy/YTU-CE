#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_EXPR_SIZE 100
#define GAUSS_SEIDAL_START_ROOT 0
#define ERR 0.00002
#define H 0.00001

void replace_x_with_input(char *expr, float input);
float parse_expression(const char *expr);
float parse_term(const char **expr);
float parse_factor(const char **expr);
float parse_number(const char **expr);
float evaluate_function(const char *func, float arg);
float parse_log_base(const char **expr);
float f(float x, char *input_expr);

char *get_f_string();
// float f(float x);

float df(float (*f)(float, char *), float, char *);

int bisection_methot();
float bisection_root(float a, float b, int n, char *func_str);
int get_sign(float a, float b);

int regula_falsi_methot();
float regula_falsi_root(float a, float b, int n, char *func_str);

int newton_rapshon_methot();
float newton_rapshon_iteration(float x, char *func_str);

int reverse_matrix();
float **determine_matrix(int N);
float **inverse_matrix(float **matrix, int N);
void print_matrix(float **matrix, int N);
int gauss_elimination_methot();
void matris_elimination(float **matrix, int N);

int gauss_seidal_methot();
void gauss_seidal_iteration(float **matrix, float *variable, float *result, int N);

int numeric_dervative();
float df_i(float (*f)(float, char *), float, float, char *);
float df_g(float (*f)(float, char *), float, float, char *);
float df_m(float (*f)(float, char *), float, float, char *);

int simpson_methot();
int trapez_methot();
int gregory_newton_methot();
float f_gnm(float *coefficient, int step, float x);

void print_f_gnm(float *coefficient, int step);

int main()
{
    int exit_flag = 0;
    while (exit_flag == 0)
    {
        int selection;
        printf("\n\tMethot\t\t|Durum\n");
        printf("1-Bisection Method\t|+\n");
        printf("2-Regula-Falsi Method\t|+\n");
        printf("3-Newton-Rapshon Method\t|+\n");
        printf("4-Reverse of Matrix\t|+\n");
        printf("5-Gauss Elimination\t|+\n");
        printf("6-Gauss Seidal Methot\t|+\n");
        printf("7-Numeric Derivative\t|+\n");
        printf("8-Simpson Methot\t|+\n");
        printf("9-Trapez Methot\t\t|+\n");
        printf("10-Gergory Newton Methot|+\n\n");
        // printf("0-STRING FUNCT PARSING\t|+\n");
        printf("!!!!!!!!==================== DIKKAT ====================!!!!!!!!!\n");
        printf("- x'in katsayilarini (5*x) seklinde giriniz \n");
        printf("- Islem onceliklerini parantez ile belirtmeyi unutmayiniz\n\n");
        printf("Press [-1] for exit \n\n");
        printf("Enter the methot: ");
        scanf("%d", &selection);

        switch (selection)
        {
        case 1:
            bisection_methot();
            break;
        case 2:
            regula_falsi_methot();
            break;
        case 3:
            newton_rapshon_methot();
            break;
        case 4:
            reverse_matrix();
            break;
        case 5:
            gauss_elimination_methot();
            break;
        case 6:
            gauss_seidal_methot();
            break;
        case 7:
            numeric_dervative();
            break;
        case 8:
            simpson_methot();
            break;
        case 9:
            trapez_methot();
            break;
        case 10:
            gregory_newton_methot();
            break;
        case -1:
            exit_flag = -1;
            break;
        default:
            break;
        }
    }
}

float f(float x, char *input_expr)
{
    char func[100];
    strcpy(func, input_expr);
    replace_x_with_input(func, x);
    // printf("Modified expression: %s\n", input_expr);
    return parse_expression(func);
}

char *get_f_string(char *input_expr)
{
    getchar();
    printf("Enter the function: ");
    fgets(input_expr, MAX_EXPR_SIZE, stdin);
}
//=====================================================================

int bisection_methot()
{
    float a, b, x;
    char func_str[MAX_EXPR_SIZE];
    printf("Bisection Methot\n");

    get_f_string(func_str);
    printf("Enter the range values\n");
    printf("Base Value: ");
    scanf("%f", &a);
    printf("Top Value: ");
    scanf("%f", &b);

    if (get_sign(f(a, func_str), f(b, func_str)) > 0)
    {
        printf("f(%.2f)*f(%.2f) = %d\n", a, b, get_sign(f(a, func_str), f(b, func_str)));
        printf("f(%.2f) = %f,f(%.2f) = %.2f\n", a, f(a, func_str), b, f(b, func_str));
        printf("There aren't any roots in (%.2f,%.2f) range!!!\n", a, b);
        return -1;
    }
    else if (f(a, func_str) == 0)
    {
        printf("%f is a root of f(x)\n", a);
        return 0;
    }
    else if (f(b, func_str) == 0)
    {
        printf("%f is a root of f(x)\n", b);
        return 0;
    }
    else
    {
        x = bisection_root(a, b, 0, func_str);
        printf("The root is: %f", x);
        return 1;
    }
}

int get_sign(float a, float b)
{
    if (a * b < 0)
        return -1;
    else if (a * b > 0)
        return 1;
    else
        return 0;
}
int check_err(int a, int b, int n)
{
    printf("err: %f\n", fabsf(b - a) / pow(2, n));
    /* if (fabsf(b - a) / pow(2, n) < 0.00001)
         return -1;*/
    if (fabsf(b - a) < ERR)
        return -1;
    else
        return 0;
}

float bisection_root(float a, float b, int n, char *func_str)
{
    float x;
    int sign_ax, sign_bx, err_flag;
    x = (b + a) / 2;
    n++;
    sign_ax = get_sign(f(a, func_str), f(x, func_str));
    sign_bx = get_sign(f(x, func_str), f(b, func_str));
    err_flag = check_err(a, b, n);

    printf("iteration: %d\n", n);
    printf("(a,x,b)\n");
    printf("(%f,%f,%f)\n", a, x, b);
    printf("(ax,xb)\n");
    printf("(%d,%d)\n", sign_ax, sign_bx);
    printf("========================\n", n);

    if (err_flag == -1 || (sign_ax == 0 && sign_bx == 0))
        return x;

    else if (sign_bx < 0)
        bisection_root(x, b, n, func_str);
    else if (sign_ax < 0)
        bisection_root(a, x, n, func_str);
}

int regula_falsi_methot()
{
    float a, b, x;
    char func_str[MAX_EXPR_SIZE];
    printf("Regula-Falsi Methot\n");

    get_f_string(func_str);
    printf("Enter the range values\n");
    printf("Base Value: ");
    scanf("%f", &a);
    printf("Top Value: ");
    scanf("%f", &b);

    if (get_sign(f(a, func_str), f(b, func_str)) > 0)
    {
        printf("f(%.2f)*f(%.2f) = %d\n", a, b, get_sign(f(a, func_str), f(b, func_str)));
        printf("f(%.2f) = %f,f(%.2f) = %.2f\n", a, f(a, func_str), b, f(b, func_str));
        printf("There aren't any roots in (%.2f,%.2f) range!!!\n", a, b);
        return -1;
    }
    else if (f(a, func_str) == 0)
    {
        printf("%f is a root of f(x)\n", a);
        return 0;
    }
    else if (f(b, func_str) == 0)
    {
        printf("%f is a root of f(x)\n", b);
        return 0;
    }
    else
    {
        printf("regula falsi root- a:%f,b:%f\n", a, b);
        x = regula_falsi_root(a, b, 0, func_str);
        printf("========================\n");
        printf("The root is: %f\n", x);
        printf("========================\n");

        return 1;
    }
}

float regula_falsi_root(float a, float b, int n, char *func_str)
{
    float x;
    int sign_ax, sign_bx, err_flag;
    x = (a * f(b, func_str) - b * f(a, func_str)) / (f(b, func_str) - f(a, func_str));

    n++;
    sign_ax = get_sign(f(a, func_str), f(x, func_str));
    sign_bx = get_sign(f(x, func_str), f(b, func_str));

    err_flag = check_err(a, b, n);
    printf("========================\n");

    printf("iteration: %d\n", n);
    printf("(a,x,b)\n");
    printf("(%f,%f,%f)\n", a, x, b);
    printf("(ax,xb)\n");
    printf("(%d,%d)\n", sign_ax, sign_bx);

    if (err_flag == -1 || (sign_ax == 0 && sign_bx == 0))
        return x;

    else if (sign_ax < 0)
        regula_falsi_root(a, x, n, func_str);
    else if (sign_bx < 0)
        regula_falsi_root(x, b, n, func_str);
}

int newton_rapshon_methot()
{
    float x, prev_x;
    char func_str[MAX_EXPR_SIZE];
    int iteration = 0;
    printf("Newton Rapshon Methot\n");

    get_f_string(func_str);
    printf("Enter the first value:");
    scanf("%f", &x);
    do
    {
        prev_x = x;
        x = newton_rapshon_iteration(prev_x, func_str);
        iteration++;
        printf("%d.iteration||x:%f\terr:%f\n", iteration, x, fabsf(x - prev_x));
    } while (fabsf(x - prev_x) > ERR);
    printf("Root:%f", x);
}

float newton_rapshon_iteration(float x, char *func_str)
{
    return (x - (f(x, func_str) / df(f, x, func_str)));
}

float df(float (*f)(float, char *), float x, char *func_str)
{
    return (((f(x + H, func_str) - f(x - H, func_str)) / (2 * H)));
}

int reverse_matrix()
{
    int N;
    float **matrix, **inverse;
    printf("Reverso of Matrix\n");

    printf("Enter the dimensions of the matrix (N): ");
    scanf("%d", &N);

    matrix = determine_matrix(N);

    printf("Original Matrix:\n\n");

    print_matrix(matrix, N);

    printf("Reversing Matrix...\n\n");

    inverse = inverse_matrix(matrix, N);

    printf("Inverse Matrix:\n");
    print_matrix(inverse, N);

    free(matrix);
    free(inverse);
}

float **determine_matrix(int N)
{
    float **matrix;
    int i, j;
    matrix = (float **)calloc(N, sizeof(float *));
    for (i = 0; i < N; i++)
        matrix[i] = (float *)calloc(N, sizeof(float));

    printf("Enter the matrix\n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("(%d,%d):", i, j);
            scanf("%f", &matrix[i][j]);
        }
    }
    return matrix;
}

float **inverse_matrix(float **matrix, int N)
{
    int i, j, k;
    float ratio;
    float **inverse;

    inverse = (float **)calloc(N, sizeof(float *));
    for (i = 0; i < N; i++)
        inverse[i] = (float *)calloc(N, sizeof(float));
    printf("Memory Allocated.");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (i == j)
                inverse[i][j] = 1;
            else
                inverse[i][j] = 0;
        }
    }
    for (i = 0; i < N; i++)
    {
        if (matrix[i][i] == 0.0)
        {
            printf("Mathematical Error!");
            return NULL;
        }
        for (j = 0; j < N; j++)
        {
            if (i != j)
            {
                ratio = matrix[j][i] / matrix[i][i];
                for (k = 0; k < N; k++)
                {
                    matrix[j][k] -= ratio * matrix[i][k];
                    inverse[j][k] -= ratio * inverse[i][k];

                    printf("\nMatrix:\n");
                    print_matrix(matrix, N);
                    printf("Inverse:\n");
                    print_matrix(inverse, N);
                    printf("\n\n");
                }
            }
        }
    }
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            inverse[i][j] /= matrix[i][i];
        }
    }

    return inverse;
}

void print_matrix(float **matrix, int N)
{
    int i, j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int gauss_elimination_methot()
{
    int N;
    float **matrix;
    printf("Gauss Elimination Methot\n");

    printf("Enter the dimensions of the matrix (N): ");
    scanf("%d", &N);

    matrix = determine_matrix(N);

    printf("Original Matrix");
    print_matrix(matrix, N);

    matris_elimination(matrix, N);
    print_matrix(matrix, N);
}

void matris_elimination(float **matrix, int n)
{
    for (int i = 0; i < n; i++)
    {

        if (matrix[i][i] == 0.0)
        {
            return;
        }

        for (int j = i + 1; j < n; j++)
        {
            float ratio = matrix[j][i] / matrix[i][i];
            for (int k = i; k <= n; k++)
            {
                matrix[j][k] -= ratio * matrix[i][k];
            }
        }
    }

    float *x;
    x = (float *)calloc(n, sizeof(float));

    for (int i = n - 1; i >= 0; i--)
    {
        x[i] = matrix[i][n];
        for (int j = i + 1; j < n; j++)
        {
            x[i] -= matrix[i][j] * x[j];
        }
        x[i] = x[i] / matrix[i][i];
    }

    printf("Eliminated Matrix:\n");
    for (int i = 0; i < n; i++)
    {
        printf("x[%d] = %8.4f\n", i, x[i]);
    }
}

int gauss_seidal_methot()
{
    int N, i, j, iteration_flag, var_flag;
    float **matrix_gauss_seidal;
    float *matrix_res, *variable_matrix, *variable_matrix_prv;
    printf("Gauss Seidal Methot\n");

    printf("Enter the dimensions of the matrix (N): ");
    scanf("%d", &N);

    matrix_gauss_seidal = determine_matrix(N);
    matrix_res = (float *)calloc(N, sizeof(float));
    variable_matrix = (float *)calloc(N, sizeof(float));

    printf("Original Matrix\n");
    print_matrix(matrix_gauss_seidal, N);
    printf("Enter the [C] matrix\n");
    for (i = 0; i < N; i++)
    {
        printf("(0,%d):", i);
        scanf("%f", &matrix_res[i]);
    }
    for (i = 0; i < N; i++)
        variable_matrix[i] = GAUSS_SEIDAL_START_ROOT;
    int a = 0;
    /*printf("Variables:\n");
    for (i = 0; i < N; i++)
    {
        printf("v%d: %f\n", i, variable_matrix[i]);
    }*/
    gauss_seidal_iteration(matrix_gauss_seidal, variable_matrix, matrix_res, N);
}

void gauss_seidal_iteration(float **matrix, float *variable, float *result, int N)
{
    int i, j, k, a, err_flag = 1, iteration = 0;
    float prev_variable[N];
    for (i = 0; i < N; i++)
    {
        prev_variable[i] = variable[i] + 2 * ERR;
    }
    while (err_flag != 0)
    {
        for (i = 0; i < N; i++)
        {
            float sum = result[i];
            for (j = 0; j < N; j++)
            {
                if (j != i)
                {
                    sum -= matrix[i][j] * variable[j];
                }
            }
            prev_variable[i] = variable[i];
            variable[i] = sum / matrix[i][i];
        }
        printf("\n%d.iteration\n", iteration);
        err_flag = 0;
        for (a = 0; a < N; a++)
        {
            printf("v%d: %f\t||\t", a, variable[a]);
            printf("err:%f\n", fabsf(variable[a] - prev_variable[a]));
            if (fabsf(variable[a] - prev_variable[a]) > ERR)
            {
                err_flag = 1;
            }
        }
        iteration++;
    }
}

int numeric_dervative()
{
    int selection;
    float h, x, res;
    char func_str[MAX_EXPR_SIZE];
    printf("Numeric Dervative\n");

    get_f_string(func_str);
    printf("Please choose the way:\n");
    printf("1-ileri fark\n2-geri fark\n3-merkezi fark\nSelection: ");
    scanf("%d", &selection);
    printf("Enter the x (6D):");
    scanf("%f", &x);
    printf("Enter the H (6D):");
    scanf("%f", &h);

    switch (selection)
    {
    case 1:
        res = df_i(f, x, h, func_str);
        break;
    case 2:
        res = df_g(f, x, h, func_str);
        break;
    case 3:
        res = df_m(f, x, h, func_str);
        break;
    default:
        break;
    }
    printf("Result of numeric dervative is (x):%f ", res);
}

float df_i(float (*f)(float, char *), float x, float h, char *func_str) { return ((f(x + h, func_str) - f(x, func_str)) / h); }

float df_g(float (*f)(float, char *), float x, float h, char *func_str) { return ((f(x, func_str) - f(x - h, func_str)) / h); }

float df_m(float (*f)(float, char *), float x, float h, char *func_str) { return ((f(x + h, func_str) - f(x - h, func_str)) / (2 * h)); }

int simpson_methot()
{
    float a, b, sum, step_val, i;
    int n, idx = 0;
    char func_str[MAX_EXPR_SIZE];
    printf("Trapez Methot\n");

    get_f_string(func_str);

    printf("Enter the range values(a,b):\n");
    scanf("%f", &a);
    scanf("%f", &b);
    printf("Enter the number of steps(n):");
    scanf("%d", &n);
    step_val = (b - a) / n;
    printf("\nTrapez Methot Integral:\n");
    printf("Range [%.2f,%.2f], n = %d, step_val= %f\n", a, b, n, step_val);
    printf("   x\t||\tf(x)\n", i, f(i, func_str));

    for (i = a; i <= b; i += step_val)
    {
        printf("f(x%.2d) = %f\t| x%d = %f", idx, f(i, func_str), idx, i);

        if (idx % 2 != 0 && idx != 0 && idx != n - 1)
        {
            sum += 4 * f(i, func_str);
            printf(" *4\n");
        }
        else if (idx % 2 == 0 && idx != 0 && idx != n - 1)
        {
            sum += 2 * f(i, func_str);
            printf(" *2\n");
        }
        else
            printf("\n");
        idx++;
    }
    sum += (f(a, func_str) + f(b, func_str));
    sum *= step_val / 3;

    printf("Integral of f(x) at range [%.2f,%.2f] = %f", a, b, sum);
}

int trapez_methot()
{
    float a, b, sum, step_val, i;
    int n, idx = 0;
    char func_str[MAX_EXPR_SIZE];
    printf("Trapez Methot\n");

    get_f_string(func_str);

    printf("Enter the range values(a,b):\n");
    scanf("%f", &a);
    scanf("%f", &b);
    printf("Enter the number of steps(n):");
    scanf("%d", &n);
    step_val = (b - a) / n;
    printf("\nTrapez Methot Integral:\n");
    printf("Range [%.2f,%.2f], n = %d, step_val= %f\n", a, b, n, step_val);
    // printf(" x\t||\tf(x)\n", i, f(i));
    printf("\n", i, f(i, func_str));

    for (i = a; i <= b; i += step_val)
    {
        printf("f(x%.2d) = %f\t| x%d = %f\n", idx, f(i, func_str), idx, i);
        sum += f(i, func_str);
        idx++;
    }
    sum -= (f(a, func_str) + f(b, func_str)) / 2;
    sum *= step_val;

    printf("Integral of f(x) at range [%.2f,%.2f] = %f", a, b, sum);
}

int gregory_newton_methot()
{
    int i, j, n, step;
    float **data;
    float *coefficient;
    float x, res;
    printf("Gregory Newton Interpolation Method\n");
    printf("Enter number of values: ");
    scanf("%d", &n);
    printf("Enter maximum step number: ");
    scanf("%d", &step);

    data = (float **)calloc(n, sizeof(float *));
    for (i = 0; i < n; i++)
        data[i] = (float *)calloc(step, sizeof(float));

    coefficient = (float *)calloc(step, sizeof(float));

    for (i = 0; i < n; i++)
    {
        printf("f(%d): ", i);
        scanf("%f", &data[i][0]);
    }
    printf("Data get\n");

    for (j = 1; j < step; j++)
    {
        for (i = 0; i < n - j; i++)
        {
            data[i][j] = data[i + 1][j - 1] - data[i][j - 1];
        }
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j <= step && j < n - i; j++)
        {
            printf("%.2f ", data[i][j]);
        }
        printf("\n");
    }

    printf("\nFactor array: ");
    for (i = 0; i < step; i++)
    {
        coefficient[i] = data[0][i];
        printf("%f ", coefficient[i]);
    }
    printf("\n\n");

    print_f_gnm(coefficient, step);

    printf("\nEnter the x for f(x):");
    scanf("%f", &x);
    printf("x: %f\n", x);
    res = f_gnm(coefficient, step, x);
    printf("Result: %f", res);

    for (i = 0; i < n; i++)
        free(data[i]);
    free(data);
    free(coefficient);

    return 0;
}

float f_gnm(float *coefficient, int step, float x)
{
    int i, j;
    float sum = 0, mult = 0, fak = 0;
    for (i = 0; i < step; i++)
    {
        mult = 1;
        fak = 1;
        for (j = 0; j < i; j++)
        {
            mult *= (x - j);
            if (j != 0)
                fak *= j;
            printf("(%d,%d)-%f,%f,%f\n", i, j, mult, i);
        }
        if (i != 0)
            sum += (coefficient[i] * mult) / i;
        else if (i == 0)
            sum += (coefficient[i] * mult);
        printf("sum: %f = %f*%f/%f\n", sum, coefficient[i], mult, i);

        // printf("%f\n", sum);
    }
    return sum;
}

void print_f_gnm(float *coefficient, int step)
{
    int i, j;
    float sum, mult, fak;
    printf("The function:\n");
    for (i = 0; i < step; i++)
    {
        printf("%.2f ", coefficient[i], i, i);
        for (j = 0; j < i; j++)
        {
            if (j == 0)
                printf("x");
            else
                printf("(x-%d)", j);
        }
        printf("/%d!", i);
        if (i != step - 1)
            printf(" + ");
    }
}

//============ PARSING FUNC =============//

float parse_expression(const char *expr)
{
    // printf("Parsing expression: %s\n", expr);
    float result = parse_term(&expr);
    // printf("Initial term result: %f\n", result);
    while (*expr == '+' || *expr == '-')
    {
        char op = *expr++;
        float value = parse_term(&expr);
        if (op == '+')
        {
            result += value;
        }
        else
        {
            result -= value;
        }
        // printf("Updated expression result with '%c': %f\n", op, result);
    }
    return result;
}

float parse_term(const char **expr)
{
    // printf("Parsing term: %s\n", *expr);
    float result = parse_factor(expr);
    // printf("Initial factor result: %f\n", result);
    while (**expr == '*' || **expr == '/' || **expr == '^')
    {
        char op = *(*expr)++;
        float value = parse_factor(expr);
        if (op == '*')
        {
            result *= value;
        }
        else if (op == '/')
        {
            result /= value;
        }
        else if (op == '^')
        {
            result = pow(result, value);
        }
        // printf("Updated term result with '%c': %f\n", op, result);
    }
    return result;
}

float parse_factor(const char **expr)
{
    // printf("Parsing factor: %s\n", *expr);
    if (**expr == '(')
    {
        (*expr)++;
        float result = parse_expression(*expr);
        if (**expr == ')')
        {
            (*expr)++;
        }
        // printf("Parsed parenthesized expression: %f\n", result);
        return result;
    }
    else if (isalpha(**expr))
    {
        char func[10];
        int i = 0;
        while (isalpha(**expr))
        {
            func[i++] = *(*expr)++;
        }
        func[i] = '\0';

        if (strcmp(func, "pi") == 0)
        {
            return M_PI;
        }
        if (strcmp(func, "e") == 0)
        {
            return M_E;
        }

        if (**expr == '_')
        {
            return parse_log_base(expr);
        }
        if (**expr == '(')
        {
            (*expr)++;
            float arg = parse_expression(*expr);
            if (**expr == ')')
            {
                (*expr)++;
            }
            float result = evaluate_function(func, arg);
            // printf("Parsed function '%s' with arg %f: %f\n", func, arg, result);
            return result;
        }
    }
    else
    {
        float result = parse_number(expr);
        // printf("Parsed number: %f\n", result);
        return result;
    }
    return 0.0;
}

float parse_number(const char **expr)
{
    float result = strtod(*expr, (char **)expr);
    // printf("Parsed number: %f\n", result);
    return result;
}

float evaluate_function(const char *func, float arg)
{
    if (strcmp(func, "sin") == 0)
    {
        return sin(arg);
    }
    else if (strcmp(func, "cos") == 0)
    {
        return cos(arg);
    }
    else if (strcmp(func, "tan") == 0)
    {
        return tan(arg);
    }
    else if (strcmp(func, "asin") == 0)
    {
        return asin(arg);
    }
    else if (strcmp(func, "acos") == 0)
    {
        return acos(arg);
    }
    else if (strcmp(func, "atan") == 0)
    {
        return atan(arg);
    }
    else if (strcmp(func, "log") == 0)
    {
        return log(arg);
    }
    else if (strcmp(func, "exp") == 0)
    {
        return exp(arg);
    }
    else if (strcmp(func, "sqrt") == 0)
    {
        return sqrt(arg);
    }

    return 0.0;
}

float parse_log_base(const char **expr)
{
    (*expr)++;
    float base = parse_number(expr);
    if (**expr == '(')
    {
        (*expr)++;
        float arg = parse_expression(*expr);
        if (**expr == ')')
        {
            (*expr)++;
        }
        float result = log(arg) / log(base);

        return result;
    }
    return 0.0;
}

void replace_x_with_input(char *expr, float input)
{
    char buffer[MAX_EXPR_SIZE];
    char input_str[20];
    snprintf(input_str, sizeof(input_str), "%lf", input);

    int i, j = 0;
    for (i = 0; expr[i] != '\0'; i++)
    {
        if (expr[i] == 'x')
        {
            strcpy(&buffer[j], input_str);
            j += strlen(input_str);
        }
        else
        {
            buffer[j++] = expr[i];
        }
    }
    buffer[j] = '\0';
    strcpy(expr, buffer);
}