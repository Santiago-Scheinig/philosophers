/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:52:48 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/10 19:14:06 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>

/*--------------------------------------------------------------------------*/
/*------------------------------ ENUMERATIONS ------------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * An enumeration list of errors codes used to identify the correct message.
 */
typedef enum e_philo_errno
{
	PH_SUCCESS = 0,		// Success
	PH_ARG_CINV,		// Amount of arguments isn't valid
	PH_ARG_VINV,		// Arguments aren't valid
	PH_MTX_IERR,		// Mutex initialization failed
	PH_MTX_DERR,		// Mutex destruction failed
	PH_MEM_AERR,		// Memory allocation failed
	PH_THD_CERR,		// Thread creation failed
}	t_philo_errno;		// @param enum_format PH_*

/**
 * An enumeration list of time codes to identify specific actions to perform
 * inside of a mutexed timestamp.
 */
typedef enum e_mtx_time
{
	MTX_TIME_IS	= 0,	// Returns timestamp depending on eating flag value
	MTX_TIME_ISFULL,	// To modify the eating flag to 0
	MTX_TIME_ISEATING,	// To modify the eating flag to 1
}	t_mtx_time;			// @param enum_format MTX_TIME_*

/**
 * An enumeration list of flag codes to identify specific actions to perform
 * inside of a mutexed flag.
 */
typedef enum e_mtx_flag
{
	MTX_FLAG_READ = 0,	// To read a mutexed variable
	MTX_FLAG_ON,		// To set a mutexed flag to 1
	MTX_FLAG_OFF,		// To set a mutexed flag to 0
	MTX_FLAG_INC,		// To increment a mutexed flag by 1
	MTX_FLAG_SUB,		// To substract a muteed flag by 1
}	t_mtx_flag;			// @param enum_format MTX_FLAG_*

/**
 * An enumeration list of print codes to identify specific messages to print
 * on STDOUT filtered by a mutex.
 */
typedef enum e_mtx_print
{
	MTX_PRINT_FORK = 0,	// Prints "[ts_in_ms] [philo_id] has taken a fork."
	MTX_PRINT_EAT,		// Prints "[ts_in_ms] [philo_id] is eating."
	MTX_PRINT_SLEEP,	// Prints "[ts_in_ms] [philo_id] is sleeping."
	MTX_PRINT_THINK,	// Prints "[ts_in_ms] [philo_id] is thinking."
	MTX_PRINT_DEATH,	// Prints "[ts_in_ms] [philo_id] died."
}	t_mtx_print;		// @param enum_format MTX_PRINT_*

/*--------------------------------------------------------------------------*/
/*------------------------------- STRUCTURES -------------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Struct used to save the rules of the experiment.
 * 
 * @param n_philo The amount of philosophers that will seat on the table.
 * @param n_forks The amount of forks avalible on the table.
 * @param time_to_die The time limit a philosopher can be without eating.
 * @param time_to_eat The time it takes a philosopher to eat.
 * @param time_to_sleep The time it takes a philosopher to sleep.
 * @param meals_required The amount of meals each philosopher must eat.
 * @param start_time The starting time of the experiment.
 * @param death_flag Flag used to start and end the experiment.
 * @param monitor_id The id of the PTHREAD_T to the monitor.
 * @param death_mutex A mutex linked to death_flag.
 * @param print_mutex A mutex linked to printf.
 * @param forks A pointer to a PTHREAD_T ARRAY to the forks in table order.
 * @param meals A pointer to a PTHREAD_T ARRAY to mute the timestamps of meals.
 */
typedef struct s_rules
{
	int				n_philo;
	int				n_forks;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				meals_required;
	struct timeval	start_time;
	int				death_flag;
	pthread_t		monitor_id;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	*forks;
	pthread_mutex_t	*meals;
}	t_rules;

/**
 * Struct used to save the information of a philosopher.
 * 
 * @param id The identification number of the philosopher.
 * @param meals_eaten The amount of meals the philosopher had eaten.
 * @param is_eating A flag to follow if the philosopher is eating or not.
 * @param last_meal_time The time the philosopher finished its last meal.
 * @param thd_id The identification thread of the philosopher.
 * @param left_fork A pointer to a fork mutex to the left of the philosopher.  
 * @param right_fork A pointer to a fork mutex to the right of the philosopher.
 * @param meal_mutex A pointer to a meal mutex linked to [meals_eaten] and 
 * [last_meal_time].
 * @param table A pointer to the experiment main T_RULES structure.
 * 
 * @warning No philosopher should be allowed to WRITE on the main T_RULES
 * structure, only READ, doing so leads to undifined behaivor and data races.
 * To interact with the rules, use only functions protected with a mutex
 * like to_dead_flag() or to_print_access().
 */
