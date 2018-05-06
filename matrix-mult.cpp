#include <iostream>
#include <thread>
#include <chrono>

void classic_matrix_mult(double** A, double** B, double** C, int nFrom, int n, int mFrom, int m, int kFrom, int k) 
{
    for (int i = nFrom; i < n; i++) 
    {
        for (int j = kFrom; j < k; j++) 
        {
            double prod = 0;
            for (int t = mFrom; t < m; t++) 
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
// try with non sharing matrices
// add boost parameters parsing
int main() 
{   
    auto start = std::chrono::high_resolution_clock::now();
    int t = 4;
    int n = 2048, m = 2048, k = 2048;
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
    auto gen = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> genElapsed = gen - start;
    std::cout << "time: " << genElapsed.count() << std::endl;

    //print_matrix(A, n, m);
    //print_matrix(B, m, k);
    //classic_matrix_mult(A, B, C, 0, 2, 0, m, 0, k);
    std::thread threads[t];
    for (int i = 0; i < t; i++)
    {   
        int nFrom = (i * n) / t;
        int nTo = ((i + 1) * n) / t;
        std::cout << nFrom << " " << nTo << std::endl;
        threads[i] = std::thread(&classic_matrix_mult, A, B, C, nFrom , nTo, 0, m, 0, k);
    }

    for (int i = 0; i < t; i++)
    {
        threads[i].join();
    }
    //print_matrix(C, n, k);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "time: " << elapsed.count() << std::endl;
}