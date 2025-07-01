/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 17:09:12 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/01 21:00:43 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * Destroys every mutex created to run the philosopher program.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note If any mutex destruction fails, an error message is
 * printed on screen detailing the failed mutex, the program
 * then exits with forcend(4).
 */
void	destroy_mutex(t_rules *table)
{
	int	i;
	int ans;

	ans = 0;
	i = -1;
	if (pthread_mutex_destroy(&(table->death_mutex)) != 0)
		ans += printf("Error: Unable to destroy %s mutex\n", "death");
	if (pthread_mutex_destroy(&(table->print_mutex)) != 0)
		ans += printf("Error: Unable to destroy %s mutex\n", "print");
	while (++i < table->n_forks)
	{
		if (table->forks && pthread_mutex_destroy(&(table->forks[i])) != 0)
			ans += printf("Error: Unable to destroy fork mutex %d\n", i); 
		if (table->meals && pthread_mutex_destroy(&(table->meals[i])) != 0)
			ans += printf("Error: Unable to destroy fork mutex %d\n", i);
	}
	if (&(table->forks))
		free(table->forks);
	if (&(table->meals))
		free(table->meals);
	table->forks = NULL;
	table->meals = NULL;
	if (ans)
		forcend(table, NULL, PH_MTX_DERR);
}

/**
 * Initializes all necessary mutex to run the philosopher program.
 * 
 * 	- Death mutex: Used to avoid data races with the death flag.
 *
 * 	- Print mutex: Used to avoid data races with the print flag.
 * 
 * 	- Forks mutex: Used to avoid data races with forks.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 */
void	initialize_mutex(t_rules *table)
{
	int	i;

	table->n_forks = 0;
	if (pthread_mutex_init(&(table->death_mutex), NULL) != 0)
		forcend(NULL, NULL, PH_MTX_IERR);
	if (pthread_mutex_init(&(table->print_mutex), NULL) != 0)
	{
		if (pthread_mutex_destroy(&(table->death_mutex)))
			printf("Error: Unable to destroy mutex\n");
		forcend(table, NULL, PH_MTX_IERR);
	}
	table->forks = malloc(sizeof(pthread_mutex_t) * table->n_philo);
	table->meals = malloc(sizeof(pthread_mutex_t) * table->n_philo);
	if (!table->meals || !table->forks)
		forcend(table, NULL, PH_MEM_AERR);
	i = -1;
	while (++i < table->n_philo)
	{
		if (pthread_mutex_init(&(table->forks[i]), NULL) != 0)
			forcend(table, NULL, PH_MTX_IERR);
		if (pthread_mutex_init(&(table->meals[i]), NULL) != 0)
			forcend(table, NULL, PH_MTX_IERR);
		table->n_forks++;
	}
}

/**
 * Initializes the monitor thread, which saves al the information of
 * the program so it can proper run the experiment.
 * @param table A pointer to the main enviroment philosopher structure.
 * @param seats A pointer to the array of T_PHILOSOPHERS.
 * @note Once the monitor is correctly created, it's rutine will start
 * the experiment.
 */
static t_monitor	*create_monitor(t_rules *table, t_philosopher *seats)
{
	t_monitor	*waiter;
	
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
	return (waiter);
}
	
/**
 * Initializes all necesary threads to run the philosopher program.
 * The amount of threads created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 * @return A pointer to the allocated array of T_PHILOSOPHERS.
 */
t_monitor	*start_philosophical_experiment(t_rules *table)
{
	t_philosopher	*seats;
	int				i;

	seats = malloc(table->n_philo * sizeof(t_philosopher));
	if (!seats)
		forcend(table, NULL, PH_MEM_AERR);
	i = -1;
	while (++i < table->n_philo)
	{
		memset(&seats[i], 0, sizeof(t_philosopher));
		seats[i].id = i;
		seats[i].meals_eaten = 0;
		seats[i].left_fork = &(table->forks[i]);
		seats[i].right_fork = &(table->forks[(i + 1) % table->n_forks]);
		seats[i].meal_mutex = &(table->meals[i]);
		seats[i].table = table;
		if (pthread_create(&(seats[i].thread_id), NULL, philosophizing, &seats[i]))
		{
			to_death_flag(table, MTX_FLAG_ON);
			forcend(table, seats, PH_THD_CERR);
		}
		if (pthread_detach(seats[i].thread_id))
			printf("Error: Unable to detatch %i philosopher thread\n", i);
	}
	return (create_monitor(table, seats));
}
