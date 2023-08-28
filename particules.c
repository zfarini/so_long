/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particules.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 18:37:57 by zfarini           #+#    #+#             */
/*   Updated: 2023/08/28 16:50:42 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

/* check the particule size calculation */
void	gen_particule_pos_and_vel(t_game *game,
			t_particule *p, t_particule_emitter *e)
{
	if (e->use_dir)
	{
		if (e->dx)
		{
			p->x = e->base_x - e->dx * (rand() % game->cell_dim);
			p->y = e->base_y + (rand() % game->cell_dim - game->cell_dim / 2);
			p->dx = -e->dx;
			p->dy = ((float)rand() / RAND_MAX) * 2 - 1;
		}
		else
		{
			p->x = e->base_x + (rand() % game->cell_dim - game->cell_dim / 2);
			p->y = e->base_y - e->dy * (rand() % game->cell_dim);
			p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
			p->dy = -e->dy;
		}
	}
	else
	{
		p->x = e->base_x + (rand() % (game->cell_dim * 2) - game->cell_dim);
		p->y = e->base_y + (rand() % (game->cell_dim * 2) - game->cell_dim);
		p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
		p->dy = ((float)rand() / RAND_MAX) * 2 - 1;
	}
}

void	gen_particule_attributes(t_game *game,
			t_particule *p, t_particule_emitter *e)
{
	float	l;

	if (e->normalize_dir)
	{
		l = sqrtf(p->dx * p->dx + p->dy * p->dy);
		if (l > 0.1)
		{
			p->dx /= l;
			p->dy /= l;
		}
	}
	p->lifetime = ((float)rand() / RAND_MAX) * e->max_lifetime;
	p->lifetime_left = p->lifetime;
	if (game->cell_dim < 8)
		p->size = 1;
	else
		p->size = rand() % (game->cell_dim / 8) + 1;
	p->r = e->r;
	p->g = e->g;
	p->b = e->b;
}

void	emit_particules(t_game *game, t_particule_emitter *e)
{
	int			i;
	t_particule	*p;

	i = 0;
	while (i < e->count)
	{
		if (game->particule_count >= (int)(sizeof(game->particules)
			/ sizeof(game->particules[0])))
			break ;
		p = &game->particules[game->particule_count++];
		gen_particule_pos_and_vel(game, p, e);
		gen_particule_attributes(game, p, e);
		i++;
	}
}

void	update_and_draw_particule(t_game *game, t_particule *p)
{
	unsigned int	color;

	p->x += p->dx;
	p->y += p->dy;
	color = ((unsigned)((p->lifetime_left / p->lifetime) * 255 + 0.5f) << 24)
		|((unsigned)(p->r * 255 + 0.5f) << 16)
		|((unsigned)(p->g * 255 + 0.5f) << 8)
		|((unsigned)(p->b * 255 + 0.5f) << 0);
	draw_particule(game, p, color);
}

void	update_and_draw_particules(t_game *game)
{
	int			i;
	t_particule	*p;

	i = 0;
	while (i < game->particule_count)
	{
		p = game->particules + i;
		if (p->lifetime_left <= 0)
		{
			*p = game->particules[game->particule_count - 1];
			game->particule_count--;
			continue ;
		}
		else
			update_and_draw_particule(game, p);
		i++;
		p->lifetime_left -= game->dt;
	}
}
