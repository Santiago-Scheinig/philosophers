/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 19:36:30 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/10 18:48:32 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * Close every semaphore created to run the philosopher program.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note If any semaphore destruction fails, an error message is
 * printed on screen detailing the failed mutex, the program
 * then exits with forcend(4).
 */
int	close_semaphores(t_rules *table)
{
	int	i;
	int ans;

	ans = 0;
	i = -1;
	if (sem_close(table->sem_death) != 0 || sem_unlink("/death") != 0)
		ans += printf("Error: Unable to close %s semaphore\n", "/death");
	if (sem_close(table->sem_print) != 0 || sem_unlink("/print") != 0)
		ans += printf("Error: Unable to close %s semaphore\n", "/print");
	if (sem_close(table->sem_forks) != 0 || sem_unlink("/forks") != 0)
		ans += printf("Error: Unable to close %s semaphore\n", "/print");
	while (++i < table->n_forks)
	{
		if (sem_close(&(table->sem_meals[i])) != 0 
		|| sem_unlink(&(table->sem_name[i])) != 0)
			ans += printf("Error: Unable to close meals semaphore %d\n", i);
	}
	if (table->sem_meals)
		free(table->sem_meals);
	if (ans)
		return (forcend(table, NULL, PH_SEM_DERR));
	return (PH_SUCCESS);
}

/**
 * Initializes all necessary semaphores to run the philosopher program.
 * 
 * 	- Death semaphore: Used to avoid data races with the death flag.
 *
 * 	- Print semaphore: Used to avoid data races with the print flag.
 * 
 * 	- Forks semaphore: Used to avoid data races, and follow remaining forks.
 * 
 *	- Meals semaphore: Used to avoid data races with the meals flag.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 */
int	initialize_semaphore(t_rules *table)
{
	int	forks;
	int	i;

	forks = table->n_philo;
	table->sem_death = sem_open("/death", O_CREAT | O_EXCL, 0644, 1);
	if (table->sem_death == SEM_FAILED)
		return (forcend(NULL, NULL, PH_SEM_IERR));
	table->sem_print = sem_open("/print", O_CREAT | O_EXCL, 0644, 1);
	if (table->sem_print == SEM_FAILED)
		return (forcend(table, NULL, PH_SEM_IERR));
	table->sem_forks = sem_open("/forks", O_CREAT | O_EXCL, 0644, forks / 2);
	if (table->sem_forks == SEM_FAILED)
		return (forcend(table, NULL, PH_SEM_IERR));
	table->sem_meals = malloc(sizeof(sem_t) * table->n_philo);
	if (!table->sem_meals)
		return (forcend(table, NULL, PH_MEM_AERR));
	i = -1;
	while (++i < table->n_philo)
	{
		table->sem_meals[i] = sem_open(sem_name[i], O_CREAT | O_EXCL, 0644, 1);
		if (table->sem_meals[i] == SEM_FAILED)
			return (forcend(table, NULL, PH_SEM_IERR));
	}
	return (PH_SUCCESS);
}

/**
 * Initializes the monitor thread, which saves al the information of
 * the program so it can proper run the experiment.
 * @param table A pointer to the main enviroment philosopher structure.
 * @param seats A pointer to the array of T_PHILOSOPHERS.
 * @note Once the monitor is correctly created, it's rutine will start
 * the experiment.
 */
static int create_monitor(t_rules *table, t_philosopher *seats)
{
	int			pid;
	t_monitor	*waiter;
	
	pid = fork();
	if (pid)
	{
		waiter = malloc(sizeof(t_monitor));
		if (!waiter)
			forcend(table, seats, PH_MEM_AERR);
		waiter->table = table;
		waiter->seats = seats;
		if (pthread_create(&(waiter->table->monitor_id), NULL, monitorize, waiter))
		{
			to_death_flag(table, MTX_FLAG_ON);
			forcend(table, seats, PH_THD_CERR);
		}
		return (PH_SUCCESS);
	}
}
	
/**
 * Initializes all necesary threads to run the philosopher program.
 * The amount of threads created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 * @return A pointer to the allocated array of T_PHILOSOPHERS.
 */
int	start_philosophical_experiment(t_rules *table, t_philosopher **seats)
{
	int	pid;
	int	i;

	(*seats) = malloc(table->n_philo * sizeof(t_philosopher));
	if (!(*seats))
		return (forcend(table, NULL, PH_MEM_AERR));
	i = -1;
	while (++i < table->n_philo)
	{
		memset(&(*seats)[i], 0, sizeof(t_philosopher));
		(*seats)[i].id = i + 1;
		(*seats)[i].sem_meal = &(table->sem_meals[i]);
		(*seats)[i].table = table;
		pid = fork();
		if (pid < 0)
		{
			to_death_flag(table, SEM_FLAG_ON);
			return (forcend(table, (*seats), PH_PCS_CERR));
		}
		if (pid)
			return (/*philo(table, (*seats)[i])philosopher process*/);
	}
	return (create_monitor(table, (*seats)));
}
