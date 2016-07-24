//
//  Blackhole.hpp
//  blackhole
//
//  Created by MS on 11/22/15.
//  Copyright © 2015 Sarbasov inc. All rights reserved.
//

#ifndef Blackhole_hpp
#define Blackhole_hpp
#include <glm/glm.hpp>
#include <stdio.h>

class Blackhole
{
public:
    glm::dvec3 m_position;
    double m_mass;
    double m_radius;
    double m_accreation_disk_coef;
    Blackhole(glm::dvec3 position, double mass, double accreation_disk_coef);
};

#endif /* Blackhole_hpp */


