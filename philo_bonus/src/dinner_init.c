/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dinner_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 19:36:30 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/17 18:55:08 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

static int	join_threads(pthread_t thd_a, pthread_t thd_b,  pthread_t thd_c)
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

static int	monitor_philo(int status)
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
				return (PH_THD_EERR);
		}
		else if (WIFEXITED(status))
		{
			code = WEXITSTATUS(status);
			if (code != PH_SUCCESS)
				return (code);
		}
	}
	return (PH_SUCCESS);
}

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
			if (safe_sem_post(table->sem_death, "/death"))
				return (try_exit_and_kill(table, PH_SEM_PERR));
		}
		if (!table->pid_id[seat.id])
			philosophize(table, &seat);
	}
	return (PH_SUCCESS);
}

/**
 * Initializes the monitor proccess, which verifies several semaphores linked
 * to the program stability.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note Once the monitor is correctly created, it's proccess will start
 * the dinner.
 */
static void	create_monitor(t_rules *table)
{
	pthread_t	thd_death_id;
	pthread_t	thd_meals_id;
	pthread_t	thd_start_id;
	int			status = 0;
	
	table->pid_id[0] = fork();
	if (table->pid_id[0] < 0)
		forcend(table, PH_PCS_CERR);
	if (!table->pid_id[0])
	{
		status = create_philosophers(table);
		if (pthread_create(&(thd_death_id), NULL, monitor_death, table))
			exit(PH_THD_CERR);
		if (pthread_create(&(thd_meals_id), NULL, monitor_meals, table))
			exit(PH_THD_CERR);
		if (pthread_create(&(thd_start_id), NULL, monitor_start, table))
			exit(PH_THD_CERR);
		status = monitor_philo(status);
		if (status && status != PH_THD_EERR)
			exit (status);
		status = join_threads(thd_death_id, thd_meals_id, thd_start_id);
		if (status)
			exit(status);
		exit(PH_SUCCESS);
	}
}	

/**
 * Initializes all necesary proccesses to run the philosopher program.
 * The amount created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
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
		printf("FATAL: can't wait for monitor, dont know if something went wrong or not");
	if (WIFSIGNALED(status))
		forcend(table, PH_FATAL_ERROR);
	if (WIFEXITED(status))
		forcend(table, status);
}
