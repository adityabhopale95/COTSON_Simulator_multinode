SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
INCLUDE = -I. -I/usr/include/lua5.1
DEPINC = -I.

all: libluabind.a

include ../Make.conf 

install:

libluabind.a: $(OBJS)
	$(RM) $@; $(AR) $@ $(OBJS)

clean:
	$(RM) $(OBJS) libluabind.a

distclean: clean

# DO NOT DELETE

class.o: ./luabind/lua_include.hpp ./luabind/config.hpp ./luabind/class.hpp
class.o: ./luabind/prefix.hpp ./luabind/scope.hpp ./luabind/raw_policy.hpp
class.o: ./luabind/detail/policy.hpp ./luabind/detail/class_registry.hpp
class.o: ./luabind/open.hpp ./luabind/detail/primitives.hpp
class.o: ./luabind/detail/yes_no.hpp ./luabind/detail/object_rep.hpp
class.o: ./luabind/detail/ref.hpp ./luabind/detail/typetraits.hpp
class.o: ./luabind/detail/class_cache.hpp ./luabind/detail/debug.hpp
class.o: ./luabind/detail/class_rep.hpp ./luabind/detail/construct_rep.hpp
class.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
class.o: ./luabind/detail/garbage_collector.hpp
class.o: ./luabind/detail/operator_id.hpp ./luabind/detail/find_best_match.hpp
class.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
class.o: ./luabind/handle.hpp ./luabind/value_wrapper.hpp
class.o: ./luabind/detail/conversion_storage.hpp
class.o: ./luabind/detail/decorate_type.hpp ./luabind/back_reference_fwd.hpp
class.o: ./luabind/from_stack.hpp ./luabind/back_reference.hpp
class.o: ./luabind/wrapper_base.hpp ./luabind/detail/call_member.hpp
class.o: ./luabind/detail/convert_to_lua.hpp ./luabind/detail/pcall.hpp
class.o: ./luabind/detail/stack_utils.hpp ./luabind/object.hpp
class.o: ./luabind/nil.hpp ./luabind/detail/has_get_pointer.hpp
class.o: ./luabind/get_pointer.hpp ./luabind/function.hpp
class.o: ./luabind/make_function.hpp ./luabind/detail/call.hpp
class.o: ./luabind/yield_policy.hpp ./luabind/detail/compute_score.hpp
class.o: ./luabind/detail/deduce_signature.hpp
class.o: ./luabind/detail/most_derived.hpp
class.o: ./luabind/detail/format_signature.hpp
class.o: ./luabind/detail/call_function.hpp ./luabind/dependency_policy.hpp
class.o: ./luabind/detail/constructor.hpp ./luabind/detail/property.hpp
class.o: ./luabind/detail/calc_arity.hpp ./luabind/detail/signature_match.hpp
class.o: ./luabind/detail/enum_maker.hpp ./luabind/detail/get_signature.hpp
class.o: ./luabind/detail/implicit_cast.hpp ./luabind/detail/pointee_typeid.hpp
class.o: ./luabind/detail/link_compatibility.hpp
class_info.o: ./luabind/lua_include.hpp ./luabind/luabind.hpp
class_info.o: ./luabind/config.hpp ./luabind/class.hpp ./luabind/prefix.hpp
class_info.o: ./luabind/scope.hpp ./luabind/raw_policy.hpp
class_info.o: ./luabind/detail/policy.hpp ./luabind/detail/class_registry.hpp
class_info.o: ./luabind/open.hpp ./luabind/detail/primitives.hpp
class_info.o: ./luabind/detail/yes_no.hpp ./luabind/detail/object_rep.hpp
class_info.o: ./luabind/detail/ref.hpp ./luabind/detail/typetraits.hpp
class_info.o: ./luabind/detail/class_cache.hpp ./luabind/detail/debug.hpp
class_info.o: ./luabind/detail/class_rep.hpp ./luabind/detail/construct_rep.hpp
class_info.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
class_info.o: ./luabind/detail/garbage_collector.hpp
class_info.o: ./luabind/detail/operator_id.hpp
class_info.o: ./luabind/detail/find_best_match.hpp
class_info.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
class_info.o: ./luabind/handle.hpp ./luabind/value_wrapper.hpp
class_info.o: ./luabind/detail/conversion_storage.hpp
class_info.o: ./luabind/detail/decorate_type.hpp
class_info.o: ./luabind/back_reference_fwd.hpp ./luabind/from_stack.hpp
class_info.o: ./luabind/back_reference.hpp ./luabind/wrapper_base.hpp
class_info.o: ./luabind/detail/call_member.hpp
class_info.o: ./luabind/detail/convert_to_lua.hpp ./luabind/detail/pcall.hpp
class_info.o: ./luabind/detail/stack_utils.hpp ./luabind/object.hpp
class_info.o: ./luabind/nil.hpp ./luabind/detail/has_get_pointer.hpp
class_info.o: ./luabind/get_pointer.hpp ./luabind/function.hpp
class_info.o: ./luabind/make_function.hpp ./luabind/detail/call.hpp
class_info.o: ./luabind/yield_policy.hpp ./luabind/detail/compute_score.hpp
class_info.o: ./luabind/detail/deduce_signature.hpp
class_info.o: ./luabind/detail/most_derived.hpp
class_info.o: ./luabind/detail/format_signature.hpp
class_info.o: ./luabind/detail/call_function.hpp
class_info.o: ./luabind/dependency_policy.hpp ./luabind/detail/constructor.hpp
class_info.o: ./luabind/detail/property.hpp ./luabind/detail/calc_arity.hpp
class_info.o: ./luabind/detail/signature_match.hpp
class_info.o: ./luabind/detail/enum_maker.hpp
class_info.o: ./luabind/detail/get_signature.hpp
class_info.o: ./luabind/detail/implicit_cast.hpp
class_info.o: ./luabind/detail/pointee_typeid.hpp
class_info.o: ./luabind/detail/link_compatibility.hpp ./luabind/class_info.hpp
class_registry.o: ./luabind/lua_include.hpp ./luabind/luabind.hpp
class_registry.o: ./luabind/config.hpp ./luabind/class.hpp ./luabind/prefix.hpp
class_registry.o: ./luabind/scope.hpp ./luabind/raw_policy.hpp
class_registry.o: ./luabind/detail/policy.hpp
class_registry.o: ./luabind/detail/class_registry.hpp ./luabind/open.hpp
class_registry.o: ./luabind/detail/primitives.hpp ./luabind/detail/yes_no.hpp
class_registry.o: ./luabind/detail/object_rep.hpp ./luabind/detail/ref.hpp
class_registry.o: ./luabind/detail/typetraits.hpp
class_registry.o: ./luabind/detail/class_cache.hpp ./luabind/detail/debug.hpp
class_registry.o: ./luabind/detail/class_rep.hpp
class_registry.o: ./luabind/detail/construct_rep.hpp
class_registry.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
class_registry.o: ./luabind/detail/garbage_collector.hpp
class_registry.o: ./luabind/detail/operator_id.hpp
class_registry.o: ./luabind/detail/find_best_match.hpp
class_registry.o: ./luabind/detail/get_overload_signature.hpp
class_registry.o: ./luabind/error.hpp ./luabind/handle.hpp
class_registry.o: ./luabind/value_wrapper.hpp
class_registry.o: ./luabind/detail/conversion_storage.hpp
class_registry.o: ./luabind/detail/decorate_type.hpp
class_registry.o: ./luabind/back_reference_fwd.hpp ./luabind/from_stack.hpp
class_registry.o: ./luabind/back_reference.hpp ./luabind/wrapper_base.hpp
class_registry.o: ./luabind/detail/call_member.hpp
class_registry.o: ./luabind/detail/convert_to_lua.hpp
class_registry.o: ./luabind/detail/pcall.hpp ./luabind/detail/stack_utils.hpp
class_registry.o: ./luabind/object.hpp ./luabind/nil.hpp
class_registry.o: ./luabind/detail/has_get_pointer.hpp
class_registry.o: ./luabind/get_pointer.hpp ./luabind/function.hpp
class_registry.o: ./luabind/make_function.hpp ./luabind/detail/call.hpp
class_registry.o: ./luabind/yield_policy.hpp ./luabind/detail/compute_score.hpp
class_registry.o: ./luabind/detail/deduce_signature.hpp
class_registry.o: ./luabind/detail/most_derived.hpp
class_registry.o: ./luabind/detail/format_signature.hpp
class_registry.o: ./luabind/detail/call_function.hpp
class_registry.o: ./luabind/dependency_policy.hpp
class_registry.o: ./luabind/detail/constructor.hpp
class_registry.o: ./luabind/detail/property.hpp ./luabind/detail/calc_arity.hpp
class_registry.o: ./luabind/detail/signature_match.hpp
class_registry.o: ./luabind/detail/enum_maker.hpp
class_registry.o: ./luabind/detail/get_signature.hpp
class_registry.o: ./luabind/detail/implicit_cast.hpp
class_registry.o: ./luabind/detail/pointee_typeid.hpp
class_registry.o: ./luabind/detail/link_compatibility.hpp
class_rep.o: ./luabind/lua_include.hpp ./luabind/detail/stack_utils.hpp
class_rep.o: ./luabind/detail/conversion_storage.hpp ./luabind/config.hpp
class_rep.o: ./luabind/luabind.hpp ./luabind/class.hpp ./luabind/prefix.hpp
class_rep.o: ./luabind/scope.hpp ./luabind/raw_policy.hpp
class_rep.o: ./luabind/detail/policy.hpp ./luabind/detail/class_registry.hpp
class_rep.o: ./luabind/open.hpp ./luabind/detail/primitives.hpp
class_rep.o: ./luabind/detail/yes_no.hpp ./luabind/detail/object_rep.hpp
class_rep.o: ./luabind/detail/ref.hpp ./luabind/detail/typetraits.hpp
class_rep.o: ./luabind/detail/class_cache.hpp ./luabind/detail/debug.hpp
class_rep.o: ./luabind/detail/class_rep.hpp ./luabind/detail/construct_rep.hpp
class_rep.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
class_rep.o: ./luabind/detail/garbage_collector.hpp
class_rep.o: ./luabind/detail/operator_id.hpp
class_rep.o: ./luabind/detail/find_best_match.hpp
class_rep.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
class_rep.o: ./luabind/handle.hpp ./luabind/value_wrapper.hpp
class_rep.o: ./luabind/detail/decorate_type.hpp
class_rep.o: ./luabind/back_reference_fwd.hpp ./luabind/from_stack.hpp
class_rep.o: ./luabind/back_reference.hpp ./luabind/wrapper_base.hpp
class_rep.o: ./luabind/detail/call_member.hpp
class_rep.o: ./luabind/detail/convert_to_lua.hpp ./luabind/detail/pcall.hpp
class_rep.o: ./luabind/object.hpp ./luabind/nil.hpp
class_rep.o: ./luabind/detail/has_get_pointer.hpp ./luabind/get_pointer.hpp
class_rep.o: ./luabind/function.hpp ./luabind/make_function.hpp
class_rep.o: ./luabind/detail/call.hpp ./luabind/yield_policy.hpp
class_rep.o: ./luabind/detail/compute_score.hpp
class_rep.o: ./luabind/detail/deduce_signature.hpp
class_rep.o: ./luabind/detail/most_derived.hpp
class_rep.o: ./luabind/detail/format_signature.hpp
class_rep.o: ./luabind/detail/call_function.hpp ./luabind/dependency_policy.hpp
class_rep.o: ./luabind/detail/constructor.hpp ./luabind/detail/property.hpp
class_rep.o: ./luabind/detail/calc_arity.hpp
class_rep.o: ./luabind/detail/signature_match.hpp
class_rep.o: ./luabind/detail/enum_maker.hpp ./luabind/detail/get_signature.hpp
class_rep.o: ./luabind/detail/implicit_cast.hpp
class_rep.o: ./luabind/detail/pointee_typeid.hpp
class_rep.o: ./luabind/detail/link_compatibility.hpp
class_rep.o: ./luabind/exception_handler.hpp
create_class.o: ./luabind/lua_include.hpp ./luabind/luabind.hpp
create_class.o: ./luabind/config.hpp ./luabind/class.hpp ./luabind/prefix.hpp
create_class.o: ./luabind/scope.hpp ./luabind/raw_policy.hpp
create_class.o: ./luabind/detail/policy.hpp ./luabind/detail/class_registry.hpp
create_class.o: ./luabind/open.hpp ./luabind/detail/primitives.hpp
create_class.o: ./luabind/detail/yes_no.hpp ./luabind/detail/object_rep.hpp
create_class.o: ./luabind/detail/ref.hpp ./luabind/detail/typetraits.hpp
create_class.o: ./luabind/detail/class_cache.hpp ./luabind/detail/debug.hpp
create_class.o: ./luabind/detail/class_rep.hpp
create_class.o: ./luabind/detail/construct_rep.hpp
create_class.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
create_class.o: ./luabind/detail/garbage_collector.hpp
create_class.o: ./luabind/detail/operator_id.hpp
create_class.o: ./luabind/detail/find_best_match.hpp
create_class.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
create_class.o: ./luabind/handle.hpp ./luabind/value_wrapper.hpp
create_class.o: ./luabind/detail/conversion_storage.hpp
create_class.o: ./luabind/detail/decorate_type.hpp
create_class.o: ./luabind/back_reference_fwd.hpp ./luabind/from_stack.hpp
create_class.o: ./luabind/back_reference.hpp ./luabind/wrapper_base.hpp
create_class.o: ./luabind/detail/call_member.hpp
create_class.o: ./luabind/detail/convert_to_lua.hpp ./luabind/detail/pcall.hpp
create_class.o: ./luabind/detail/stack_utils.hpp ./luabind/object.hpp
create_class.o: ./luabind/nil.hpp ./luabind/detail/has_get_pointer.hpp
create_class.o: ./luabind/get_pointer.hpp ./luabind/function.hpp
create_class.o: ./luabind/make_function.hpp ./luabind/detail/call.hpp
create_class.o: ./luabind/yield_policy.hpp ./luabind/detail/compute_score.hpp
create_class.o: ./luabind/detail/deduce_signature.hpp
create_class.o: ./luabind/detail/most_derived.hpp
create_class.o: ./luabind/detail/format_signature.hpp
create_class.o: ./luabind/detail/call_function.hpp
create_class.o: ./luabind/dependency_policy.hpp
create_class.o: ./luabind/detail/constructor.hpp ./luabind/detail/property.hpp
create_class.o: ./luabind/detail/calc_arity.hpp
create_class.o: ./luabind/detail/signature_match.hpp
create_class.o: ./luabind/detail/enum_maker.hpp
create_class.o: ./luabind/detail/get_signature.hpp
create_class.o: ./luabind/detail/implicit_cast.hpp
create_class.o: ./luabind/detail/pointee_typeid.hpp
create_class.o: ./luabind/detail/link_compatibility.hpp
error.o: ./luabind/error.hpp ./luabind/prefix.hpp ./luabind/config.hpp
exception_handler.o: ./luabind/config.hpp ./luabind/exception_handler.hpp
exception_handler.o: ./luabind/lua_include.hpp ./luabind/error.hpp
exception_handler.o: ./luabind/prefix.hpp
find_best_match.o: ./luabind/lua_include.hpp ./luabind/luabind.hpp
find_best_match.o: ./luabind/config.hpp ./luabind/class.hpp
find_best_match.o: ./luabind/prefix.hpp ./luabind/scope.hpp
find_best_match.o: ./luabind/raw_policy.hpp ./luabind/detail/policy.hpp
find_best_match.o: ./luabind/detail/class_registry.hpp ./luabind/open.hpp
find_best_match.o: ./luabind/detail/primitives.hpp ./luabind/detail/yes_no.hpp
find_best_match.o: ./luabind/detail/object_rep.hpp ./luabind/detail/ref.hpp
find_best_match.o: ./luabind/detail/typetraits.hpp
find_best_match.o: ./luabind/detail/class_cache.hpp ./luabind/detail/debug.hpp
find_best_match.o: ./luabind/detail/class_rep.hpp
find_best_match.o: ./luabind/detail/construct_rep.hpp
find_best_match.o: ./luabind/detail/overload_rep_base.hpp
find_best_match.o: ./luabind/weak_ref.hpp
find_best_match.o: ./luabind/detail/garbage_collector.hpp
find_best_match.o: ./luabind/detail/operator_id.hpp
find_best_match.o: ./luabind/detail/find_best_match.hpp
find_best_match.o: ./luabind/detail/get_overload_signature.hpp
find_best_match.o: ./luabind/error.hpp ./luabind/handle.hpp
find_best_match.o: ./luabind/value_wrapper.hpp
find_best_match.o: ./luabind/detail/conversion_storage.hpp
find_best_match.o: ./luabind/detail/decorate_type.hpp
find_best_match.o: ./luabind/back_reference_fwd.hpp ./luabind/from_stack.hpp
find_best_match.o: ./luabind/back_reference.hpp ./luabind/wrapper_base.hpp
find_best_match.o: ./luabind/detail/call_member.hpp
find_best_match.o: ./luabind/detail/convert_to_lua.hpp
find_best_match.o: ./luabind/detail/pcall.hpp ./luabind/detail/stack_utils.hpp
find_best_match.o: ./luabind/object.hpp ./luabind/nil.hpp
find_best_match.o: ./luabind/detail/has_get_pointer.hpp
find_best_match.o: ./luabind/get_pointer.hpp ./luabind/function.hpp
find_best_match.o: ./luabind/make_function.hpp ./luabind/detail/call.hpp
find_best_match.o: ./luabind/yield_policy.hpp
find_best_match.o: ./luabind/detail/compute_score.hpp
find_best_match.o: ./luabind/detail/deduce_signature.hpp
find_best_match.o: ./luabind/detail/most_derived.hpp
find_best_match.o: ./luabind/detail/format_signature.hpp
find_best_match.o: ./luabind/detail/call_function.hpp
find_best_match.o: ./luabind/dependency_policy.hpp
find_best_match.o: ./luabind/detail/constructor.hpp
find_best_match.o: ./luabind/detail/property.hpp
find_best_match.o: ./luabind/detail/calc_arity.hpp
find_best_match.o: ./luabind/detail/signature_match.hpp
find_best_match.o: ./luabind/detail/enum_maker.hpp
find_best_match.o: ./luabind/detail/get_signature.hpp
find_best_match.o: ./luabind/detail/implicit_cast.hpp
find_best_match.o: ./luabind/detail/pointee_typeid.hpp
find_best_match.o: ./luabind/detail/link_compatibility.hpp
function.o: ./luabind/exception_handler.hpp ./luabind/lua_include.hpp
function.o: ./luabind/config.hpp ./luabind/make_function.hpp
function.o: ./luabind/object.hpp ./luabind/nil.hpp ./luabind/value_wrapper.hpp
function.o: ./luabind/detail/pcall.hpp ./luabind/handle.hpp
function.o: ./luabind/detail/ref.hpp ./luabind/from_stack.hpp
function.o: ./luabind/detail/policy.hpp ./luabind/detail/class_registry.hpp
function.o: ./luabind/open.hpp ./luabind/detail/primitives.hpp
function.o: ./luabind/detail/yes_no.hpp ./luabind/detail/object_rep.hpp
function.o: ./luabind/detail/typetraits.hpp ./luabind/detail/class_cache.hpp
function.o: ./luabind/prefix.hpp ./luabind/detail/debug.hpp
function.o: ./luabind/detail/class_rep.hpp ./luabind/detail/construct_rep.hpp
function.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
function.o: ./luabind/detail/garbage_collector.hpp
function.o: ./luabind/detail/operator_id.hpp
function.o: ./luabind/detail/find_best_match.hpp
function.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
function.o: ./luabind/detail/conversion_storage.hpp
function.o: ./luabind/detail/decorate_type.hpp ./luabind/back_reference_fwd.hpp
function.o: ./luabind/detail/stack_utils.hpp
function.o: ./luabind/detail/convert_to_lua.hpp ./luabind/detail/call.hpp
function.o: ./luabind/yield_policy.hpp ./luabind/detail/compute_score.hpp
function.o: ./luabind/detail/deduce_signature.hpp
function.o: ./luabind/detail/most_derived.hpp
function.o: ./luabind/detail/format_signature.hpp
implicit_cast.o: ./luabind/lua_include.hpp ./luabind/luabind.hpp
implicit_cast.o: ./luabind/config.hpp ./luabind/class.hpp ./luabind/prefix.hpp
implicit_cast.o: ./luabind/scope.hpp ./luabind/raw_policy.hpp
implicit_cast.o: ./luabind/detail/policy.hpp
implicit_cast.o: ./luabind/detail/class_registry.hpp ./luabind/open.hpp
implicit_cast.o: ./luabind/detail/primitives.hpp ./luabind/detail/yes_no.hpp
implicit_cast.o: ./luabind/detail/object_rep.hpp ./luabind/detail/ref.hpp
implicit_cast.o: ./luabind/detail/typetraits.hpp
implicit_cast.o: ./luabind/detail/class_cache.hpp ./luabind/detail/debug.hpp
implicit_cast.o: ./luabind/detail/class_rep.hpp
implicit_cast.o: ./luabind/detail/construct_rep.hpp
implicit_cast.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
implicit_cast.o: ./luabind/detail/garbage_collector.hpp
implicit_cast.o: ./luabind/detail/operator_id.hpp
implicit_cast.o: ./luabind/detail/find_best_match.hpp
implicit_cast.o: ./luabind/detail/get_overload_signature.hpp
implicit_cast.o: ./luabind/error.hpp ./luabind/handle.hpp
implicit_cast.o: ./luabind/value_wrapper.hpp
implicit_cast.o: ./luabind/detail/conversion_storage.hpp
implicit_cast.o: ./luabind/detail/decorate_type.hpp
implicit_cast.o: ./luabind/back_reference_fwd.hpp ./luabind/from_stack.hpp
implicit_cast.o: ./luabind/back_reference.hpp ./luabind/wrapper_base.hpp
implicit_cast.o: ./luabind/detail/call_member.hpp
implicit_cast.o: ./luabind/detail/convert_to_lua.hpp ./luabind/detail/pcall.hpp
implicit_cast.o: ./luabind/detail/stack_utils.hpp ./luabind/object.hpp
implicit_cast.o: ./luabind/nil.hpp ./luabind/detail/has_get_pointer.hpp
implicit_cast.o: ./luabind/get_pointer.hpp ./luabind/function.hpp
implicit_cast.o: ./luabind/make_function.hpp ./luabind/detail/call.hpp
implicit_cast.o: ./luabind/yield_policy.hpp ./luabind/detail/compute_score.hpp
implicit_cast.o: ./luabind/detail/deduce_signature.hpp
implicit_cast.o: ./luabind/detail/most_derived.hpp
implicit_cast.o: ./luabind/detail/format_signature.hpp
implicit_cast.o: ./luabind/detail/call_function.hpp
implicit_cast.o: ./luabind/dependency_policy.hpp
implicit_cast.o: ./luabind/detail/constructor.hpp ./luabind/detail/property.hpp
implicit_cast.o: ./luabind/detail/calc_arity.hpp
implicit_cast.o: ./luabind/detail/signature_match.hpp
implicit_cast.o: ./luabind/detail/enum_maker.hpp
implicit_cast.o: ./luabind/detail/get_signature.hpp
implicit_cast.o: ./luabind/detail/implicit_cast.hpp
implicit_cast.o: ./luabind/detail/pointee_typeid.hpp
implicit_cast.o: ./luabind/detail/link_compatibility.hpp
link_compatibility.o: ./luabind/detail/link_compatibility.hpp
link_compatibility.o: ./luabind/config.hpp
object_rep.o: ./luabind/detail/object_rep.hpp ./luabind/config.hpp
object_rep.o: ./luabind/detail/ref.hpp ./luabind/lua_include.hpp
object_rep.o: ./luabind/detail/class_rep.hpp ./luabind/detail/construct_rep.hpp
object_rep.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
object_rep.o: ./luabind/detail/garbage_collector.hpp
object_rep.o: ./luabind/detail/operator_id.hpp
object_rep.o: ./luabind/detail/class_registry.hpp ./luabind/open.hpp
object_rep.o: ./luabind/detail/find_best_match.hpp
object_rep.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
object_rep.o: ./luabind/prefix.hpp ./luabind/handle.hpp
object_rep.o: ./luabind/value_wrapper.hpp ./luabind/detail/primitives.hpp
object_rep.o: ./luabind/detail/yes_no.hpp
open.o: ./luabind/lua_include.hpp ./luabind/luabind.hpp ./luabind/config.hpp
open.o: ./luabind/class.hpp ./luabind/prefix.hpp ./luabind/scope.hpp
open.o: ./luabind/raw_policy.hpp ./luabind/detail/policy.hpp
open.o: ./luabind/detail/class_registry.hpp ./luabind/open.hpp
open.o: ./luabind/detail/primitives.hpp ./luabind/detail/yes_no.hpp
open.o: ./luabind/detail/object_rep.hpp ./luabind/detail/ref.hpp
open.o: ./luabind/detail/typetraits.hpp ./luabind/detail/class_cache.hpp
open.o: ./luabind/detail/debug.hpp ./luabind/detail/class_rep.hpp
open.o: ./luabind/detail/construct_rep.hpp
open.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
open.o: ./luabind/detail/garbage_collector.hpp ./luabind/detail/operator_id.hpp
open.o: ./luabind/detail/find_best_match.hpp
open.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
open.o: ./luabind/handle.hpp ./luabind/value_wrapper.hpp
open.o: ./luabind/detail/conversion_storage.hpp
open.o: ./luabind/detail/decorate_type.hpp ./luabind/back_reference_fwd.hpp
open.o: ./luabind/from_stack.hpp ./luabind/back_reference.hpp
open.o: ./luabind/wrapper_base.hpp ./luabind/detail/call_member.hpp
open.o: ./luabind/detail/convert_to_lua.hpp ./luabind/detail/pcall.hpp
open.o: ./luabind/detail/stack_utils.hpp ./luabind/object.hpp ./luabind/nil.hpp
open.o: ./luabind/detail/has_get_pointer.hpp ./luabind/get_pointer.hpp
open.o: ./luabind/function.hpp ./luabind/make_function.hpp
open.o: ./luabind/detail/call.hpp ./luabind/yield_policy.hpp
open.o: ./luabind/detail/compute_score.hpp
open.o: ./luabind/detail/deduce_signature.hpp ./luabind/detail/most_derived.hpp
open.o: ./luabind/detail/format_signature.hpp
open.o: ./luabind/detail/call_function.hpp ./luabind/dependency_policy.hpp
open.o: ./luabind/detail/constructor.hpp ./luabind/detail/property.hpp
open.o: ./luabind/detail/calc_arity.hpp ./luabind/detail/signature_match.hpp
open.o: ./luabind/detail/enum_maker.hpp ./luabind/detail/get_signature.hpp
open.o: ./luabind/detail/implicit_cast.hpp ./luabind/detail/pointee_typeid.hpp
open.o: ./luabind/detail/link_compatibility.hpp
overload_rep.o: ./luabind/detail/overload_rep.hpp ./luabind/config.hpp
overload_rep.o: ./luabind/detail/overload_rep_base.hpp
overload_rep.o: ./luabind/detail/is_indirect_const.hpp
overload_rep.o: ./luabind/detail/yes_no.hpp ./luabind/detail/policy.hpp
overload_rep.o: ./luabind/detail/class_registry.hpp ./luabind/open.hpp
overload_rep.o: ./luabind/detail/primitives.hpp ./luabind/detail/object_rep.hpp
overload_rep.o: ./luabind/detail/ref.hpp ./luabind/lua_include.hpp
overload_rep.o: ./luabind/detail/typetraits.hpp
overload_rep.o: ./luabind/detail/class_cache.hpp ./luabind/prefix.hpp
overload_rep.o: ./luabind/detail/debug.hpp ./luabind/detail/class_rep.hpp
overload_rep.o: ./luabind/detail/construct_rep.hpp ./luabind/weak_ref.hpp
overload_rep.o: ./luabind/detail/garbage_collector.hpp
overload_rep.o: ./luabind/detail/operator_id.hpp
overload_rep.o: ./luabind/detail/find_best_match.hpp
overload_rep.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
overload_rep.o: ./luabind/handle.hpp ./luabind/value_wrapper.hpp
overload_rep.o: ./luabind/detail/conversion_storage.hpp
overload_rep.o: ./luabind/detail/decorate_type.hpp
overload_rep.o: ./luabind/back_reference_fwd.hpp ./luabind/from_stack.hpp
pcall.o: ./luabind/detail/pcall.hpp ./luabind/config.hpp ./luabind/error.hpp
pcall.o: ./luabind/prefix.hpp ./luabind/lua_include.hpp
ref.o: ./luabind/config.hpp ./luabind/detail/ref.hpp ./luabind/lua_include.hpp
ref.o: ./luabind/detail/debug.hpp
scope.o: ./luabind/lua_include.hpp ./luabind/scope.hpp ./luabind/prefix.hpp
scope.o: ./luabind/config.hpp ./luabind/detail/debug.hpp
scope.o: ./luabind/detail/stack_utils.hpp
stack_content_by_name.o: ./luabind/lua_include.hpp ./luabind/luabind.hpp
stack_content_by_name.o: ./luabind/config.hpp ./luabind/class.hpp
stack_content_by_name.o: ./luabind/prefix.hpp ./luabind/scope.hpp
stack_content_by_name.o: ./luabind/raw_policy.hpp ./luabind/detail/policy.hpp
stack_content_by_name.o: ./luabind/detail/class_registry.hpp ./luabind/open.hpp
stack_content_by_name.o: ./luabind/detail/primitives.hpp
stack_content_by_name.o: ./luabind/detail/yes_no.hpp
stack_content_by_name.o: ./luabind/detail/object_rep.hpp
stack_content_by_name.o: ./luabind/detail/ref.hpp
stack_content_by_name.o: ./luabind/detail/typetraits.hpp
stack_content_by_name.o: ./luabind/detail/class_cache.hpp
stack_content_by_name.o: ./luabind/detail/debug.hpp
stack_content_by_name.o: ./luabind/detail/class_rep.hpp
stack_content_by_name.o: ./luabind/detail/construct_rep.hpp
stack_content_by_name.o: ./luabind/detail/overload_rep_base.hpp
stack_content_by_name.o: ./luabind/weak_ref.hpp
stack_content_by_name.o: ./luabind/detail/garbage_collector.hpp
stack_content_by_name.o: ./luabind/detail/operator_id.hpp
stack_content_by_name.o: ./luabind/detail/find_best_match.hpp
stack_content_by_name.o: ./luabind/detail/get_overload_signature.hpp
stack_content_by_name.o: ./luabind/error.hpp ./luabind/handle.hpp
stack_content_by_name.o: ./luabind/value_wrapper.hpp
stack_content_by_name.o: ./luabind/detail/conversion_storage.hpp
stack_content_by_name.o: ./luabind/detail/decorate_type.hpp
stack_content_by_name.o: ./luabind/back_reference_fwd.hpp
stack_content_by_name.o: ./luabind/from_stack.hpp ./luabind/back_reference.hpp
stack_content_by_name.o: ./luabind/wrapper_base.hpp
stack_content_by_name.o: ./luabind/detail/call_member.hpp
stack_content_by_name.o: ./luabind/detail/convert_to_lua.hpp
stack_content_by_name.o: ./luabind/detail/pcall.hpp
stack_content_by_name.o: ./luabind/detail/stack_utils.hpp ./luabind/object.hpp
stack_content_by_name.o: ./luabind/nil.hpp ./luabind/detail/has_get_pointer.hpp
stack_content_by_name.o: ./luabind/get_pointer.hpp ./luabind/function.hpp
stack_content_by_name.o: ./luabind/make_function.hpp ./luabind/detail/call.hpp
stack_content_by_name.o: ./luabind/yield_policy.hpp
stack_content_by_name.o: ./luabind/detail/compute_score.hpp
stack_content_by_name.o: ./luabind/detail/deduce_signature.hpp
stack_content_by_name.o: ./luabind/detail/most_derived.hpp
stack_content_by_name.o: ./luabind/detail/format_signature.hpp
stack_content_by_name.o: ./luabind/detail/call_function.hpp
stack_content_by_name.o: ./luabind/dependency_policy.hpp
stack_content_by_name.o: ./luabind/detail/constructor.hpp
stack_content_by_name.o: ./luabind/detail/property.hpp
stack_content_by_name.o: ./luabind/detail/calc_arity.hpp
stack_content_by_name.o: ./luabind/detail/signature_match.hpp
stack_content_by_name.o: ./luabind/detail/enum_maker.hpp
stack_content_by_name.o: ./luabind/detail/get_signature.hpp
stack_content_by_name.o: ./luabind/detail/implicit_cast.hpp
stack_content_by_name.o: ./luabind/detail/pointee_typeid.hpp
stack_content_by_name.o: ./luabind/detail/link_compatibility.hpp
weak_ref.o: ./luabind/lua_include.hpp ./luabind/config.hpp
weak_ref.o: ./luabind/weak_ref.hpp
wrapper_base.o: ./luabind/config.hpp ./luabind/lua_include.hpp
wrapper_base.o: ./luabind/function.hpp ./luabind/make_function.hpp
wrapper_base.o: ./luabind/object.hpp ./luabind/nil.hpp
wrapper_base.o: ./luabind/value_wrapper.hpp ./luabind/detail/pcall.hpp
wrapper_base.o: ./luabind/handle.hpp ./luabind/detail/ref.hpp
wrapper_base.o: ./luabind/from_stack.hpp ./luabind/detail/policy.hpp
wrapper_base.o: ./luabind/detail/class_registry.hpp ./luabind/open.hpp
wrapper_base.o: ./luabind/detail/primitives.hpp ./luabind/detail/yes_no.hpp
wrapper_base.o: ./luabind/detail/object_rep.hpp ./luabind/detail/typetraits.hpp
wrapper_base.o: ./luabind/detail/class_cache.hpp ./luabind/prefix.hpp
wrapper_base.o: ./luabind/detail/debug.hpp ./luabind/detail/class_rep.hpp
wrapper_base.o: ./luabind/detail/construct_rep.hpp
wrapper_base.o: ./luabind/detail/overload_rep_base.hpp ./luabind/weak_ref.hpp
wrapper_base.o: ./luabind/detail/garbage_collector.hpp
wrapper_base.o: ./luabind/detail/operator_id.hpp
wrapper_base.o: ./luabind/detail/find_best_match.hpp
wrapper_base.o: ./luabind/detail/get_overload_signature.hpp ./luabind/error.hpp
wrapper_base.o: ./luabind/detail/conversion_storage.hpp
wrapper_base.o: ./luabind/detail/decorate_type.hpp
wrapper_base.o: ./luabind/back_reference_fwd.hpp
wrapper_base.o: ./luabind/detail/stack_utils.hpp
wrapper_base.o: ./luabind/detail/convert_to_lua.hpp ./luabind/detail/call.hpp
wrapper_base.o: ./luabind/yield_policy.hpp ./luabind/detail/compute_score.hpp
wrapper_base.o: ./luabind/detail/deduce_signature.hpp
wrapper_base.o: ./luabind/detail/most_derived.hpp
wrapper_base.o: ./luabind/detail/format_signature.hpp ./luabind/scope.hpp
wrapper_base.o: ./luabind/detail/call_function.hpp
