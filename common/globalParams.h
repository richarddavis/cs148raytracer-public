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
#define SAMPLES_PER_PIXEL 64
#define ACCELERATION_TYPE 2
#define ANTI_ALIAS 1
#define PHOTON_MAPPING 0
#define NUM_BOUNCES 5
#define AREA_LIGHT 0
#define AREA_LIGHT_SAMPLES 3
#define MATERIAL_HACK 0

// Settings for the Ray Tracer
#define TRANSPARENT_SHADOWS 1
#define MAX_TRANSMIT 5.f

// Settings for the Photon Mapper
#define DIRECT_LIGHTING 1
#define VISUALIZE_PHOTON_MAPPING 0
#define NUM_PHOTONS 1000000
#define PHOTON_GATHER_RADIUS 0.04
#define REFLECT_COLOR 0
#define FINAL_GATHERING 1
#define GATHER_RAYS 128
#define CONE_FILTER 1
#define CONE_CONSTANT 3
#define RESAMPLE_MISSED_GATHER_RAYS 0
#define BRIGHTNESS_HACK 30
#define FIRST_BOUNCE 0

#endif /* globalParams_h */
