//
//  globalParams.h
//  cs148-raytracer
//
//  Created by Richard Davis on 12/3/15.
//
//

#ifndef globalParams_h
#define globalParams_h

// General settings
#define SAMPLES_PER_PIXEL 1
#define ACCELERATION_TYPE 2
#define ANTI_ALIAS 1
#define PHOTON_MAPPING 0
#define NUM_REFLECTION_BOUNCES 3
#define NUM_REFRACTION_BOUNCES 3
#define AREA_LIGHT 0
#define AREA_LIGHT_SAMPLES 1
#define MATERIAL_HACK 0

// Settings for the Ray Tracer
#define TRANSPARENT_SHADOWS 0
#define MAX_TRANSMIT 5.f

// Settings for the Photon Mapper
#define DIRECT_LIGHTING 1
#define VISUALIZE_PHOTON_MAPPING 0
#define NUM_PHOTONS 100000
#define PHOTON_GATHER_RADIUS 0.03
#define REFLECT_COLOR 1
#define FINAL_GATHERING 0
#define GATHER_RAYS 32
#define CONE_FILTER 0
#define CONE_CONSTANT 3
#define RESAMPLE_MISSED_GATHER_RAYS 0
#define BRIGHTNESS_HACK 10
#define FIRST_BOUNCE 0

#endif /* globalParams_h */
