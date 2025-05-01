#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


// parameters
#define CHROMOSOME_LENGTH 20
#define POPULATION_SIZE 100
#define MAX_GENERATIONS 500
#define MUTATION_RATE 0.1
#define CROSSOVER_RATE 0.7
#define TOURNAMENT_SIZE 3


// data structures
// single individual (potential solution)
typedef struct{
	int genes[CHROMOSOME_LENGTH];
	double fitness;
}Individual;

// the entire population of individuals
typedef struct{
	Individual individuals[POPULATION_SIZE];
	int best_individual_index;
	double total_fitness;
}Population;

// function prototypes

// problem specific fitness function:
double calculate_fitness(Individual *ind);

// GA Core functions
void initialize_population(Population *pop);
void evaluate_population(Population *pop);
Individual select_parent_tournament(Population *pop);
void crossover(Individual parent1, Individual parent2, Individual *offspring1, Individual *offspring2);
void mutate(Individual *ind);
void print_individual(Individual *ind);

int main(){
	// 1) seed random number generator
	srand((unsigned int)time(NULL));

	// 2) initialize the population
	Population current_population;
	initialize_population(&current_population);

	// 3) initialize evaluation
	evaluate_population(&current_population);

	printf("genetic algorithm start\n");
	printf("parameters: pop_size=%d, chromo_len=%d, gens=%d, mut_rate=%.2f, cross_rate=%.2f\n",POPULATION_SIZE, CHROMOSOME_LENGTH, MAX_GENERATIONS, MUTATION_RATE, CROSSOVER_RATE);
	printf("-------------------------------------------------------------");
	printf("Generation 0: Best Fitness = %.4f\n", current_population.individuals[current_population.best_individual_index].fitness);

	// 4) main GA loop (generational cycle)
	for(int generation = 1; generation <= MAX_GENERATIONS; generation++){
		Population next_population;
		int next_pop_idx = 0;
		//ensuring the best individual found so far is not lost (elitism)
		next_population.individuals[next_pop_idx++] = current_population.individuals[current_population.best_individual_index];

		// 5) generate the rest of the new population
		while(next_pop_idx < POPULATION_SIZE){
			// a. selection (choose two parents)
			Individual parent1 = select_parent_tournament(&current_population);
			Individual parent2 = select_parent_tournament(&current_population);

			// b. crossover (create two offspring from parents)
			Individual offspring1, offspring2;
			crossover(parent1, parent2, &offspring1, &offspring2);

			// c. mutation (apply random changes)
			mutate(&offspring1);
			mutate(&offspring2);

			// d. add offspring to the new population
			if(next_pop_idx < POPULATION_SIZE){
				next_population.individuals[next_pop_idx++] = offspring1;
			}
			if(next_pop_idx < POPULATION_SIZE){
				next_population.individuals[next_pop_idx++] = offspring2;
			}
		}
		
		// 6) replace the old population wit the newly generated one
		current_population = next_population;

		// 7) evaluate the fitness of the new population
		evaluate_population(&current_population);

		// 8) logging and termination check
		if(generation % 50 == 0 || generation == MAX_GENERATIONS){
			printf("generation %d: best fitness = %.4f\n", generation, current_population.individuals[current_population.best_individual_index].fitness);
		}

		//other termination conditions go here
		//	- target fitness reached
		//	- fitness hasn't improved in x generations
		/*
		 * if(current_population.individuals[current_population.best_individual_index].fitness >= TARGET_FITNESS){
		 * 	printf("target fitness reached at generation %d!\n, generation);
		 * 	break;
		 * 	}
		 * }
		 */
	}

	// 9) output the best solution found after all generations
	printf("-------------------------------------------------------------\n");
	printf("genetic algorithm finished.\n");
	printf("best solution found.\n");
	printf("fitness: %.4f\n", current_population.individuals[current_population.best_individual_index].fitness);
	printf("genes: ");
	print_individual(&current_population.individuals[current_population.best_individual_index]);
	printf("-------------------------------------------------------------\n");

	return 0;
}


// fitness function - customize this!
double calculate_fitness(Individual *ind){
	double score = 0.0;
	for(int i=0;i<CHROMOSOME_LENGTH;i++){
		if(ind->genes[i]==1)
			score += 1.0;
	}
	return score;
}


// GA core functions implementations

/**
 * @brief initializes the population with random individuals.
 * @param pop Pointer to the Population struct to initialize
 */

