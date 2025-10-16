#ifndef GD_INTEROP_HPP
#define GD_INTEROP_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_server3d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/shader.hpp>

#include "using.inc"

#ifdef GD_NATIVE
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/templates/list.hpp>
#endif

/*
* Some functions in GDExtension are "virtual" calls in the scripting engine. They do not per say exist as C++ functions.
* However there might be a function that drops underdash from the name which calls scripting engine. If this function
* virtual it can be overriding for in engine.
*
* This macro adds the dash or drops it based upon is compilation for the GDExtension or GDNative.
*/
#ifdef GDEXTENSION
	#define DEC_GDVIRTUAL(func) _ ## func
#else
	#define DEC_GDVIRTUAL(func) func
#endif

/*
* Sometimes GDExtension has function public which in engine has dash and is protected. These cases this method can be used
* connect the two.
*/
#ifdef GDEXTENSION
	#define DEC_GDMETHOD(func) func
#else
	#define DEC_GDMETHOD(func) _ ## func
#endif

/*
* GDNative does not provide public access to Resource::take_over_path. This is most like caused by unintended leaking protected
* methods in GDScrtip which lacks protection levels as of writting.
* 
* To fix this situation this macro defines public take_over_path which then can be used for Resource children to when GDNative is defined.
*
* Howevery, since it is unclear is take_over_path public use uninded mark this function as deprecated.
*/
#ifdef GDEXTENSION
	#define RESOURCE_TAKE_OVER_PATH
#else
	#define RESOURCE_TAKE_OVER_PATH /*[[deprecated("In Godot engine _take_over_path is protected. Usage of take_over_path might be unintended outside of Resource.")]]*/ void take_over_path(String p_path) { this->_take_over_path(p_path); }
#endif

namespace GDInterop {

	namespace OS {
		/*
		* OS::get_cmdline_args return value differs between in engine and GDextension. Alias the return type.
		*/
		#ifdef GDEXTENSION
			using CMDLINE_ARGS = PackedStringArray;
		#else
			using CMDLINE_ARGS = List<::String>;
		#endif
	}

	namespace FileAccess {
		/*
		* FileAccess::File_exists is are virtual method in the GDNative but static in GDExtension.
		* Static exists check is just `FileAccess::exists` in GDNative.
		*/
		static inline bool file_exists(::String path) {
			#ifdef GDEXTENSION
				return ::FileAccess::file_exists(path);
			#else
				return ::FileAccess::exists(path);
			#endif
		}
	}

	namespace Node {
		/*
		* For some reason get_node is get_node_internal in the GDExtension.
		* To fix interopt conflict get_node() function is provided as inline.
		*/
		static inline ::Node *get_node_internal(::Node *p_this, ::String path) {
			#ifdef GDEXTENSION
				return p_this->get_node_internal(path);
			#else
				return p_this->get_node(path);
			#endif
		}
	}

	namespace ResourceLoader {
		/*
		* Issue with ResourceLoader::load is in engine it is a static method where as in the GDExtension is it a
		* reqular method in the ResourceLoader.
		*/
		static inline Ref<::Resource> load(const ::String &p_path, const ::String &p_type_hint = ::String(), ::ResourceFormatLoader::CacheMode p_cache_mode = ::ResourceFormatLoader::CacheMode::CACHE_MODE_REUSE) {
			#ifdef GDEXTENSION
				return ::ResourceLoader::get_singleton()->load(p_path, p_type_hint, (::ResourceLoader::CacheMode)p_cache_mode);
			#else
				// Error is more information but for the load failing might be intentional so don't print anything.
				// Might be usefull for debugging.
				Error r_error;
				return ::ResourceLoader::load(p_path, p_type_hint, p_cache_mode, &r_error);
			#endif
		}

	}

	namespace ResourceSaver {
		/*
		* In GDNative save is a static method where as in GDExtension you access singleton to get the save method.
		* This unifies the approahes.
		*/
		static inline Error save(const Ref<::Resource> &p_resource, const ::String &p_path = ::String(), ::BitField<::ResourceSaver::SaverFlags> p_flags = ::ResourceSaver::SaverFlags::FLAG_NONE) {
			#ifdef GDEXTENSION
				return ::ResourceSaver::get_singleton()->save(p_resource, p_path, p_flags);
			#else
				return ::ResourceSaver::save(p_resource, p_path, p_flags);
			#endif
		}
	}

