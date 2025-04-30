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
void crossover(Individual parent1, Individual parent2, Individual *offspring1, individual *offspring2);
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
	printf("parameters: pop_size=%d, chromo_len=%d, gens=%d, mut_rate=%.2f, cross_rate=%.2f\n"POPULATION_SIZE, CHROMOSOME_LENGTH, MAX_GENERATIONS, MUTATION_RATE, CROSSOVER_RATE);
	printf("-------------------------------------------------------------");
	printf("Generation 0: Best Fitness = %.4f\n", current_population.individuals[current_population.best_individual].fitness);

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
				next_population.individuals[bext_pop_idx++] = offspring1;
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
		 * 	printf("tarhet fitness reached at generation %d!\n, generation);
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
