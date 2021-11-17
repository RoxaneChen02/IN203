# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>
#include <tgmath.h>

int puissancede2_inf(int value) {
	int retour = (int)floor(log(value) / log(2));
	return retour;
}

int puissancede2_sup(int value) {
	if (value == 0) { return 0; }
	else {
		int retour = (int)ceil(log(value) / log(2));
		return retour;
	}
}

int puissance2(int value) {
	if (value == 0) { return 1; }
	return 2*puissance2(value - 1);
}

int main( int nargs, char* argv[] )
{
	// On initialise le contexte MPI qui va s'occuper :
	//    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
	//       et assurer la cohésion de l'ensemble des processus créés par MPI;
	//    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
	//       le communicateur COMM_WORLD
	//    3. etc...
	MPI_Init( &nargs, &argv );
	// Pour des raisons de portabilité qui débordent largement du cadre
	// de ce cours, on préfère toujours cloner le communicateur global
	// MPI_COMM_WORLD qui gère l'ensemble des processus lancés par MPI.
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	// On interroge le communicateur global pour connaître le nombre de processus
	// qui ont été lancés par l'utilisateur :
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	MPI_Status status;
	int tag = 0;
	// On interroge le communicateur global pour connaître l'identifiant qui
	// m'a été attribué ( en tant que processus ). Cet identifiant est compris
	// entre 0 et nbp-1 ( nbp étant le nombre de processus qui ont été lancés par
	// l'utilisateur )
	int rank;
	int dimension = puissancede2_inf(nbp);

	MPI_Comm_rank(globComm, &rank);
	// Création d'un fichier pour ma propre sortie en écriture :
	std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );

	int jeton = 2;
	if (rank == 0) {
		int i = puissancede2_sup(rank);
		output << "I'm the processus " << rank << " on " << nbp << " processes." << std::endl;
		while (rank + puissance2(i) < nbp) {

			MPI_Send(&jeton, 1, MPI_INT, rank + puissance2(i), 0, MPI_COMM_WORLD);

			output << "J'envoie à " << rank + puissance2(i) << std::endl;
			i=i+1;

			
			
		}

		
		
		
	}
	//on se place dans le processus rank 
	else {
		
		int recu;
		int rang_recu = rank - puissance2(puissancede2_inf(rank));
		output << "I'm the processus " << rank << " on " << nbp << " processes." << std::endl;
		output << "je reçois de " << rang_recu << std::endl;

		MPI_Recv(&recu, 1, MPI_INT, rang_recu, tag, MPI_COMM_WORLD, &status);
		int i = puissancede2_sup(rank);
		while (rank + puissance2(i) < nbp) {
			MPI_Send(&recu, 1, MPI_INT, rank + puissance2(i), 0, MPI_COMM_WORLD);
			
			output << "J'envoie à " << rank + puissance2(i) << std::endl;
			++i;
		}

		
	}
	output.close();
	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}
