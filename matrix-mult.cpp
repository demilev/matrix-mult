#include <iostream>
#include <fstream>

#include <thread>
#include <mutex>
#include <utmpx.h>

#include <stdexcept>

#include <string>
#include <chrono>

#include <boost/program_options.hpp>


using std::cout;
using std::endl;
using std::thread;
using std::cerr;
using std::string;
using std::invalid_argument;
using std::ifstream;
using std::ofstream;
using std::mutex;
using std::lock_guard;

namespace po = boost::program_options;

mutex iomutex;

void classic_matrix_mult(double** A, double** B, double** C, int nFrom, int n, int mFrom, int m, int kFrom, int k) 
{   
    {
        lock_guard<mutex> iolock(iomutex);
        std::cout << "Thread on CPU " << sched_getcpu() << "\n";
    }

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


void read_matrices(const string& input_file, double** &A, double** &B, int& n, int& m, int& k)
{
    ifstream in(input_file);
    if(! (in >> n) || ! (in >> m) || ! (in >> k))
    {
        throw std::invalid_argument("invalid file format");
    }
    
    A = new double*[n];
    B = new double*[m];

    for (int i = 0; i < n; i++)
    {   
        A[i] = new double[m];
        for (int j = 0; j < m; j++)
        {   
            if (! (in >> A[i][j]))
            {
                throw std::invalid_argument("invalid file format");
            }
        }
        string endline;
        getline(in, endline);
        if (!endline.empty())
        {
            throw std::invalid_argument("invalid file format");
        }
    }


    for (int i = 0; i < m; i++)
    {   
        B[i] = new double[k];
        for (int j = 0; j < k; j++)
        {
            if (! (in >> B[i][j]))
            {
                throw std::invalid_argument("invalid file format");
            }
        }
        
        string endline;
        getline(in, endline);
        if (!endline.empty())
        {
            throw std::invalid_argument("invalid file format");
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

void print_args(int n, int m, int k, string input, string output, bool quite, int t)
{
    cout << "N = " << n << endl
         << "M = " << m << endl
         << "K = " << k << endl
         << "Input = " << input << endl
         << "Output = " << output << endl
         << "Quite = " << quite << endl
         << "T = " << t << endl;
}


int main(int argc, char **argv) 
{   
    int t = 1, n = 0, m = 0, k = 0;
    bool quite = false, generate_random = false;
    string output_file = "", input_file = "";

    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "prints help message")
        ("nrows,n", po::value<int>(), "number of rows of first matrix")
        ("mcols,m", po::value<int>(), "number of columns of first matrix and rows of second matrix")
        ("kcols,k", po::value<int>(), "number of columns of second matrix")
        ("input,i", po::value<string>(), "input file with both matrices; this option has bigger priority than -n,-m,-k")
        ("output,o", po::value<string>(), "output file for the result matix")
        ("tasks,t", po::value<int>()->default_value(1), "number of threads to be used")
        ("quiet,q", po::bool_switch()->default_value(false), "quiet mode");
    
    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (po::error &e)
    {
        cerr << "ERROR: " << e.what() << endl << endl;
        cout << "Matrix multiplication program options:" << endl << desc << endl;
        return 0;
    }

    if (vm.count("help"))
    {
        cout << "Matrix multiplication program options:" << endl << desc << endl;
        return 0;
    }
    
    if (!vm.count("output"))
    {
        cout << "You must specify the output matrix file." << endl 
             << "Use -h [ --help ] to see information about usage." << endl;
        return 0;
    }

    if (!vm.count("input"))
    {
        if (!vm.count("nrows") || !vm.count("mcols") || !vm.count("kcols"))
        {
            cout << "You must specify matrices sizes or give input file for them as an argument." << endl 
                 << "Use -h [ --help ] to see information about usage." << endl;
            return 0;
        }
        n = vm["nrows"].as<int>();
        m = vm["mcols"].as<int>();
        k = vm["kcols"].as<int>();
        generate_random = true;
    }
    else 
    {
        generate_random = false;
        input_file = vm["input"].as<string>();
        ifstream in(input_file);
        if (!in.good())
        {
            cout << "File " << input_file << " does not exists." << endl;
            return 0;
        }   
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    t = vm["tasks"].as<int>();
    output_file = vm["output"].as<string>();
    if (vm.count("quiet"))
    {
        quite = true;
    }

    
    double **A;
    double **B;
    double **C;

    if (generate_random)
    {
        A = new double*[n]; 
        B = new double*[m]; 
        C = new double*[n];
        
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
    }
    else
    {
        try
        {
            read_matrices(input_file, A, B, n, m, k);
        }
        catch (invalid_argument&)
        {
            cout << "Invalid input file." << endl
                 << "See example_input.txt for an example." << endl;
            return 0;
        }
    }

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
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    cout << "time: " << elapsed.count() << endl;
}
