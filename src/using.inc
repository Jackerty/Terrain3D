// Since GDExtension uses godot namespace but GDNative does not
// we use this header to make GDExtension to force godot namespace
// as global.
#if defined(GDEXTENSION) && !defined(GD_NATIVE)
using namespace godot;
#endif