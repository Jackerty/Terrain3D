// Copyright © 2025 Cory Petkovsek, Roope Palmroos, and Contributors.

#ifndef TERRAIN3D_REGISTER_TYPES_H
#define TERRAIN3D_REGISTER_TYPES_H

#ifdef GDEXTENSION
#include <godot_cpp/godot.hpp>
using namespace godot;
#else
#include "modules/register_module_types.h"
#endif

// NOTE: These have module ending for custom module build combability.
void initialize_terrain_3d_module(ModuleInitializationLevel p_level);
void uninitialize_terrain_3d_module(ModuleInitializationLevel p_level);

#endif // TERRAIN3D_REGISTER_TYPES_H
