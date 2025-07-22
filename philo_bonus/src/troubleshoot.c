/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   troubleshoot.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 12:00:38 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 20:40:04 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

/**
 * Cleans up all allocated memory after failing during initialization, as well
 * as close and unlink any created semaphores until that moment.
 * 
 * @param table A pointer to the main enviroment philosopher strucutre.
 * @param i The previous index before the initialization failed.
 * @note This execution also terminates the program. 
 */
void	cleanup_philo_sem(t_rules *table, int i)
{
	while (--i >= 0)
	{
		free(table->sem_names[i]);
		if (sem_close(table->sem_philo[i]))
			printf("sem: unable to close %s\n", table->sem_names[i]);
		if (sem_unlink(table->sem_names[i]))
			printf("sem: unable to unlink %s\n", table->sem_names[i]);
	}
	table->clean_up = 1;
	forcend(table, PH_MEM_AERR);
}

/**
 * Frees an ARRAY of STRINGS.
 * 
 * @param split A pointer to an ARRAY of STRINGS
 */
static void	split_free(char **split)
{
	int	i;

	i = -1;
	while (split[++i])
		free(split[i]);
	free(split);	
}

/**
 * Cleans up all allocated memory used by the program, as well
 * as close and unlink any created semaphores.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * @note if cleanup_philo_sem() was previously called during the
 * program execution, the semaphore free, close and unlink step is skipped.
 */
static void	cleanup_all(t_rules *table)
{
	if (table)
	{
		if (table->sem_philo)
		{
			if (!table->clean_up)
			{
				close_semaphores(table);
				unlink_semaphores(table);
			}
			free(table->sem_philo);
			table->sem_philo = NULL;
		}
		if (table->pid_id)
			free(table->pid_id);
		if (table->sem_names)
			split_free(table->sem_names);
	}
}

/**
 * Prints a specific error following an ARRAY of STRINGS messages.
 * 
 * @param msg A pointer to an ARRAY of STRINGS messages.
 * @param errmssg the index number of the error message.
 * @note PH_THD_JERR - PH_PID_MERR - PH_TDH_EERR - PH_KILL_ERR [errmsg]
 * are printed signaling a FATAL error instead of regular.
 */
static void	print_err(const char *msg[], int errmsg)
{
	if (errmsg)
	{
		if (errmsg == PH_THD_JERR || errmsg == PH_PID_MERR 
		|| errmsg == PH_THD_EERR || errmsg == PH_KILL_ERR)
			printf("FATAL: %s. Force manual exit.\n", msg[errmsg]);
		else
			printf("ERROR: %s\n", msg[errmsg]);
	}
}

/**
 * Philosopher failsafe, in case of error, frees all memory that could remain
 * allocated in the main structure, and closes/unlinks any created semaphore.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * @param errmsg The error number which points to its error string.
 */
void	forcend(t_rules *table, int errmsg)
{
	const char	*msg[] = {
		"success",
		"invalid amount of arguments",
		"invalid arguments",
		"semaphore initialization failed",
		"semaphore unlink failed",
		"semaphore destruction failed",
		"function sem_post() execution failed",
		"function sem_wait() execution failed",
		"thread creation failed",
		"thread execution terminated unexpectecly. Program status unknown",
		"monitor thread join failed. Program status unknown",
		"proccess creation failed",
		"proccess killing execution failed",
		"waitpid for monitor proccess failed. Program status unknown",
		"memory allocation failed",
		"function kill() failed. Unable to terminate failing proccesses.",
	};
	print_err(msg, errmsg);
	cleanup_all(table);
	exit(errmsg);
}
