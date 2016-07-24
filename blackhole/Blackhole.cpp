//
//  Blackhole.cpp
//  blackhole
//
//  Created by MS on 11/22/15.
//  Copyright © 2015 Sarbasov inc. All rights reserved.
//

#include "Blackhole.hpp"
#include "Constants.h"
using namespace glm;
Blackhole::Blackhole(dvec3 position, double mass, double accreation_disk_coef) : m_position(position), m_mass(mass), m_accreation_disk_coef(accreation_disk_coef)
{
    m_radius = 2 * mass * PHYSICS_G / pow(PHYSICS_C, 2);
}