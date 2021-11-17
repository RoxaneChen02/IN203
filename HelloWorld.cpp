# include <iostream>
# include <cstdlib>
# include <mpi.h>
# include <sstream>
# include <fstream>


# include <sstream>
# include <string>
# include <fstream>

# include <iomanip>


int main(int nargs, char* argv[])
{
    MPI_Init(&nargs, &argv);
    int numero_du_processus, nombre_de_processus;

    MPI_Comm globComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &globComm);



    MPI_Comm_rank(MPI_COMM_WORLD,
        &numero_du_processus);

    MPI_Comm_size(MPI_COMM_WORLD,
        &nombre_de_processus);

    std::stringstream HelloWorld;
    HelloWorld << "Output" << std::setfill('0') << std::setw(5) << numero_du_processus << ".txt";
    std::ofstream output(HelloWorld.str().c_str());

    output << "I'm the processus " << numero_du_processus << " on " << nombre_de_processus << " processes." << std::endl;
    output << "Hello world from "
        << numero_du_processus << " in "
        << nombre_de_processus << " executed"
        << std::endl;
    output.close();

    MPI_Finalize();
    return EXIT_SUCCESS;
}
