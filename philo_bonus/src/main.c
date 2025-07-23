/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:59:02 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/23 17:31:09 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

/**
 * Verifies user input to not include the following:
 * 
 * - Empty arguments.
 * 
 * - Non numeric characters.
 * 
 * If any of those is found, returns PH_ARG_VINV. Else returns 0.
 */
static int	check_arg_validity(char **argv)
{
	int	i;
	int	j;

	i = 0;
	while (argv[++i])
	{
		if (!argv[i][0])
			return (PH_ARG_VINV);
		j = -1;
		while (argv[i][++j])
			if (!argv[i][j] || (argv[i][j] < '0' || argv[i][j] > '9'))
				return (PH_ARG_VINV);
	}
	return (PH_SUCCESS);
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
		forcend(NULL, PH_ARG_CINV);
	if (check_arg_validity(argv))
		forcend(NULL, PH_ARG_VINV);
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

	check_args(argc, argv, &table);
	initialize_semaphores(&table);
	initialize_dinner(&table);
}
