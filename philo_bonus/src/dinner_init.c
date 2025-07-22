/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dinner_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 19:36:30 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 21:03:06 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

/**
 * Rejoins the monitor local threads, saving their exit status.
 * 
 * @param thd_a The first of the local threads. 
 * @param thd_b The second of the local threads.
 * @param thd_c The third of the local threads.
 * @return The exit status of the threads.
 */
static int	join_threads(pthread_t thd_a, pthread_t thd_b, pthread_t thd_c)
{
	void		*ret;

	ret = NULL;
	if (pthread_join(thd_a, &ret) != 0)
		return (PH_THD_JERR);
	if (ret)
		return ((__intptr_t)ret);
	ret = NULL;
	if (pthread_join(thd_b, &ret) != 0)
		return (PH_THD_JERR);
	if (ret)
		return ((__intptr_t)ret);
	ret = NULL;
	if (pthread_join(thd_c, &ret) != 0)
		return (PH_THD_JERR);
	if (ret)
		return ((__intptr_t)ret);
	return (PH_SUCCESS);
}

/**
 * Using waitpid(), the monitor waits for any signal send by a child proccess
 * and register it's status.
 * 
 * - SIGTERM signal: means the proccess ended correctly.
 * 
 * - SIGKILL signal or EXIT status: means the proccess ended on error.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * @param status The current status of the dinner.
 * @return 0 on success. On any error, the program returns the error code.
 * @note If the current status of the dinner is different than 0, means an
 * error occured, the monitor will wait for all created proccesses to end.
 */
static int	monitor_philo(t_rules *table, int status)
{
	int	sig;
	int	code;

	if (status)
	{
		while (waitpid(0, NULL, 0) != -1)
			usleep(100);
		return (status);
	}
	while (waitpid(0, &status, 0) != -1)
	{
		if (WIFSIGNALED(status))
		{
			sig = WTERMSIG(status);
			if (sig != SIGTERM)
				return (monitor_philo(table, PH_THD_EERR));
		}
		else if (WIFEXITED(status))
		{
			code = WEXITSTATUS(status);
			if (code != PH_SUCCESS)
				return (monitor_philo(table, try_exit_and_kill(table, code)));
		}
	}
	return (PH_SUCCESS);
}

/** 
 * Initializes the philosopher proccesses.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * 
 * @return 0 on success. On error, the specific errcode. 
 * @note If a philosopher initialization fails, a death semaphore is signaled
 * to end all proccesses previously allocated via local threads on the monitor.
*/
static int	create_philosophers(t_rules *table)
{
	t_philosopher	seat;
	int				i;

	memset(&seat, 0, sizeof(t_philosopher));
	seat.table = table;
	i = -1;
	while (++i < table->n_philo)
	{
		seat.id = i + 1;
		seat.sem_name = table->sem_names[seat.id];
		seat.sem_philo = table->sem_philo[seat.id];
		table->pid_id[seat.id] = fork();
		if (table->pid_id[seat.id] < 0)
		{
			if (safe_sem_post(table->sem_death, "/death", -1))
				return (try_exit_and_kill(table, PH_SEM_PERR));
		}
		if (!table->pid_id[seat.id])
			philosophize(&seat);
	}
	return (PH_SUCCESS);
}

/**
 * Initializes the monitor proccess, which verifies several semaphores linked
 * to the program stability.
 * 
 * Once the monitor is correctly created, it's proccess will create the
 * philosopher proccesses before starting the dinner.
 * 
 * The monitor will then wait on all the child proccessess and save their
 * signal ending.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * @note If something fails, either with the child proccesses or the local
 * threads, the error code is saved on status and returned to main, after
 * killing all possible proccesses the safest way possible, and waitpid()
 * them.
 */
static void	create_monitor(t_rules *table)
{
	pthread_t	thd_death_id;
	pthread_t	thd_meals_id;
	pthread_t	thd_start_id;
	int			pcs_status;
	int			thd_status;

	table->pid_id[0] = fork();
	if (table->pid_id[0] < 0)
		forcend(table, PH_PCS_CERR);
	if (!table->pid_id[0])
	{
		pcs_status = create_philosophers(table);
		if (pthread_create(&(thd_death_id), NULL, monitor_death, table))
			exit(PH_THD_CERR);
		if (pthread_create(&(thd_meals_id), NULL, monitor_meals, table))
			exit(PH_THD_CERR);
		if (pthread_create(&(thd_start_id), NULL, monitor_start, table))
			exit(PH_THD_CERR);
		pcs_status = monitor_philo(table, pcs_status);
		thd_status = join_threads(thd_death_id, thd_meals_id, thd_start_id);
		if (pcs_status)
			exit(pcs_status);
		exit(thd_status);
	}
}

/**
 * Initializes all necesary proccesses to run the philosopher program.
 * The amount created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note Once all initialization is done, it will wait for the monitor
 * proccessor to end the program.
 */
void	initialize_dinner(t_rules *table)
{
	int	status;

	table->pid_id = malloc((table->n_philo + 2) * sizeof(pid_t));
	if (!table->pid_id)
		forcend(table, PH_MEM_AERR);
	memset(table->pid_id, -1, (table->n_philo + 2) * sizeof(pid_t));
	create_monitor(table);
	if (waitpid(table->pid_id[0], &status, 0) == -1)
		forcend(table, PH_PID_MERR);
	if (WIFSIGNALED(status))
		forcend(table, PH_THD_EERR);
	if (WIFEXITED(status))
		forcend(table, status);
}
