/* firestarter.c 
 * David Joiner
 * Usage: Fire [forestSize(20)] [numTrials(5000)] * [numProbabilities(101)] [showGraph(1)]
 * 
 * Edited by Quentin Barnes
 * For CS 374 HW 3
 * At Calvin University
 * On Sep 25, 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include "X-graph.h"
#include <mpi.h>

#define UNBURNT 0
#define SMOLDERING 1
#define BURNING 2
#define BURNT 3

#define true 1
#define false 0

typedef int boolean;

extern void seed_by_time(int);
extern int **allocate_forest(int);
extern void initialize_forest(int, int **);
extern double get_percent_burned(int, int **);
extern void delete_forest(int, int **);
extern void light_tree(int, int **, int, int);
extern boolean forest_is_burning(int, int **);
extern void forest_burns(int, int **, double);
extern int burn_until_out(int, int **, double, int, int);
extern void print_forest(int, int **);

int main(int argc, char **argv)
{
    // initial conditions and variable definitions
    int forest_size = 20;
    double *prob_spread;
    double prob_min = 0.0;
    double prob_max = 1.0;
    double prob_step;
    int **forest;
    double *percent_burned;
    int i_trial;
    int n_trials = 5000;
    int i_prob;
    int n_probs = 101;
    int do_display = 0;
    xgraph thegraph;
    double *times_burned;
    int id = -1, numWorkers = -1;
    double *final_percent_burned;
    double *final_times_burned;

    // check command line arguments

    if (argc > 1)
    {
        sscanf(argv[1], "%d", &forest_size);
    }
    if (argc > 2)
    {
        sscanf(argv[2], "%d", &n_trials);
    }
    if (argc > 3)
    {
        sscanf(argv[3], "%d", &n_probs);
    }
    if (argc > 4)
    {
        sscanf(argv[4], "%d", &do_display);
    }
    if (do_display != 1)
        do_display = 0;

    //Init MPI and set id and numWorkers
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);

    // setup problem.  Allocate memory for arrays
    seed_by_time(0);
    forest = allocate_forest(forest_size);
    prob_spread = (double *)malloc(n_probs * sizeof(double));
    percent_burned = (double *)malloc(n_probs * sizeof(double));
    times_burned = (double *)malloc(n_probs * sizeof(double));

    //Start timer
    double startTime = 0.0, totalTime = 0.0;
    startTime = MPI_Wtime();

    // for a number of probabilities, calculate
    // average burn and output. Print if process 0
    prob_step = (prob_max - prob_min) / (double)(n_probs - 1);
    if (id == 0)
    {
        printf("Probability of fire spreading, Average percent burned, Average times burned\n");
    }

    for (i_prob = 0; i_prob < n_probs; i_prob++)
    {
        //for each probability, and the number of trials, calculate average
        //percent burn and iterations it took to burn
        //Splits the 5000 trials into chunks depending on the # of processes
        prob_spread[i_prob] = prob_min + (double)i_prob * prob_step;
        for (i_trial = id; i_trial < n_trials; i_trial += numWorkers)
        {
            //burn until fire is gone and add the number of steps it took
            times_burned[i_prob] += burn_until_out(forest_size, forest, prob_spread[i_prob],
                                                   forest_size / 2, forest_size / 2);
            percent_burned[i_prob] += get_percent_burned(forest_size, forest);
        }
    }

    //Allocate memory for the arrays that will be getting reduced into
    final_percent_burned = (double *)malloc(n_probs * sizeof(double));
    final_times_burned = (double *)malloc(n_probs * sizeof(double));

    //Reduce the arrays that had the fragments of data in them to
    //one final array
    MPI_Reduce(times_burned, final_times_burned, n_probs, MPI_DOUBLE, MPI_SUM, 0,
               MPI_COMM_WORLD);
    MPI_Reduce(percent_burned, final_percent_burned, n_probs, MPI_DOUBLE, MPI_SUM, 0,
               MPI_COMM_WORLD);

    //Stop the timer since the parallel calculations are done
    totalTime = MPI_Wtime() - startTime;

    //Print out the averages for percent burned and iterations it took
    if (id == 0)
    {
        for (int i = 0; i < n_probs; i++)
        {
            final_percent_burned[i] /= n_trials;
            final_times_burned[i] /= n_trials;

            // print output
            printf("%lf , %lf , %lf\n", prob_spread[i],
                   final_percent_burned[i], final_times_burned[i]);
        }

        //Print total time took
        printf("Firestarter took %f\n\n", totalTime);
    }

    //If master process and display is set to true show graph
    //Though it doesn't seem to work now that the program is parellelized
    if (id == 0)
    {
        if (do_display == 1)
        {
            xgraphSetup(&thegraph, 300, 300);
            xgraphDraw(&thegraph, n_probs, 0, 0, 1, 1, prob_spread, final_percent_burned);
            pause();
        }
    }

    //Clean up memory and process and exit
    delete_forest(forest_size, forest);
    free(prob_spread);
    free(percent_burned);
    free(times_burned);
    free(final_percent_burned);
    free(final_times_burned);
    MPI_Finalize();
    return 0;
}

#include <time.h>

void seed_by_time(int offset)
{
    time_t the_time;
    time(&the_time);
    srand((int)the_time + offset);
}

int burn_until_out(int forest_size, int **forest, double prob_spread,
                   int start_i, int start_j)
{
    int count;

    initialize_forest(forest_size, forest);
    light_tree(forest_size, forest, start_i, start_j);

    // burn until fire is gone
    count = 0;
    while (forest_is_burning(forest_size, forest))
    {
        forest_burns(forest_size, forest, prob_spread);
        count++;
    }

    return count;
}

double get_percent_burned(int forest_size, int **forest)
{
    int i, j;
    int total = forest_size * forest_size - 1;
    int sum = 0;

    // calculate pecrent burned
    for (i = 0; i < forest_size; i++)
    {
        for (j = 0; j < forest_size; j++)
        {
            if (forest[i][j] == BURNT)
            {
                sum++;
            }
        }
    }

    // return percent burned;
    return ((double)(sum - 1) / (double)total);
}

int **allocate_forest(int forest_size)
{
    int i;
    int **forest;

    forest = (int **)malloc(sizeof(int *) * forest_size);
    for (i = 0; i < forest_size; i++)
    {
        forest[i] = (int *)malloc(sizeof(int) * forest_size);
    }

    return forest;
}

void initialize_forest(int forest_size, int **forest)
{
    int i, j;

    for (i = 0; i < forest_size; i++)
    {
        for (j = 0; j < forest_size; j++)
        {
            forest[i][j] = UNBURNT;
        }
    }
}

void delete_forest(int forest_size, int **forest)
{
    int i;

    for (i = 0; i < forest_size; i++)
    {
        free(forest[i]);
    }
    free(forest);
}

void light_tree(int forest_size, int **forest, int i, int j)
{
    forest[i][j] = SMOLDERING;
}

boolean fire_spreads(double prob_spread)
{
    if ((double)rand() / (double)RAND_MAX < prob_spread)
        return true;
    else
        return false;
}

void forest_burns(int forest_size, int **forest, double prob_spread)
{
    int i, j;
    extern boolean fire_spreads(double);

    //burning trees burn down, smoldering trees ignite
    for (i = 0; i < forest_size; i++)
    {
        for (j = 0; j < forest_size; j++)
        {
            if (forest[i][j] == BURNING)
                forest[i][j] = BURNT;
            if (forest[i][j] == SMOLDERING)
                forest[i][j] = BURNING;
        }
    }

    //unburnt trees catch fire
    for (i = 0; i < forest_size; i++)
    {
        for (j = 0; j < forest_size; j++)
        {
            if (forest[i][j] == BURNING)
            {
                if (i != 0)
                { // North
                    if (fire_spreads(prob_spread) && forest[i - 1][j] == UNBURNT)
                    {
                        forest[i - 1][j] = SMOLDERING;
                    }
                }
                if (i != forest_size - 1)
                { //South
                    if (fire_spreads(prob_spread) && forest[i + 1][j] == UNBURNT)
                    {
                        forest[i + 1][j] = SMOLDERING;
                    }
                }
                if (j != 0)
                { // West
                    if (fire_spreads(prob_spread) && forest[i][j - 1] == UNBURNT)
                    {
                        forest[i][j - 1] = SMOLDERING;
                    }
                }
                if (j != forest_size - 1)
                { // East
                    if (fire_spreads(prob_spread) && forest[i][j + 1] == UNBURNT)
                    {
                        forest[i][j + 1] = SMOLDERING;
                    }
                }
            }
        }
    }
}

boolean forest_is_burning(int forest_size, int **forest)
{
    int i, j;

    for (i = 0; i < forest_size; i++)
    {
        for (j = 0; j < forest_size; j++)
        {
            if (forest[i][j] == SMOLDERING || forest[i][j] == BURNING)
            {
                return true;
            }
        }
    }
    return false;
}

void print_forest(int forest_size, int **forest)
{
    int i, j;

    for (i = 0; i < forest_size; i++)
    {
        for (j = 0; j < forest_size; j++)
        {
            if (forest[i][j] == BURNT)
            {
                printf(".");
            }
            else
            {
                printf("X");
            }
        }
        printf("\n");
    }
}
