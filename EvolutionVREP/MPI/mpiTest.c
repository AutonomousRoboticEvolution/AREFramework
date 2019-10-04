#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> 
#include <unistd.h>

int main(int argc, char ** argv) {
	int rank, size;
	char name[80];
	int length;

	printf("starting MPI");

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Get_processor_name(name,&length);
	
	char vrepArguments[255];
	sprintf(vrepArguments, "xvfb-run sh V-REP_PRO_EDU_V3_5_0_Linux/vrep.sh -h -g%d -g0 -g/share/frankv/test",rank);
		
	printf("Hello MPI: processor %d of %d on %s\n", rank,size,name);

	char cwd[1024];
   	if (getcwd(cwd, sizeof(cwd)) != NULL)
       		fprintf(stdout, "Current working dir: %s\n", cwd);
   	else
       		printf('.');

	printf("vrepArguments: %s\n", vrepArguments);
	system(vrepArguments);
	MPI_Finalize();
}

