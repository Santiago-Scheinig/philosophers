/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:58:40 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/10 18:46:54 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <sys/time.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>
# include <semaphore.h>

/*--------------------------------------------------------------------------*/
/*------------------------------ ENUMERATIONS ------------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * An enumeration list of errors codes used to identify the correct message.
 */
typedef	enum e_philo_errno
{
	PH_SUCCESS = 0,		// Success
	PH_ARG_CINV,		// Amount of arguments isn't valid
	PH_ARG_VINV,		// Arguments aren't valid
	PH_SEM_IERR,		// Semaphore initialization failed
	PH_SEM_DERR,		// Semaphore destruction failed
	PH_MEM_AERR,		// Memory allocation failed
	PH_THD_CERR,		// Thread creation failed
	PH_PCS_CERR			// Proccess creation failed
}	t_philo_errno;		// @param enum_format PH_*

/**
 * An enumeration list of time codes to identify specific actions to perform
 * inside of a mutexed timestamp.
 */
typedef enum e_sem_time
{
	SEM_TIME_IS	= 0,	// Returns timestamp depending on eating flag value
	SEM_TIME_ISFULL,	// To modify the eating flag to 0
	SEM_TIME_ISEATING	// To modify the eating flag to 1
}	t_sem_time;			// @param enum_format SEM_TIME_*

/**
 * An enumeration list of flag codes to identify specific actions to perform
 * inside of a mutexed flag.
 */
typedef	enum e_sem_flag
{
	SEM_FLAG_READ = 0,	// To read a semaphore protected variable
	SEM_FLAG_ON,		// To set a semaphore protected flag to 1
	SEM_FLAG_OFF,		// To set a semaphore protected flag to 0
	SEM_FLAG_INC,		// To increment a semaphore protected flag by 1
	SEM_FLAG_SUB		// To substract a semaphore protected flag by 1
}	t_sem_flag;			// @param enum_format SEM_FLAG_*

/**
 * An enumeration list of print codes to identify specific messages to print
 * on STDOUT filtered by a mutex.
 */
typedef	enum e_sem_print
{
	SEM_PRINT_FORK = 0,	// Prints "[ts_in_ms] [philo_id] has taken a fork."
	SEM_PRINT_EAT,		// Prints "[ts_in_ms] [philo_id] is eating."
	SEM_PRINT_SLEEP,	// Prints "[ts_in_ms] [philo_id] is sleeping."
	SEM_PRINT_THINK,	// Prints "[ts_in_ms] [philo_id] is thinking."
	SEM_PRINT_DEATH		// Prints "[ts_in_ms] [philo_id] died."
}	t_sem_print;		// @param enum_format SEM_PRINT_*

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
 * @param monitor_id The pid of the forked proccess to the monitor.
 * @param sem_death A semaphore linked to death_flag.
 * @param sem_print A semaphore linked to printf.
 * @param forks A pointer to a PTHREAD_T ARRAY to the forks in table order.
 * @param sem_meals A pointer to a SEM_T ARRAY to protect timestamps of meals.
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
	pid_t			monitor_id;
	sem_t			*sem_death;
	sem_t			*sem_print;
	sem_t			*sem_forks;
	sem_t			**sem_meals;	//double cuz is one semaphore per philosopher 
}	t_rules;

/**
 * Struct used to save the information of a philosopher.
 * 
 * @param id The identification number of the philosopher.
 * @param meals_eaten The amount of meals the philosopher had eaten.
 * @param last_meal_time The time the philosopher finished its last meal.
 * @param thead_id The identification thread of the philosopher.
 * @param left_fork A pointer to a fork mutex to the left of the philosopher.  
 * @param right_fork A pointer to a fork mutex to the right of the philosopher.
 * @param rules A pointer to the experiment main T_RULES structure.
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
	pid_t			pid_id;
	sem_t			*sem_meal;
	t_rules			*table;
}	t_philosopher;

/**
 * Struct used to save all the information of the experiment as a whole.
 * 
 * @param table A pointer to the main T_RULES structure.
 * @param seats A pointer to the T_PHILOSOPHER structure array.
 */
typedef	struct s_monitor
{
	t_rules 		*table;
	t_philosopher	*seats;
}	t_monitor;

/*--------------------------------------------------------------------------*/
/*----------------------------- INITIALIZATION -----------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Initializes all necessary semaphores to run the philosopher program.
 * 
 * 	- Death semaphore: Used to avoid data races with the death flag.
 *
 * 	- Print semaphore: Used to avoid data races with the print flag.
 * 
 * 	- Forks semaphore: Used to avoid data races, and follow remaining forks.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 */
int	initialize_semaphore(t_rules *table)

/**
 * Destroys every mutex created to run the philosopher program.
 * @param table The main enviroment philosopher structure.
 * @note If any mutex destruction fails, an error message is
 * printed on screen detailing the failed mutex, the program
 * then exits with forcend(4).
 */
int	sem_close(t_rules *table);

/**
 * Initializes all necesary threads to run the philosopher program.
 * The amount of threads created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 * @return A pointer to the allocated array of T_PHILOSOPHERS.
 */
int			start_philosophical_experiment(t_rules *table, t_philosopher **seats);

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
int			to_death_flag(t_rules *table, t_mtx_flag action);

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
int			to_meals_value(t_philosopher *seat, t_mtx_flag action);

/**
 * Using the T_MTX_PRINT enum, prints a specific message on STDOUT linked to
 * the pointer to a T_PHILOSOPHER seat, avoiding print chunks using a mutex.
 * 
 * @param seat A pointer to the T_PHILOSOPHER linked to the message.
 * @param action The action to perform into death_flag.
 */
void		to_print_access(t_philosopher *seat, t_mtx_print action);

/*--------------------------------------------------------------------------*/
/*---------------------------------- UTILS ---------------------------------*/
/*--------------------------------------------------------------------------*/

void		*monitorize(void *arg);

void		*philo(void *arg);

int	cronometer(struct timeval last_meal, long ms_death);

/**
 * Finds the first number on a STRING with a decimal base.
 * @param str The string where the base number is saved.
 * @param base The base in which the number must be found.
 * @return The decimal INT found on STR.
 * @note A number can, but is not forced to, start with any
 * amount of spaces and one sign; but the next character must 
 * be a digit, if not or str doesn't exists, returns 0.
 */
int			ft_atoi(const char *nptr);

/**
 * Philosopher failsafe, in case of error, frees all memory that could remain
 * allocated in the main structure, and destroy any created mutex.
 * @param table A pointer to the main environment philosopher structure.
 * @param errmsg The error number which points to its error string.
 */
int		forcend(t_rules *table, t_philosopher *chairs, int errmsg);

#endif