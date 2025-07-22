/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_sem.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:58:40 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 21:00:55 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_SEM_H
# define PHILO_SEM_H

# include <sys/time.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <stdio.h>
# include <signal.h>
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
typedef enum e_philo_errno
{
	PH_SUCCESS,				// Success
	PH_ARG_CINV,			// Amount of arguments isn't valid
	PH_ARG_VINV,			// Arguments aren't valid
	PH_SEM_IERR,			// Semaphore initialization failed
	PH_SEM_UERR,			// Semaphore unlink failed
	PH_SEM_DERR,			// Semaphore destruction failed
	PH_SEM_PERR,			// Semaphore sem_post execution failed
	PH_SEM_WERR,			// Semaphore sem_wait execution failed
	PH_THD_CERR,			// Thread creation failed
	PH_THD_EERR,			// Thread execution failed
	PH_THD_JERR,			// Thread join failed
	PH_PCS_CERR,			// Proccess creation failed
	PH_PCS_KERR,			// Proccess killing execution failed
	PH_PID_MERR,			// Waitpid for monitor failed
	PH_MEM_AERR,			// Memory allocation failed
	PH_KILL_ERR,			// Unable to kill created proccesses
}	t_philo_errno;			// @param enum_format PH_*

/**
 * An enumeration list of time codes to identify specific actions to perform
 * inside of a mutexed timestamp.
 */
typedef enum e_sem_time
{
	SEM_TIME_IS	= 0,	// Returns timestamp depending on eating flag value
	SEM_TIME_ISFULL,	// To modify the eating flag to 0
	SEM_TIME_ISEATING,	// To modify the eating flag to 1
}	t_sem_time;			// @param enum_format SEM_TIME_*

/**
 * An enumeration list of print codes to identify specific messages to print
 * on STDOUT filtered by a mutex.
 */
typedef enum e_sem_print
{
	SEM_PRINT_FORK = 0,	// Prints "[ts_in_ms] [philo_id] has taken a fork."
	SEM_PRINT_EAT,		// Prints "[ts_in_ms] [philo_id] is eating."
	SEM_PRINT_SLEEP,	// Prints "[ts_in_ms] [philo_id] is sleeping."
	SEM_PRINT_THINK,	// Prints "[ts_in_ms] [philo_id] is thinking."
	SEM_PRINT_DEATH,	// Prints "[ts_in_ms] [philo_id] died."
}	t_sem_print;		// @param enum_format SEM_PRINT_*

/*--------------------------------------------------------------------------*/
/*------------------------------- STRUCTURES -------------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Struct used to save the rules of the experiment.
 * 
 * @param n_philo The amount of philosophers that will seat on the table.
 * @param exit_flag
 * @param time_to_die The time limit a philosopher can be without eating.
 * @param time_to_eat The time it takes a philosopher to eat.
 * @param time_to_sleep The time it takes a philosopher to sleep.
 * @param meals_required The amount of meals each philosopher must eat.
 * @param pid_id The pid of the forked proccess to the monitor.
 * @param sem_death A semaphore linked to death_flag.
 * @param sem_print A semaphore linked to printf.
 * @param sem_forks A pointer to a PTHREAD_T ARRAY to the forks in table order.
 * @param sem_start
 * @param sem_ready
 * @param sem_philo A pointer to a SEM_T ARRAY to protect timestamps of meals.
 * @param sem_names 
 */
typedef struct s_rules
{
	int				clean_up;
	int				n_philo;
	int				exit_flag;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				meals_required;
	pid_t			*pid_id;
	sem_t			*sem_death;
	sem_t			*sem_print;
	sem_t			*sem_forks;
	sem_t			*sem_start;
	sem_t			*sem_ready;
	sem_t			**sem_philo;
	char			**sem_names;
}	t_rules;

/**
 * Struct used to save the information of a philosopher.
 * 
 * @param id The identification number of the philosopher.
 * @param is_eating A flag to follow if the philosopher is eating or not.
 * @param meals_eaten The amount of meals the philosopher had eaten.
 * @param max_meals_achieved
 * @param start_time
 * @param last_meal_time The time the philosopher finished its last meal.
 * @param sem_name A pointer to a mutex linked to all meal interactions.
 * @param sem_philo
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
	int				is_eating;
	int				meals_eaten;
	int				max_meals_achieved;
	struct timeval	start_time;
	struct timeval	last_meal_time;
	char			*sem_name;
	sem_t			*sem_philo;
	t_rules			*table;
}	t_philosopher;

/*--------------------------------------------------------------------------*/
/*----------------------------- INITIALIZATION -----------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Initializes all necessary semaphores to run the philosopher program.
 * 
 * 	- Death semaphore: Used to follow-up proccesses death.
 *
 * 	- Print semaphore: Used to avoid write interleaving between proccesses.
 * 
 * 	- Forks semaphore: Used to follow-up remaining forks.
 * 
 *	- Start semaphore: Used to link the start of philosopher proccesses.
 * 
 *	- Philo semaphore: Used to avoid data races with philosopher threads.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 */
void			initialize_semaphores(t_rules *table);

/**
 * Unlinks every semaphore created to run the philosopher program.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * 
 * @note If any semaphore unlink fails, an error message is
 * printed on screen detailing the failed semaphore, the program
 * then exits with forcend(4).
 */
