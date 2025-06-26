/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 17:09:12 by sscheini          #+#    #+#             */
/*   Updated: 2025/06/26 17:10:55 by sscheini         ###   ########.fr       */
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
	{
		ans = 1;
		printf("Error: Unable to destroy %s mutex\n", "death");
	}
	if (pthread_mutex_destroy(&(table->print_mutex)) != 0)
	{
		ans = 1;
		printf("Error: Unable to destroy %s mutex\n", "print");
	}
	while (++i < table->n_forks)
		if (pthread_mutex_destroy(&(table->forks[i])) != 0)
			ans = printf("Error: Unable to destroy fork mutex %d\n", i);
	free(table->forks);
	table->forks = NULL;
	if (ans)
		forcend(table, NULL, PH_MUTEXDERR);
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
		forcend(NULL, NULL, PH_MUTEXIERR);
	if (pthread_mutex_init(&(table->print_mutex), NULL) != 0)
	{
		if (pthread_mutex_destroy(&(table->death_mutex)))
			printf("Error: Unable to destroy mutex\n");
		forcend(table, NULL, PH_MUTEXIERR);
	}
	table->forks = malloc(sizeof(pthread_mutex_t) * table->n_philo);
	if (!table->forks)
		forcend(table, NULL, PH_MEMFAIL);
	i = -1;
	while (++i < table->n_philo)
	{
		if (pthread_mutex_init(&(table->forks[i]), NULL) != 0)
			forcend(table, NULL, PH_MUTEXIERR);
		table->n_forks++;
	}
}

/**
 * Initializes the monitor thread, which saves al the information of
 * the program so it can proper run the experiment.
 * @param table A pointer to the main enviroment philosopher structure.
 * @param chairs A pointer to the array of T_PHILOSOPHERS.
 * @note Once the monitor is correctly created, it's rutine will start
 * the experiment.
 */
static void	create_monitor(t_rules *table, t_philosopher *chairs)
{
	t_monitor	*waiter;
	
	waiter = malloc(sizeof(waiter));
	if (!waiter)
		forcend(table, chairs, PH_MEMFAIL);
	waiter->table = table;
	waiter->chairs = chairs;
	if (pthread_create(&(waiter->table->monitor_id), NULL, /*routine*/, waiter))
	{
		set_death_flag(table, 1);
		forcend(table, chairs, PH_THREADERR);
	}
}

/**
 * Initializes all necesary threads to run the philosopher program.
 * The amount of threads created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 * @return A pointer to the allocated array of T_PHILOSOPHERS.
 */
t_philosopher	*start_philosophical_experiment(t_rules *table)
{
	t_philosopher	*chairs;
	int				i;

	chairs = malloc(table->n_philo * sizeof(t_philosopher));
	if (!chairs)
		forcend(table, NULL, PH_MEMFAIL);
	i = 0;
	while (i < table->n_philo)
	{
		memset(&chairs[i], 0, sizeof(t_philosopher));
		chairs[i].id = i;
		chairs[i].left_fork = &(table->forks[i]);
		chairs[i].right_fork = &(table->forks[(i + 1) % table->n_forks]);
		chairs[i].rules = table;
		if (pthread_create(&(chairs[i].thread_id), NULL, /*routine*/, &chairs[i]))
		{
			set_death_flag(table, 1);
			forcend(table, chairs, PH_THREADERR);
		}
		if (pthread_detach(chairs[i].thread_id))
			printf("Error: Unable to detatch %i philosopher thread\n", i);
		i++;
	}
	create_monitor(table, chairs);
	return (chairs);
}