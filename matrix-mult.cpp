#include<iostream>

void classic_matrix_mult(double** A, double** B, double** C, int n, int m, int k) 
{
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < k; j++) 
        {
            double prod = 0;
            for (int t = 0; t < m; t++) 
            {
                prod += A[i][t] * B[t][j];
            }
            C[i][j] = prod;
        }
    }
}

void generate_random_matrix(double** A, int n, int m) 
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++) 
        {
            A[i][j] = rand() % 10;
        }
    }
}

void print_matrix(double** A, int n, int m)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            std::cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() 
{
    int n = 3, m = 3, k = 3;
    double **A = new double*[n]; 
    double **B = new double*[m]; 
    double **C = new double*[n];
    for (int i = 0; i < n; i++)
    {
        A[i] = new double[m];
        C[i] = new double[k];
    }

    for (int i = 0; i < m; i++)
    {
        B[i] = new double[k];
    }
    generate_random_matrix(A, n, m);
    generate_random_matrix(B, m, k);
    print_matrix(A, n, m);
    print_matrix(B, m, k);
    classic_matrix_mult(A, B, C, n, m, k);
    print_matrix(C, n, k);
}