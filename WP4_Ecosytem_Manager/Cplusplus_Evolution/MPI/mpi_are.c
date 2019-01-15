#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> 
#include <unistd.h>
#define WORKTAG     1
#define DIETAG     2

int main(int argc, char ** argv) {
	int rank, size;
	char name[80];
	int length;
	MPI_Comm new_comm;


	printf("starting MPI");

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
//	MPI_Comm_size(MPI_COMM_WORLD,&size);
  	MPI_Comm_split( MPI_COMM_WORLD, rank == 0, 0, &new_comm );
    	if (rank == 0) 
		printf("Rank is zero thus initializing master")<

		master_io( MPI_COMM_WORLD, new_comm );
    	else
		slave_io( MPI_COMM_WORLD, new_comm );
	
	

	MPI_Finalize();
}

/* This is the master */
int master_io( master_comm, comm )
MPI_Comm comm;
{
    int        i,j, size;
    char       buf[256];
	sprintf( buf, "Hello?");
 	printf("Master here\n");

    MPI_Status status;
printf("Master is going to run vrep\n");
    system("shvrep.sh");
printf("Master ran VREP\n");


    MPI_Comm_size( master_comm, &size );
    for (j=1; j<=2; j++) {
	for (i=1; i<size; i++) {
	    MPI_Recv( buf, 256, MPI_CHAR, i, 0, master_comm, &status );
	    fputs( buf, stdout );
	}
    }

}

/* This is the slave */
int slave_io( master_comm, comm )
MPI_Comm comm;
{
    char buf[256];
    	int rank, size, length;
	char name[80];
MPI_Status          status;

    MPI_Comm_rank( comm, &rank );
    sprintf( buf, "Hello from slave %d\n", rank );
    MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm );
    
   
	
	MPI_Get_processor_name(name,&length);
	char vrepArguments[255];
	//sprintf(vrepArguments, "xvfb-run sh vrep.sh -h -g%d -g0 -g/users/f/v/fveenstr/scratch/VREP/V-REP_PRO_V3_4_0_64_Linux/plantResults",rank);
		
	sprintf(vrepArguments, "sh vrep.sh -h -g%d -g0 -g/users/f/v/fveenstr/scratch/VREP/V-REP_PRO_V3_4_0_64_Linux/plantResults",rank);
	printf("Hello MPI: processor %d of %d on %s\n", rank,size,name);

	char cwd[1024];
   	if (getcwd(cwd, sizeof(cwd)) != NULL)
       		fprintf(stdout, "Current working dir: %s\n", cwd);
   	else
       		printf('.');

	printf("vrepArguments: %s\n", vrepArguments);
	printf("Slave %d is going to run vrep\n", rank);
	system(vrepArguments);
	printf("Slave %d completed running vrep\n", rank);


	sprintf( buf, "Hello master %d\n", rank );
        MPI_Send( buf, strlen(vrepArguments) + 1, MPI_CHAR, 0, 0, master_comm );


	

	if (status.MPI_TAG == DIETAG) {
		sprintf( buf, "Goodbye from slave %d\n", rank );
		MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm );
		printf("Slave %d is down\n", rank);
		return 0;

	}

}

