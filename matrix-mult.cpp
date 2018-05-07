#include <iostream>
#include <thread>
#include <chrono>
#include <boost/program_options.hpp>

using std::cout;
using std::endl;
using std::thread;

namespace po = boost::program_options;

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
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
}

double** copy_matrix(double** M, int n, int m)
{  
    double** R = new double*[n];
    for (int i = 0; i < n; i++)
    {
        R[i] = new double[m];
        for (int j = 0; j < m; j++)
        {
            R[i][j] = M[i][j];
        }
    }
    return R;
}
// add boost parameters parsing
int main(int argc, char **argv) 
{   
    










    auto start = std::chrono::high_resolution_clock::now();
    int t = 2;
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
    
    //print_matrix(A, n, m);
    //print_matrix(B, m, k);
    //classic_matrix_mult(A, B, C, 0, 2, 0, m, 0, k);
    thread threads[t];
    for (int i = 0; i < t; i++)
    {   
        int nFrom = (i * n) / t;
        int nTo = ((i + 1) * n) / t;
        threads[i] = thread(&classic_matrix_mult, A, B, C, nFrom , nTo, 0, m, 0, k);
    }

    for (int i = 0; i < t; i++)
    {
        threads[i].join();
    }
    //print_matrix(C, n, k);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    cout << "time: " << elapsed.count() << endl;
}
