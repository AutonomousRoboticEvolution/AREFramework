#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> 
#include <unistd.h>
#define WORKTAG     1
#define DIETAG     2

long SEED = 0;
char * REPOSITORY = NULL;
char * VREP_EXE = NULL;

int main(int argc, char ** argv) {
	int rank, size;
	char name[80];
	int length;
	MPI_Comm new_comm;

	if (argc < 4) {
		printf("Usage: %s [vrep_exe] [seed] [repository]\n", argv[0]);
		return 1;
	}
	
	VREP_EXE   = argv[1];
	SEED       = atol(argv[2]);
	REPOSITORY = argv[3];

	// printf("starting MPI\n");

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);


	// printf("[%d] vrep_exe = %s\n", rank, VREP_EXE);
	// printf("[%d] SEED = %d\n", rank, SEED);
	// printf("[%d] REPO = %s\n", rank, REPOSITORY);

  	MPI_Comm_split( MPI_COMM_WORLD, rank == 0, 0, &new_comm );

	if (rank == 0) {
		printf("Rank is zero thus initializing master\n");
		master_io( MPI_COMM_WORLD, new_comm );
	} else {
		slave_io( MPI_COMM_WORLD, new_comm );
	}

	MPI_Finalize();
}

/* This is the master */
int master_io( master_comm, comm )
MPI_Comm comm;
{
	int	i,j, size;
	char	buf[256];
	char	erClientArguments[512];
	const char* clientPath = "WP4_Ecosytem_Manager/Cplusplus_Evolution/ERClient/ERClient";

	MPI_Comm_size(MPI_COMM_WORLD,&size);
	sprintf( buf, "Hello?");
	printf("Master here\n");

	MPI_Status status;
	printf("Master is going to run vrep\n");
	for (i=1; i<size; i++) {
		MPI_Recv( buf, 256, MPI_CHAR, i, 0, master_comm, &status );
		fputs( buf, stdout );
	}

	// INIT BARRIER
	MPI_Barrier(MPI_COMM_WORLD);

	// arguments: repository, seed and amount CPU's
	sprintf(erClientArguments, "%s %s %d %d ", clientPath, REPOSITORY, SEED, size);
	system(erClientArguments);
	printf("Master ran VREP\n");

	// FINALIZE BARRIER
	MPI_Barrier(MPI_COMM_WORLD);

	for (i=1; i<size; i++) {
		MPI_Recv( buf, 256, MPI_CHAR, i, 0, master_comm, &status );
		fputs( buf, stdout );
	}
}

/* This is the slave */
int slave_io( master_comm, comm )
MPI_Comm comm;
{
	char 	buf[256];
	int 	rank, size, length;
	char 	name[80];
	MPI_Status	status;

	MPI_Comm_rank( comm, &rank );
	MPI_Comm_size( comm, &size );
	sprintf( buf, "Hello from slave %d\n", rank );
	MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm );
	
	// INIT BARRIER
	MPI_Barrier(MPI_COMM_WORLD);

	printf("Hello from slave %d\n", rank);
	MPI_Get_processor_name(name,&length);
	char erClientArguments[512];
	//sprintf(erClientArguments, "xvfb-run sh vrep.sh -h -g%d -g0 -g/users/f/v/fveenstr/scratch/VREP/V-REP_PRO_V3_4_0_64_Linux/plantResults",rank);
		
	// parameters
	// [1] rank
	// [2] type of run
	//       - 1 SERVER
	//       - 9 RECALL
	// [3] repository
	sprintf(erClientArguments, "%s -h -g%d -g1 -g%s -gREMOTEAPISERVERSERVICE_%d_TRUE_TRUE", VREP_EXE, rank, REPOSITORY, rank + 104000);
	// sprintf(erClientArguments, "%s -h -g%d -g1 -g%s -gREMOTEAPISERVERSERVICE_%d_TRUE_TRUE", VREP_EXE, 0, REPOSITORY, rank + 104000);
	printf("Hello MPI: processor %d of %d on %s\n", rank,size,name);

	char cwd[1024];
   	if (getcwd(cwd, sizeof(cwd)) != NULL)
		fprintf(stdout, "Current working dir: %s\n", cwd);
   	else
		printf('.');

	printf("erClientArguments: %s\n", erClientArguments);
	printf("Slave %d is going to run vrep\n", rank);
	system(erClientArguments);
	printf("Slave %d completed running vrep\n", rank);


	// sprintf( buf, "Hello master %d\n", rank );
	// MPI_Send( buf, strlen(erClientArguments) + 1, MPI_CHAR, 0, 0, master_comm );
	
	// FINALIZE BARRIER
	MPI_Barrier(MPI_COMM_WORLD);

	sprintf( buf, "Goodbye from slave %d\n", rank );
	MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm );
	printf("Slave %d is down\n", rank);

	return 0;
}

