/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_reset.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 17:01:44 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/17 18:29:11 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

static char	*get_sem_name(char *name, int id)
{
	const char	*base_name = "/philo_";
	int			name_len;
	int			i;

	i = -1;
	while (base_name[++i])
		name[i] = base_name[i];
	name_len = i;
	while (id >= 10)
	{
		name[name_len] = (id % 10) + 48;
		id /= 10;
		name_len++;
	}
	name[name_len] = (id + 48);
	return (name);
}

int main(int argc, char **argv)
{
	char	*name;
	int		n_philo;
	int		ans;
	int		i;

	ans = 0;
	if (argc != 2)
		return (printf("Error: invalid amount of arguments"));
	n_philo = atoi(argv[1]);
	sem_unlink("/death");
	sem_unlink("/print");
	sem_unlink("/forks");
	sem_unlink("/start");
	sem_unlink("/ready");
	i = -1;
	while (++i < n_philo + 1)
	{
		name = malloc(20 * sizeof(char));
		name = get_sem_name(name, i);
		sem_unlink(name);
		free(name);
	}
}