	namespace PhysicsDirectSpaceState3D {
		/*
		* Intersect_ray differs the in parameters. There is also private method in GDNative which has same parameters.
		*/
		static inline Dictionary intersect_ray(::PhysicsDirectSpaceState3D *p_this, const Ref<::PhysicsRayQueryParameters3D> &p_parameters) {
			#ifdef GDEXTENSION
				return p_this->intersect_ray(p_parameters);
			#else
				// In Native this method does exist but it is marked private. We have to call public intersect_ray which uses Godot internal
				// structures to make the call to actual physics engine implementation.
				const ::PhysicsDirectSpaceState3D::RayParameters &query = p_parameters->get_parameters();

				::PhysicsDirectSpaceState3D::RayResult result;
				if(p_this->intersect_ray(query, result) == false) return Dictionary();

				// This copy paste from the godot engine it self to make sure dictionary is build like the engine.
				// No function for this convertion is provided.
				Dictionary d;
				d["position"] = result.position;
				d["normal"] = result.normal;
				d["face_index"] = result.face_index;
				d["collider_id"] = result.collider_id;
				d["collider"] = result.collider;
				d["shape"] = result.shape;
				d["rid"] = result.rid;

				return d;
			#endif
		}
	}

	namespace RenderingServer {
		/*
		* RenderingServer::texture_2d_layered_create first parameter chages between GDExtension and GDNative. This function unwraps TypedArray when GDNative is called.
		*
		* Other way to solve this issue would be alias different class in GDExtension than GDNative. However, this way was chosen so that GDExtension takes priority.
		*/
		static inline RID texture_2d_layered_create(const TypedArray<Ref<Image>> &p_layers, ::RenderingServer::TextureLayeredType p_layered_type) {
			#ifdef GDEXTENSION
				return ::RenderingServer::get_singleton()->texture_2d_layered_create(p_layers, p_layered_type);
			#else
				Vector<Ref<Image>> layers;
				layers.resize(p_layers.size());
				for(int i = 0; i < p_layers.size(); i++) {
					layers.set(i, p_layers[i]);
				}
				return ::RenderingServer::get_singleton()->texture_2d_layered_create(layers, p_layered_type);
			#endif
		}

		/*
		* In the GDNative the force_draw is called just draw... I assume they were at some point different methods.
		*/
		static inline void force_draw(bool p_swap_buffers = true, double p_frame_step = 0.0) {
			#ifdef GDEXTENSION
				::RenderingServer::get_singleton()->force_draw(p_swap_buffers, p_frame_step);
			#else
				::RenderingServer::get_singleton()->draw(p_swap_buffers, p_frame_step);
			#endif
		}

		/*
		* This function does exist in GDNative but is marked private and has different name.
		* Same named function in GDNative is has different type of parameter signature and no return.
		*/
		static inline TypedArray<Dictionary> get_shader_parameter_list(const RID &p_shader){
			#ifdef GDEXTENSION
				return ::RenderingServer::get_singleton()->get_shader_parameter_list(p_shader);
			#else
				List<PropertyInfo> result;
				::RenderingServer::get_singleton()->get_shader_parameter_list(p_shader, &result);

				return convert_property_list(&result);
			#endif
		}
	}

	namespace CollisionShape3D {
		/*
		 * In GDNative this method is called set_debug_fill_enabled. Maybe waiting for next Godot major release
		 * for rename....
		*/
		static inline void set_enable_debug_fill(::CollisionShape3D *p_this, bool p_enable){
			#ifdef GDEXTENSION
				p_this->set_enable_debug_fill(p_enable);
			#else
				p_this->set_debug_fill_enabled(p_enable);
			#endif
		}
	}

	namespace Shader {
		/*
		* This function does exist in GDNative but is private and has front dash. Alternative call
		* has different parameters.
		*/
		static inline Array get_shader_uniform_list(::Shader *p_this, bool p_get_groups = false) {
			#ifdef GDEXTENSION
				return p_this->get_shader_uniform_list(p_get_groups);
			#else
				List<PropertyInfo> results;
				p_this->get_shader_uniform_list(&results, p_get_groups);
				return convert_property_list(&results);
			#endif
		}

		/*
		* Alias for the get_shader_uniform_list(::Shader *, bool) which takes in Ref for easier use.
		*/
		static inline Array get_shader_uniform_list(Ref<::Shader> p_this, bool p_get_groups = false) {
			#ifdef GDEXTENSION
				return p_this->get_shader_uniform_list(p_get_groups);
			#else
				List<PropertyInfo> results;
				p_this->get_shader_uniform_list(&results, p_get_groups);
				return convert_property_list(&results);
			#endif
		}
	}

	namespace String {
		/*
		* GDNative does not define String::size. There is resize_uninitialized which sounds similiar.
		*/
		static inline Error resize(::String *p_this, int64_t p_size){
			#ifdef GDEXTENSION
				return p_this->resize(p_size);
			#else
				return p_this->resize_uninitialized(p_size);
			#endif
		}
	}
}

#endif /* GD_INTEROP_HPP */