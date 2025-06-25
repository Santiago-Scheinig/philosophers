/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:58:40 by sscheini          #+#    #+#             */
/*   Updated: 2025/06/19 18:50:51 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

typedef struct s_rules
{
	int				n_philo;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				meals_required;
	long long		start_time;
	int				death_flag;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t semaphore_mutex;
}	t_rules;

typedef struct s_philosopher
{
	int				id;
	int				has_forks;
	int				meals_eaten;
	long long		last_meal_time;
	t_rules			rules;
}	t_philosopher;