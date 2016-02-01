/**
 * @file sampledlsh.h
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 * @date 2015
 *
 * @section GPL
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @brief Declaration of functios for Sampled Locality-Sensitive Hashing (SLSH)
 */
#ifndef SAMPLEDLSH_H
#define  SAMPLEDLSH_H

#include "l1lsh.h"
#include "lplsh.h"

void sampledlsh_l1_get_coitems(ListDB *, HashTableL1 *);
ListDB sampledlsh_l1mine(ListDB *, uint, uint, uint, uint);
void sampledlsh_lp_get_coitems(ListDB *, HashTableLP *);
ListDB sampledlsh_lpmine(VectorDB *, uint, uint, double, uint, double (*ps_dist)(void));

#endif
