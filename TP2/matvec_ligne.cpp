// Produit matrice-vecteur
# include <cassert>
# include <vector>
# include <iostream>
#include<mpi.h>

// ---------------------------------------------------------------------
class Matrix : public std::vector<double>
{
public:
    Matrix (int dim);
    Matrix( int nrows, int ncols );
    Matrix( const Matrix& A ) = delete;
    Matrix( Matrix&& A ) = default;
    ~Matrix() = default;

    Matrix& operator = ( const Matrix& A ) = delete;
    Matrix& operator = ( Matrix&& A ) = default;
    
    double& operator () ( int i, int j ) {
        return m_arr_coefs[i + j*m_nrows];
    }
    double  operator () ( int i, int j ) const {
        return m_arr_coefs[i + j*m_nrows];
    }
    
    std::vector<double> operator * ( const std::vector<double>& u ) const;
    
    std::ostream& print( std::ostream& out ) const
    {
        const Matrix& A = *this;
        out << "[\n";
        for ( int i = 0; i < m_nrows; ++i ) {
            out << " [ ";
            for ( int j = 0; j < m_ncols; ++j ) {
                out << A(i,j) << " ";
            }
            out << " ]\n";
        }
        out << "]";
        return out;
    }
private:
    int m_nrows, m_ncols;
    std::vector<double> m_arr_coefs;
};
// ---------------------------------------------------------------------
inline std::ostream& 
operator << ( std::ostream& out, const Matrix& A )
{
    return A.print(out);
}
// ---------------------------------------------------------------------
inline std::ostream&
operator << ( std::ostream& out, const std::vector<double>& u )
{
    out << "[ ";
    for ( const auto& x : u )
        out << x << " ";
    out << " ]";
    return out;
}
// ---------------------------------------------------------------------
std::vector<double> 
Matrix::operator * ( const std::vector<double>& u ) const
{
    const Matrix& A = *this;
    assert( u.size() == unsigned(m_ncols) );
    std::vector<double> v(m_nrows, 0.);
    for ( int i = 0; i < m_nrows; ++i ) {
        for ( int j = 0; j < m_ncols; ++j ) {
            v[i] += A(i,j)*u[j];
        }            
    }
    return v;
}

// =====================================================================
Matrix::Matrix (int dim) : m_nrows(dim), m_ncols(dim),
                           m_arr_coefs(dim*dim)
{
    for ( int i = 0; i < dim; ++ i ) {
        for ( int j = 0; j < dim; ++j ) {
            (*this)(i,j) = (i+j)%dim;
        }
    }
}
// ---------------------------------------------------------------------
Matrix::Matrix( int nrows, int ncols ) : m_nrows(nrows), m_ncols(ncols),
                                         m_arr_coefs(nrows*ncols)
{
    int dim = (nrows > ncols ? nrows : ncols );
    for ( int i = 0; i < nrows; ++ i ) {
        for ( int j = 0; j < ncols; ++j ) {
            (*this)(i,j) = (i+j)%dim;
        }
    }    
}
// =====================================================================
int main( int nargs, char* argv[] )
{

    MPI_Init(&nargs, &argv);
    // Pour des raison pr�f�re toujours cloner le communicateur global
    // MPI_COMM_WORLD qui g�re l'ensemble des processus lanc�s par MPI.
    MPI_Comm globComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
    // On interroge le communicateur global pour conna�tre le nombre de processus
    // qui ont �t� lanc�s par l'utilisateur :
    int nbp;
    MPI_Comm_size(globComm, &nbp);
    // On interroge le communicateur global pour conna�tre l'identifiant qui
    // m'a �t� attribu� ( en tant que processus ). Cet identifiant est compris
    // entre 0 et nbp-1 ( nbp �tant le nombre de processus qui ont �t� lanc�s par
    // l'utilisateur )
    int rank;
    MPI_Comm_rank(globComm, &rank);
    int tag = 0;
    MPI_Status status;
    const int N = 6;
    Matrix A(N);
    //std::cout << "A : " << A << std::endl;
    std::vector<double> u(N);
    for (int i = 0; i < N; ++i) u[i] = i + 1;

    //std::cout << " u : " << u << std::endl;
    int Nloc = N / nbp;
    std::vector<double> w(N);
    if (N % nbp == 0) {
       
        std::vector<double> result(N); //le vecteur � renvoyer
        std::vector<double> w(N);
        for (int i = 0; i < N; ++i) { result[i] = 0; } //
        
        for (int i = 0; i < Nloc; ++i) {
            for (int j = 0; j < N; ++j) {
                result[i] = u[j+rank]* A(i, rank+j)+result[i];
                
            }
        }

       
        MPI_Allreduce(result.data(), w.data(), N, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        std::cout<<"w = "<<w<<std::endl;
        


        std::cout << "A : " << A << std::endl;

        std::cout << " u : " << u << std::endl;

        //std::cout << "on verifie si le resultat obtenu est bon \n";
        //std::cout << "le resultat obtenu avec calcul en parall�le : \n" << std::endl;
        //std::cout << "A.u = " << w << std::endl;

        //std::cout << "\n \n \n  le calcul en s�quentiel donne : \n";

        std::vector<double> v = A * u;
        std::cout << "A.u = " << v << std::endl;

    }

    
    else {
        printf("error la dimension N n'est pas divisible par le nombre de taches");
        MPI_Finalize();
        return EXIT_SUCCESS;
    }

    
    
    
    MPI_Finalize();    
   
   
    return EXIT_SUCCESS;
}
