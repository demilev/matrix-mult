#include <iostream>
#include <fstream>

#include <pthread.h>
#include <thread>
#include <mutex>
#include <utmpx.h>

#include <stdexcept>

#include <algorithm>
#include <string>
#include <chrono>
 
#include <ctime> 



#include <boost/program_options.hpp>


using std::cout;
using std::endl;
using std::min;
using std::thread;
using std::cerr;
using std::string;
using std::invalid_argument;
using std::ifstream;
using std::ofstream;
using std::mutex;
using std::lock_guard;
using std::srand;
using std::time;

namespace po = boost::program_options;

mutex iomutex;

void matrix_mult(double** A, double** B, double** C, 
                 int nFrom, int n, int mFrom, int m, int kFrom, int k, 
                 bool quiet, int thread_number) 
{   
    if (!quiet)
    {
        lock_guard<mutex> iolock(iomutex);
        cout << "Thread-" << thread_number << " started.\n";
    }
    
    auto start = std::chrono::high_resolution_clock::now();

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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    if (!quiet)
    {
        lock_guard<mutex> iolock(iomutex);
        cout << "Thread-" << thread_number << " stopped.\n";
        cout << "Thread-" << thread_number << " execution time was (millis): " << millis << ".\n";
    }
}

void generate_random_matrix(double** A, int n, int m) 
{   

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++) 
        {
            A[i][j] = (double)rand() / RAND_MAX;
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


void persist_matrix(double** A, int n, int m, ofstream& out)
{   
    out << n << " " << m << endl;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            out << A[i][j] << " ";
        }
        out << endl;
    }
}

int main(int argc, char **argv) 
{   
    int t = 1, n = 0, m = 0, k = 0;
    bool quiet = false, generate_random = false;
    string output_file, input_file;

    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "prints help message")
        ("nrows,n", po::value<int>(), "number of rows of first matrix")
        ("mcols,m", po::value<int>(), "number of columns of first matrix and rows of second matrix")
        ("kcols,k", po::value<int>(), "number of columns of second matrix")
        ("input,i", po::value<string>(), "input file with both matrices; this option has bigger priority than -n,-m,-k")
        ("output,o", po::value<string>()->default_value(""), "output file for the result matix")
        ("tasks,t", po::value<int>()->default_value(1), "number of threads to be used")
        ("quiet,q", po::bool_switch()->default_value(false), "quiet mode");
    
    po::variables_map vm;

    // parse command line arguments
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (po::error &e)
    {
        cerr << "ERROR: " << e.what() << endl << endl;
        cout << "Matrix multiplication program options:" << endl << desc << endl;
        return 0;
    }

    /*
     * if command line argument -h(--help) is set 
     * then just print help text and exit the rpogram 
     */
    if (vm.count("help"))
    {
        cout << "Matrix multiplication program options:" << endl << desc << endl;
        return 0;
    }
    
    // argument -i(--input) has higher priority than -n, -m and -k
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

    t = vm["tasks"].as<int>();
    output_file = vm["output"].as<string>();
    quiet = vm["quiet"].as<bool>();
    
    double **A;
    double **B;
    double **C;

    // generate matrixes or read them from the input file
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
        
        srand(time(NULL));
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
    
    // start the timer
    auto start = std::chrono::high_resolution_clock::now();

    /*
     * because of the way the task is splitted into
     * subtasks, we have to make sure that the 
     * number of threads that we are going to start
     * is no more than the rows of matrix A
     */
    int max_threads = std::min(n, t);
    thread threads[max_threads];
    for (int i = 0; i < max_threads; i++)
    {   
        int nFrom = (i * n) / max_threads;
        int nTo = ((i + 1) * n) / max_threads;
        // start each thread to calculate its sector of the result matrix
        threads[i] = thread(&matrix_mult, A, B, C, nFrom , nTo, 0, m, 0, k, quiet, i + 1);
	}

    // join the threads to wait there execution
    for (int i = 0; i < max_threads; i++)
    {
        threads[i].join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    // output the result matrix if -o(--output) is set
    if (!output_file.empty())
    {   
        ofstream out(output_file);
        persist_matrix(C, n, k, out);
    }
    
    // print total execution time
    cout << "Threads used in current run: " << max_threads << ".\n";
    cout << "Total execution time for current run (millis): " << millis << ".\n";
}
