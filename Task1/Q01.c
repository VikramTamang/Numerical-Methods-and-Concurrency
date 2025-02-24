#include <stdio.h>

int find(const char *file_n, double *x, double *y, double *xx, double *xy, int *n)
{
    FILE *file = fopen(file_n, "r");
    if (file == NULL)
    {
        printf("Error: Cannot access file %s\n", file_n);
        return 0; // Indicate failure
    }

    double x_value, y_value;
    while (fscanf(file, "%lf,%lf", &x_value, &y_value) == 2)
    {
        *x += x_value;
        *y += y_value;
        *xy += x_value * y_value;
        *xx += x_value * x_value;
        (*n)++;
    }

    fclose(file);
    return 1; // Indicate success
}

int main()
{
    double Sx = 0, Sy = 0, Sx2 = 0, Sxy = 0;
    int n = 0;

    const char *files[] = {"datasetLR1.txt", "datasetLR2.txt", "datasetLR3.txt", "datasetLR4.txt"};
    for (int i = 0; i < 4; i++)
    {
        if (!find(files[i], &Sx, &Sy, &Sx2, &Sxy, &n))
        {
            printf("Error reading data from file: %s\n", files[i]);
            return 1;
        }
    }

    if (n == 0)
    {
        printf("No valid data found in the files.\n");
        return 1;
    }

    double a, b, Sumx_sqr;
    Sumx_sqr = Sx * Sx;

    if (Sumx_sqr == 0 || n * Sx2 - Sumx_sqr == 0)
    {
        printf("Error: Division by zero in regression calculation. Check input data.\n");
        return 1;
    }

    a = ((Sy * Sx2) - (Sx * Sxy)) / ((n * Sx2) - Sumx_sqr);
    b = ((n * Sxy) - (Sx * Sy)) / ((n * Sx2) - Sumx_sqr);

    printf("The equation of the straight line is y = %.6lfx + %.6lf\n", b, a);

    double input;
    printf("Please enter the value of x to calculate y: ");
    while (scanf("%lf", &input) != 1)
    {
        printf("Invalid input. Please enter a numeric value: ");
        while (getchar() != '\n')
            ; // Clear the input buffer
    }

    double n_y = (b * input) + a;
    printf("Value of y is: %.6lf\n\n", n_y);

    return 0;
}