void initialize_population(Population *pop){
	for(int i=0;i<POPULATION_SIZE;i++){
		for(int j=0;j<CHROMOSOME_LENGTH;j++){
			// randomly initialize genes (bin)
			pop->individuals[i].genes[j] = rand() % 2;
		}
		//initialize fitness to 0 before evaluation
		pop->individuals[i].fitness = 0.0;
	}
	pop->best_individual_index = -1; // not id'd yet
	pop->total_fitness = 0.0; //not calculated yet
}


/**
 * @brief calculates the fitness for each individual in the population
 * @param pop pointer to the Population struct to evaluate
 */

void evaluate_population(Population *pop){
	pop->total_fitness=0.0;
	double best_fitness = -1e0; // very small number - assumes non-negative fitness
	for(int i=0;i<POPULATION_SIZE;i++){
		//calculating and storing fitness for each individual
		pop->individuals[i].fitness = calculate_fitness(&(pop->individuals[i]));
		pop->total_fitness+=pop->individuals[i].fitness;
		//tracking the best individual so far found in this generation
		if(pop->individuals[i].fitness>best_fitness){
			best_fitness = pop->individuals[i].fitness;
			pop->best_individual_index = i;
		}
	}
	// cases where all fitnesses might be 0 or <0
	if(pop->best_individual_index == -1 && POPULATION_SIZE > 0)
		pop->best_individual_index = 0;
}

/**
 * @brief selects a parent from the population using tournament selection.
 * randomly picks TOURNAMENT_SIZE individuals and returns the fittest among them.
 * @param pop pointer to the current population.
 * @return a copy of the selected parent individual
 */

Individual select_parent_tournament(Population *pop){
	int best_idx = -1;
	double best_fitness = -1e9; // very small number
	
	// running the tournament
	for(int i=0;i<TOURNAMENT_SIZE;i++){
		int random_idx = rand() % POPULATION_SIZE; // picks a random competitor
		if(pop->individuals[random_idx].fitness>best_fitness){
			best_fitness = pop->individuals[random_idx].fitness;
			best_idx = random_idx;
		}
	}
	// return a copy of the winner
	// defaults to 0 if best_idx remains -1
	if(best_idx == -1) best_idx = rand() % POPULATION_SIZE;
	return pop->individuals[best_idx];
}

/**
 * @brief performs single-point crossover between two parents to create two offspring.
 * crossover occurs based on CROSSOVER_RATE. if no crossover, offspring are clones.
 * @param parent1 the first parent individual.
 * @param parent2 the 2nd parent individual.
 * @param offspring1 pointer to the first offspring individual.
 * @param offspring2 pointer to the 2nd offspring individual.
 */

void crossover(Individual parent1, Individual parent2, Individual *offspring1, Individual *offspring2){
	// decide if crossover will happen based on the rate
	if(((double)rand() / RAND_MAX) > CROSSOVER_RATE){
		// no crossover - offspring are clones of the parents
		*offspring1 = parent1;
		*offspring2 = parent2;
	}else{
		// crossover - choose point in [idx1 : len(2)]
		int point = (rand() % (CHROMOSOME_LENGTH -1))+1;
		// create offspring by swapping gene segments
		for(int i=0;i<CHROMOSOME_LENGTH;i++){
			if(i<point){
				//1st part of parent1, 2nd part of parent2
				offspring1->genes[i] = parent1.genes[i];
				offspring2->genes[i] = parent2.genes[i];
			}else{
				//1st part of parent2, 2nd part of parent1
				offspring1->genes[i] = parent2.genes[i];
				offspring2->genes[i] = parent1.genes[i];
			}
		}
	}
	// resetting fitness for offspring (later evaluated)
	offspring1->fitness = 0.0;
	offspring2->fitness = 0.0;
}

/**
 * @brief applies mutation to an individual's genes based on MUTATION_RATE
 * flipping a bit...
 * @param ind pointer to the individual to potentially mutate
 */

void mutate(Individual *ind){
	for(int i=0;i<CHROMOSOME_LENGTH;i++){
		if(((double)rand() / RAND_MAX) < MUTATION_RATE)
			ind->genes[i] = 1 - ind->genes[i];
	}
}

/**
 * @brief helper function to print the genes of an individual.
 * @param ind pointer to the individual to print.
 */

void print_individual(Individual *ind){
	printf("[ ");
	for(int i=0;i<CHROMOSOME_LENGTH;i++){
		printf("%d ", ind->genes[i]);
	}
	printf("]\n");
}
