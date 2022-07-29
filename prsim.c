#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
//note to self: figure out using scanf for malformed line issue
//also create a function for inserting something onto the CPU to help mitigate the RNG function, probably a good idea to add one for the IO as well.
typedef struct CPU {
	int busy;
	int idle;
	float utilization;
	int dispatches;		
	float thoroughput;
	} CPU;
struct tasks {
	char  name[10];
	int   runtime;
	float blockfloat;
	struct tasks *next;
	int dispatches;
	int completed;
	int IOblocks;
	int IOtime;
	int initialRuntime;
	int newOnStack;
	bool blocking;
	int blockTime; // time until blocking
	int IOruntime; // time to run in IO queue
};

void CPUInsert(struct tasks* item, struct tasks* tail){
	//printf("TAIL ORIGINAL %s\n", tail->name);
	tail->next = item;
	item->next = NULL;
	tail = item;
	//printf("TAIL NOW %s\n", tail->name);
	// add RNG and blocking/blockTime set here
}
int main(int argc, char *argv[]) {
	(void) srandom(12345);
	short RR = 0;
	short quantum = 5;
	int completedThisTick = 0;
	struct tasks* endHead;

	typedef struct IODevice {
		int busy;
		int idle;
		float utilization;
		int starts;
		float thoroughput;
		struct tasks* head;
	} IODevice;
	int ctime = 0;
	CPU cpu = {
		.busy = 0,
		.idle = 0,
		.utilization = 0,
		.dispatches = 0,
		.thoroughput = 0
	};
	IODevice io = {
		.busy = 0,
		.idle = 0,
		.utilization = 0,
		.starts = 0,
		.thoroughput = 0,
		.head = NULL
	};
	if (argc < 3) {
		fprintf(stderr, "Invalid args. Usage: ./procsim -r / -f input.txt");
		exit(1);
	}
	else if (strcmp(argv[1], "-r") == 0) {
		RR = 1;
	}
	else if (strcmp(argv[1], "-f") == 0) {
		RR = 0;
	}
	else {
		fprintf(stderr, "Usage: %s [-r | -f] file\n", argv[0]);
		exit(1);
	}
	FILE *fp;
	fp = fopen(argv[2], "r");
	if (fp == NULL) {
		errno = ENOENT;
		perror(argv[2]);
		exit(1);
	}
	struct tasks *head = NULL;
	struct tasks *tail = NULL;
	char buffer[200]; 
	float f;
	int run;
	int i = 0;
	while(fgets(buffer, 200, fp) != NULL) {
		char* input;
		struct tasks *temp = (struct tasks*) malloc(sizeof(struct tasks));
		input = strtok (buffer, "\t\n ,-");
		if (input == NULL) {
			fprintf(stderr, "Malformed line %s(%d)\n", argv[2], i+1);
			exit(1);
		}
		if (strlen(input) > 10) {
			fprintf(stderr, "name is too long %s(%d)\n", input, i+1);
			exit(1);
		}
		else {
			strcpy(temp->name, input);
		}
		input = strtok(NULL, "\t\n ,-");
		if (input == NULL) {
			fprintf(stderr, "Malformed line %s(%d)\n", argv[2], i+1);
			exit(1);
		}
		run = atoi(input);
		if (run < 1) {
			if (input[0] != '0') {
				fprintf(stderr, "Malformed line %s(%d)\n", argv[2], i+1);
				exit(1);
			}
			else {
				fprintf(stderr, "runtime is not positive integer %s(%d)\n", argv[2], i+1);
				exit(1);
			}
		}
		else {
			temp->runtime = run;
			temp->initialRuntime = run;
		}
		input = strtok(NULL, "\t\n ,-");
		if (input == NULL) {
			fprintf(stderr, "Malformed line %s(%d)\n", argv[2], i+1);
			exit(1);
		}
		else if ((strlen(input) > 4 && !isspace(input[4])) || (input[0]  != '0' && input[0] != '1') || input[1] != '.' || (input[0] == '1' && (input[2] != '0' || input[3] != '0')) ) {
		//	printf("%ld\n", strlen(input));
			//printf("%d\n", input[4]);
			//printf("test: %d %d %d\n", (input[0]  != '0' && input[0] != '1'), strlen(input) > 4, input[1] != '.');
			fprintf(stderr, "probability < 0 or > 1 %s(%d)\n", argv[2], i+1);
			exit(1);
		}
		else {
				f = atof(input);
				temp->blockfloat = f;
		}
		input = strtok(NULL, "\t\n ,-");
		if (input != NULL) {
			fprintf(stderr, "Malformed line %s(%d)\n", argv[2], i+1);
		}
		temp->completed = 0;
		temp->IOblocks = 0;
		temp->IOtime = 0;
		temp->blocking = 0;
		temp->blockTime = 0;
		temp->newOnStack = 1;
		temp->IOruntime = 0;
		temp->dispatches = 0;
		temp->next = NULL;
		if (i == 0) {
			head = temp;
			tail = temp;
			endHead = temp;
		}
		else {
			tail->next = temp;
			tail = temp;
		}
		cpu.dispatches += 1;
		temp->dispatches += 1;
		i = i + 1;
	}
	// main routine
	printf("Processes:\n\n");
	printf("   name     CPU time  when done  cpu disp  i/o disp  i/o time\n");
	while ((head != NULL || io.head != NULL) && ctime != 300) {
		ctime = ctime + 1;
	//	printf("TICK: %d\n", ctime);
		// check CPU queue if something is done
		if (head != NULL) {
			if (head->runtime == 0 || (head->blockTime == 0 && head->blocking)) {
				if (quantum != 5 && RR == 1 && head->blocking != 1) {
					// quantum only matters here if there's something else on the CPU to work on
					//going to have to do rng here
					cpu.dispatches += 1;
					head->dispatches +=1;
					head->newOnStack = 1;
					if (head->next != NULL) {
						struct tasks* temp = head;
						head = head->next;
						temp->next = NULL;
						tail->next = temp;
						tail = tail->next;
					}
					quantum = 5;
				}
				quantum = 5;
				if (head->blocking) {
					io.starts += 1;
					head->IOblocks += 1;
					head->newOnStack = ctime;
					if (io.head != NULL) {
						struct tasks* temp = io.head;
						while (temp->next != NULL) {
							temp = temp->next;
						}
						temp->next = head;
					}
					else {
						io.head = head;
					}
					head->blocking = false;
					struct tasks* temp2 = head;
					head = head->next;
					if (head == NULL) {
						tail = NULL;
					}
					temp2->next = NULL;
				}
				if (head != NULL && head->runtime == 0) {
					head->completed = ctime;
					completedThisTick = 1;
					struct tasks* endHead = head;
					printf("%-10s %6d     %6d    %6d    %6d    %6d\n",endHead->name, endHead->initialRuntime, endHead->completed, endHead->dispatches, endHead->IOblocks, endHead->IOtime);
					head = head->next;
					free(endHead);
				}
			}
			else if (quantum == 0) {
				// quantum only matters here if there's something else on the CPU to work on
				//going to have to do rng here
				cpu.dispatches += 1;
				head->dispatches +=1;
				head->newOnStack = 1;
				if (head->next != NULL) {
					struct tasks* temp = head;
					head = head->next;
					temp->next = NULL;
					tail->next = temp;
					tail = tail->next;
				}
				quantum = 5;
			}
		}
		// CPU queue
		if (head != NULL && completedThisTick == 0) {
			//printf("CPU queue:\n"); 
			struct tasks* temp = head;
			while (temp != NULL) {
			//	printf("\t%s runtime:%d quantum: %d, blockTime: %d dispatches: %d\n", temp->name, temp->runtime, quantum, temp->blockTime, temp->dispatches);
				temp = temp->next;
			}
			if (head->newOnStack == 1) {
				if (head->runtime >= 2) {
					float f = (float)random() / RAND_MAX;
				//	printf("FLOAT GENERATED: %f\n", f);
					if (head->blockfloat > f) {
						int temp1 = random();
					//	printf("FLOAT for %s generated as %d\n", head->name, temp1);
						if (RR == 1) {
							int calculatorVar = 0;
							if (head->runtime < quantum) {
								calculatorVar = head->runtime;
							}
							else {
								calculatorVar = quantum;
							}
							int temp = (temp1 % calculatorVar) + 1;
					//		printf("Int using generated: %d\n", temp);
							head->blockTime = temp;
						}
						else {
							int temp = (temp1 % head->runtime) + 1;
					//		printf("Int using generated: %d\n", temp);
							head->blockTime = temp;
						}
						head->blocking = 1;
					}
				}
				head->newOnStack = 0;
			}
			head->runtime = head->runtime - 1;
			if (RR == 1) {
				quantum = quantum - 1;
			}
			cpu.busy += 1;
			if (head->blocking) {
				head->blockTime = head->blockTime - 1;
			}
		}
		else {
		//	printf("completedThisTick = %d", completedThisTick);
			cpu.idle += 1;
			completedThisTick = 0;
		}
		// io done routine
		if (io.head != NULL) {
			if (io.head->IOruntime == 0 && io.head->newOnStack == 0) {
				struct tasks* temp = io.head->next;
				cpu.dispatches += 1;
				io.head->dispatches +=1;
				io.head->newOnStack = 1;
				if (head == NULL) {
					head = io.head;
					head->next = NULL;
					tail = head;
				} else {
					CPUInsert(io.head, tail);
					tail = tail->next;
				}
				io.head = temp;

			}
		}
		// IO queue
		if (io.head != NULL) {
			if (io.head->newOnStack <= ctime && io.head->newOnStack != 0) {
				if (io.head->runtime == 0) {
					io.head->newOnStack = 0;
					io.head->IOruntime = 1;
				}
				else {
					int temp = random();
				//	printf("GENERATED RANDOM FOR IO: %d\n", temp);
					temp = temp % 30;
					io.head->IOruntime = temp + 1;
				//	printf("Process %s IOtime allocated as %d\n", io.head->name, io.head->IOruntime);
					io.head->newOnStack = 0;
				}
			}
			if (io.head->newOnStack == 0) {
			//	printf("IO queue:\n"); 
				struct tasks* temp = io.head;
				while (temp != NULL) {
				//	printf("\t%s runtime:%d\n", temp->name, temp->IOruntime);
					temp = temp->next;
				}
				io.head->IOruntime = io.head->IOruntime - 1;
				io.head->IOtime = io.head->IOtime + 1;
				io.busy += 1;
				if (io.head->IOruntime == 0) {
				//	printf("IO finishes, pushing to cpu\n");
					struct tasks* temp = io.head->next;
					cpu.dispatches += 1;
					io.head->dispatches +=1;
					io.head->newOnStack = 1;
				//	printf("current task %s dispatches: %d\n", io.head->name, io.head->dispatches);
					if (head == NULL) {
						head = io.head;
						head->next = NULL;
						tail = head;
					} else {
						CPUInsert(io.head, tail);
						tail = tail->next;
					}
					io.head = temp;
				}
			}
			else {
				io.idle += 1;
			//	printf("JUST JOINED IO QUEUE HOLD A MOMENT %s\n", io.head->name);
			}
		}
		else {
			io.idle += 1;
		}
	}
	// test printing:
	
	/* print clock time at end */
	printf("\nSystem:\n");
	printf("The wall clock time at which the simulation finished: %d\n", ctime);

	/* print cpu statistics */
	printf("\nCPU:\n");
	printf("Total time spent busy: %d\n", cpu.busy);
	printf("Total time spent idle: %d\n", cpu.idle);
	printf("CPU utilization: %.2f\n", (float)cpu.busy / (float)ctime);
	printf("Number of dispatches: %d\n", cpu.dispatches);
	printf("Overall throughput: %.2f\n", (float)i / (float)ctime);

	/* print i/o statistics */
	printf("\nI/O device:\n");
	printf("Total time spent busy: %d\n", io.busy);
	printf("Total time spent idle: %d\n", io.idle);
	printf("I/O utilization: %.2f\n", (float)io.busy/(float)ctime);
	printf("Number of dispatches: %d\n", io.starts);
	printf("Overall throughput: %.2f\n", (float) i / (float)ctime);

	
	
	
	
}
