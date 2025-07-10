/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:59:05 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/10 18:26:02 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * Philosopher failsafe, in case of error, frees all memory that could remain
 * allocated in the main structure, and destroy any created mutex.
 * @param table A pointer to the main enviroment philosopher structure.
 * @param seats A pointer to the T_PHILOSOPHER structure array.
 * @param errmsg The error number which points to its error string.
 * @return In case of error, the value of errmsg. Else, returns 0.
 */
int	forcend(t_rules *table, t_philosopher *seats, int errmsg)
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
			return (destroy_mutex(table));
	if (errmsg)
		printf("Error: %s\n", msg[errmsg]);
	return (errmsg);
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
 * @return Returns 0 on success. In case of error, returns the value of it.
 * @note An argument that starts with anything other than a number, is read
 * as 0.
 */
static int	check_args(int argc, char **argv, t_rules *table)
{
	int	i;

	if (argc < 5 || argc > 6)
		return (forcend(NULL, NULL, PH_ARG_CINV));
	i = 0;
	memset(table, 0, sizeof(t_rules));
	table->death_flag = -1;
	table->n_philo = ft_atoi(argv[++i]);
	if (table->n_philo <= 0)
		return (forcend(NULL, NULL, PH_ARG_VINV));
	table->time_to_die = ft_atoi(argv[++i]);
	if (table->time_to_die < 0)
		return (forcend(NULL, NULL, PH_ARG_VINV));
	table->time_to_eat = ft_atoi(argv[++i]);
	if (table->time_to_eat < 0)
		return (forcend(NULL, NULL, PH_ARG_VINV));
	table->time_to_sleep = ft_atoi(argv[++i]);
	if (table->time_to_sleep < 0)
		return (forcend(NULL, NULL, PH_ARG_VINV));
	table->meals_required = ft_atoi(argv[++i]);
	if (table->meals_required < 0)
		return (forcend(NULL, NULL, PH_ARG_VINV));
	return (PH_SUCCESS);
}

int	main(int argc, char **argv)
{
	t_rules			table;
	t_philosopher	*seats;

	if (check_args(argc, argv, &table))
		return (1);
	if (initialize_mutex(&table))
		return (1);
	if (start_philosophical_experiment(&table, &seats))
		return (1);
	if (pthread_join(table.monitor_id, NULL) != 0)
		printf("Error: Unable to join monitor thread");
	return (forcend(&table, seats, PH_SUCCESS));
}