typedef struct s_philosopher
{
	int				id;
	int				meals_eaten;
	int				is_eating;
	struct timeval	last_meal_time;
	pthread_t		thd_id;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*meal_mutex;
	t_rules			*table;
}	t_philosopher;

/**
 * Struct used to save all the information of the experiment as a whole.
 * 
 * @param table A pointer to the main T_RULES structure.
 * @param seats A pointer to the T_PHILOSOPHER structure array.
 */
typedef struct s_monitor
{
	t_rules			*table;
	t_philosopher	*seats;
}	t_monitor;

/*--------------------------------------------------------------------------*/
/*----------------------------- INITIALIZATION -----------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Initializes all necessary mutex to run the philosopher program.
 * 
 * 	- Death mutex: Used to avoid data races with the death flag.
 *
 * 	- Print mutex: Used to avoid data races with the print flag.
 * 
 * 	- Forks mutex: Used to avoid data races with forks.
 * 
 * @param table The main enviroment philosopher structure.
 */
int				initialize_mutex(t_rules *table);

/**
 * Destroys every mutex created to run the philosopher program.
 * @param table The main enviroment philosopher structure.
 * @note If any mutex destruction fails, an error message is
 * printed on screen detailing the failed mutex, the program
 * then exits with forcend(4).
 */
int				destroy_mutex(t_rules *table);

/**
 * Initializes all necesary threads to run the philosopher program.
 * The amount of threads created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 * @return A pointer to the allocated array of T_PHILOSOPHERS.
 */
int				start_dinner(t_rules *table, t_philosopher **seats);

/*--------------------------------------------------------------------------*/
/*------------------------------ MUTEX_ACTIONS -----------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Using the t_mtx_flag enum, executes instructions on the death_flag
 * variable, saved on the main experiment structure, avoiding data races using
 * a mutex.
 * 
 * @param table A pointer to the main experiment structure.
 * @param action The action to perform into death_flag.
 */
int				to_death_flag(t_rules *table, t_mtx_flag action);

/**
 * Using the t_mtx_time enum, executes instructions on the last_meal timestamp
 * variable, saved on the T_PHILOSOPHER structure, avoiding data races using
 * a mutex.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the last_meal
 * timestamp variable.
 * @param action The action to perfrom into meals_eaten
 */
struct timeval	to_time_value(t_philosopher *seat, t_mtx_time action);

/**
 * Using the t_mtx_flag enum, executes instructions on the meals_eaten
 * variable, saved on the T_PHILOSOPHER structure, avoiding data races using
 * a mutex.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the meals_eaten
 * variable.
 * @param action The action to perfrom into meals_eaten
 */
int				to_meals_value(t_philosopher *seat, t_mtx_flag action);

/**
 * Using the T_MTX_PRINT enum, prints a specific message on STDOUT linked to
 * the pointer to a T_PHILOSOPHER seat, avoiding print chunks using a mutex.
 * 
 * @param seat A pointer to the T_PHILOSOPHER linked to the message.
 * @param action The action to perform into death_flag.
 */
void			to_print_access(t_philosopher *seat, t_mtx_print action);

/*--------------------------------------------------------------------------*/
/*---------------------------- THREAD ROUTINES -----------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * The monitor thread routine.
 * 
 * - Sets the [start_time] of the dinner to current time.
 * 
 * - Initializes the dinner by setting the [death_flag] to false.
 * 
 * @param arg A pointer to the needed arguments of the routine. A pointer to
 * the T_MONITOR structure in this case.
 * @note If [death_flag] becomes true, it joins every philosopher thread and
 * frees it's own structure. 
 */
void			*monitorize(void *arg);

/**
 * The philosopher thread routine.
 * 
 * - Sets the [last_meal_time] to [start_time].
 * 
 * - If odd, waits ([time_to_eat] / 2) to create eating waves.
 * 
 * - While [death_flag] is false: Thinks, eats and sleeps.
 * 
 * @param arg A pointer to the needed arguments of the routine. A pointer to
 * the T_PHILOSOPHER structure in this case.
 * @note If [n_philosopher] it's an odd amount, uses is_hungry() to set 
 * priorities between waves.
 */
void			*philo(void *arg);

/*--------------------------------------------------------------------------*/
/*---------------------------------- UTILS ---------------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Finds the first number on a STRING with a decimal base.
 * @param str The string where the base number is saved.
 * @param base The base in which the number must be found.
 * @return The decimal INT found on STR.
 * @note A number can, but is not forced to, start with any
 * amount of spaces and one sign; but the next character must 
 * be a digit, if not or str doesn't exists, returns 0.
 */
int				ft_atoi(const char *nptr);

/**
 * Philosopher failsafe, in case of error, frees all memory that could remain
 * allocated in the main structure, and destroy any created mutex.
 * @param table A pointer to the main environment philosopher structure.
 * @param errmsg The error number which points to its error string.
 */
int				forcend(t_rules *table, t_philosopher *chairs, int errmsg);

#endif