/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:59:02 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/16 18:33:37 by sscheini         ###   ########.fr       */
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
		"Invalid amount of arguments.",
		"Invalid arguments.",
		"Semaphore initialization failed.",
		"Semaphore unlink failed.",
		"Semaphore destruction failed.",
		"Function sem_post() execution failed."
		"Function sem_wait() execution failed."
		"Thread creation failed.",
		"Proccess creation failed.",
		"Proccess killing execution failed",
		"Waitpid of child proccess failed.",
		"Memory allocation failed.",
	};
	if (table)
	{
		if (table->sem_philo)
		{
			close_semaphores(table);
			unlink_semaphores(table);
			free(table->sem_philo);
			table->sem_philo = NULL;
		}
	}
	if (table->pid_id)
		free(table->pid_id);
	if (table->sem_names)
		split_free(table->sem_names);
	if (errmsg)
		printf("ERROR: %s\n", msg[errmsg]);
	exit(errmsg);
}

/**
 * Verifies user input and initializes the main enviroment structure with
 * all its data.
 * 
 * If either "n_philo" or "time_to_die" are 0, or if any other input 
 * number is negative, then the program exits with forcend(2). If the amount
 * of arguments isn't valid, the program exits with forcend(1).
 * 
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
		return (forcend(NULL, PH_ARG_CINV));
	i = 0;
	memset(table, 0, sizeof(t_rules));
	table->n_philo = ft_atoi(argv[++i]);
	if (table->n_philo <= 0)
		forcend(NULL, PH_ARG_VINV);
	table->time_to_die = ft_atoi(argv[++i]);
	if (table->time_to_die < 0)
		forcend(NULL, PH_ARG_VINV);
	table->time_to_eat = ft_atoi(argv[++i]);
	if (table->time_to_eat < 0)
		forcend(NULL, PH_ARG_VINV);
	table->time_to_sleep = ft_atoi(argv[++i]);
	if (table->time_to_sleep < 0)
		forcend(NULL, PH_ARG_VINV);
	table->meals_required = ft_atoi(argv[++i]);
	if (table->meals_required < 0)
		forcend(NULL, PH_ARG_VINV);
}

int	main(int argc, char **argv)
{
	t_rules	table;
	int		i;

	check_args(argc, argv, &table);
	initialize_semaphores(&table);
 	initialize_dinner(&table);
	forcend(&table, PH_SUCCESS);
}