void			unlink_semaphores(t_rules *table);

/**
 * Close every semaphore created to run the philosopher program.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * 
 * @note If any semaphore close fails, an error message is
 * printed on screen detailing the failed semaphore, the program
 * then exits with forcend(5).
 */
void			close_semaphores(t_rules *table);

/**
 * Initializes all necesary proccesses to run the philosopher program.
 * The amount created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note Once all initialization is done, it will wait for the monitor
 * proccessor to end the program.
 */
void			initialize_dinner(t_rules *table);

/**
 * The philosopher proccess routine.
 * 
 * - Sets the [last_meal_time] to [start_time].
 * 
 * - If odd, waits ([time_to_eat] / 2) to create eating waves.
 * 
 * - A personal thread checks regulary if proccess died by starvation.
 * 
 * - Thinks, eats and sleeps.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the last_meal
 * timestamp variable.
 * @note If [n_philosopher] it's an odd amount, uses is_hungry() to set 
 * priorities between waves.
 */
void			philosophize(t_philosopher *seat);

/*--------------------------------------------------------------------------*/
/*---------------------------- THREADS_ROUTINES ----------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * A monitor local thread that waits on the death semaphore, ready to kill
 * every other child proccess if any of them dies.
 * 
 * @param arg A pointer to the main philosopher structure.
 * @return A pointer to an errcode if any.
 * @note Failures on this local thread are consider sensitive.
 */
void			*monitor_death(void *arg);

/**
 * A monitor local thread that waits on the meals semaphore, ready to kill
 * every other child proccess once the max meals quota is accomplished.
 * 
 * @param arg A pointer to the main philosopher structure.
 * @return A pointer to an errcode if any.
 * @note Failures on this local thread are consider sensitive.
 */
void			*monitor_meals(void *arg);

/**
 * A monitor local thread that waits on the ready semaphore, used to
 * wait for every proccess to be ready, before starting the experiment.
 * 
 * @param arg A pointer to the main philosopher structure.
 * @return A pointer to an errcode if any.
 * @note Failures on this local thread are consider sensitive.
 */
void			*monitor_start(void *arg);

/**
 * A philosopher local thread used to verify [last_meal_time] and
 * [time_to_die] difference, setting the [sem_death] to 1 as well
 * as blocking the [sem_print] in order to prevent further writing
 * on STDIN and signal the monitor for it's own death.
 * 
 * @param arg A pointer to the main philosopher structure.
 * @return NULL since this thread it's detatched.
 */
void			*philo_death(void *arg);

/*--------------------------------------------------------------------------*/
/*---------------------------- SEMAPHORE_ACTIONS ---------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Using the t_mtx_time enum, executes instructions on the last_meal timestamp
 * variable, saved on the T_PHILOSOPHER structure, avoiding data races using
 * a mutex.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the last_meal
 * timestamp variable.
 * @param action The action to perfrom into meals_eaten
 */
struct timeval	to_time_value(t_philosopher *seat, t_sem_time action);

/**
 * Using the T_MTX_PRINT enum, prints a specific message on STDOUT linked to
 * the pointer to a T_PHILOSOPHER seat, avoiding print chunks using a mutex.
 * 
 * @param seat A pointer to the T_PHILOSOPHER linked to the message.
 * @param action The action to perform into death_flag.
 */
void			to_print_access(t_philosopher *seat, t_sem_print action);

/*--------------------------------------------------------------------------*/
/*------------------------- SEMAPHORE_TROUBLESHOOT -------------------------*/
/*--------------------------------------------------------------------------*/

/**
 * Failsafe for sem_wait().
 * 
 * @param sem A pointer to the semaphore to wait.
 * @param sem_name The name of the semaphore to wait.
 * @return 0 on success. On error, the specific errcode.
 * @note If sem_wait() fails, an error log is printed on STDOUT.
 */
int				safe_sem_wait(sem_t *sem, const char *sem_name, int id);

/**
 * Failsafe for sem_post().
 * 
 * @param sem A pointer to the semaphore to post.
 * @param sem_name The name of the semaphore to post.
 * @return 0 on success. On error, the specific errcode.
 * @note If sem_post() fails, an error log is printed on STDOUT.
 */
int				safe_sem_post(sem_t *sem, const char *sem_name, int id);

/**
 * Cleanly kills all child proccesses with SEGTERM.
 * 
 * @param table A pointer to the main philosopher structure.
 * @param errcode The errcode that triggered the exit and kill proccess.
 * @return The errcode that triggered the exit and kill proccess.
 * @note Any error occured during this execution it's consider sensitive, and
 * will execute kill_all() as a countermeasure.
 */
int				try_exit_and_kill(t_rules *table, int errcode);

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
 * Cleans up all allocated memory after failing during initialization, as well
 * as close and unlink any created semaphores until that moment.
 * 
 * @param table A pointer to the main enviroment philosopher strucutre.
 * @param i The previous index before the initialization failed.
 * @note This execution also terminates the program. 
 */
void			cleanup_philo_sem(t_rules *table, int i);

/**
 * Philosopher failsafe, in case of error, frees all memory that could remain
 * allocated in the main structure, and destroy any created mutex.
 * @param table A pointer to the main environment philosopher structure.
 * @param errmsg The error number which points to its error string.
 */
void			forcend(t_rules *table, int errmsg);

#endif