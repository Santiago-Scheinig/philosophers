/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:59:05 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/01 21:28:30 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * Philosopher failsafe, in case of error, frees all memory that could remain
 * allocated in the main structure, and destroy any created mutex.
 * @param table A pointer to the main enviroment philosopher structure.
 * @param seats A pointer to the T_PHILOSOPHER structure array.
 * @param errmsg The error number which points to its error string.
 */
void	forcend(t_rules *table, t_philosopher *seats, int errmsg)
{
	const char	*msg[] = {
		"Success",
		"Invalid amount of arguments",
		"Invalid arguments",
		"Mutex initialization failed",
		"Mutex destruction failed",
		"Memory allocation failed",
		"Thread creation failed"
	};
	if (seats)
		free(seats);
	if (table)
		if (&(table->forks) || &(table->meals))
			destroy_mutex(table);
	if (errmsg)
		printf("Error: %s\n", msg[errmsg]);
}

/**
 * Verifies user input and initializes the main enviroment structure with
 * all its data.
 * If either "n_philo" or "time_to_die" are 0, or if any other input 
 * number is negative, then the program exits with forcend(2). If the amount
 * of arguments isn't valid, the program exits with forcend(1).
 * @param argc The amount of main arguments.
 * @param argv An array of STRINGS with all the main arguments.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note An argument that starts with anything other than a number, is read
 * as 0.
 */
static void	check_args(int argc, char **argv, t_rules *table)
{
	int	i;

	if (argc < 5 || argc > 6)
		forcend(NULL, NULL, PH_ARG_CINV);
	i = 0;
	memset(table, 0, sizeof(t_rules));
	table->death_flag = -1;
	table->n_philo = ft_atoi(argv[++i]);
	if (table->n_philo <= 0)
		forcend(NULL, NULL, PH_ARG_VINV);
	table->time_to_die = ft_atoi(argv[++i]);
	if (table->time_to_die < 0)
		forcend(NULL, NULL, PH_ARG_VINV);
	table->time_to_eat = ft_atoi(argv[++i]);
	if (table->time_to_eat < 0)
		forcend(NULL, NULL, PH_ARG_VINV);
	table->time_to_sleep = ft_atoi(argv[++i]);
	if (table->time_to_sleep < 0)
		forcend(NULL, NULL, PH_ARG_VINV);
	table->meals_required = ft_atoi(argv[++i]);
	if (table->meals_required < 0)
		forcend(NULL, NULL, PH_ARG_VINV);
}

int	main(int argc, char **argv)
{
	t_rules			table;
	t_monitor		*waiter;

	check_args(argc, argv, &table);
	initialize_mutex(&table);
	waiter = start_philosophical_experiment(&table);
	pthread_join(table.monitor_id, NULL);
	forcend(waiter->table, waiter->seats, PH_SUCCESS);
	free(waiter);
}
