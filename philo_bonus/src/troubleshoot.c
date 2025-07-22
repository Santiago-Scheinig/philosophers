/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   troubleshoot.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 12:00:38 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 18:36:21 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

void	split_free(char **split)
{
	int	i;

	i = -1;
	while (split[++i])
		free(split[i]);
	free(split);	
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
		"FATAL: Thread join failed",
		"proccess creation failed",
		"proccess killing execution failed",
		"waitpid of child proccess failed",
		"memory allocation failed",
	};
	if (errmsg)//PH_THD_JERR || PH_PID_MERR || PH_THD_EERR || PH_KILL_ERR are FATAL errors.
		printf("ERROR: %s\n", msg[errmsg]);
	if (table)
	{
		if (table->sem_philo)
		{
			close_semaphores(table);
			unlink_semaphores(table);
			free(table->sem_philo);
			table->sem_philo = NULL;
		}
		if (table->pid_id)
			free(table->pid_id);
		if (table->sem_names)
			split_free(table->sem_names);
	}
	exit(errmsg);
}
