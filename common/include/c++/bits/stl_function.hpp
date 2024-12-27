#ifndef __STL_FUNCTION
#define __STL_FUNCTION
#include "concepts"
#include "bits/move.h"
#include "bits/invoke.hpp"
namespace std
{
    template<typename FT> class function;
    template<typename T> concept __location_invariant = std::is_trivially_copyable_v<T>;
    class __undefined;
    union __no_copy
    {
        void*       __object;
        const void* __object_const;
        void (*__function_ptr)();
        void (__undefined::*__memfn_ptr)();
    };
    union [[gnu::may_alias]] __data_store
    {
        __no_copy __ignored;
        char __stored_data[sizeof(__no_copy)];
        constexpr void* __access() noexcept { return &__stored_data[0]; }
        constexpr const void* __access() const noexcept { return &__stored_data[0]; }
        template<typename T> constexpr T& __access() noexcept { return *static_cast<T*>(__access()); }
        template<typename T> constexpr T const& __access() const noexcept { return *static_cast<T const*>(__access()); }
    };
    enum __func_manage_op
    {
        __get_type_info,
        __get_functor_ptr,
        __clone_functor,
        __destroy_functor
    };
    template<typename R, typename C, typename ... Args> using member_fn = R (C::*)(Args...);
    struct __function_base
    {
        template<typename FT>
        class __fn_manager
        {
            constexpr static size_t __max_size = sizeof(__no_copy);
            constexpr static size_t __max_align = alignof(__no_copy);
            template<typename F2> constexpr static void __create_wrapper(__data_store& dest, F2&& src, true_type) { ::new (dest.__access()) FT { forward<F2>(src) }; }
            template<typename F2> constexpr static void __create_wrapper(__data_store& dest, F2&& src, false_type) { using ftor = remove_reference_t<F2>; dest.__access<F2*>() = new ftor{ src }; }
            constexpr static void __delete_wrapper(__data_store& target, true_type) { target.__access<FT>().~FT(); }
            constexpr static void __delete_wrapper(__data_store& target, false_type) { ::operator delete(target.__access<FT*>()); }
        protected:
            using __is_locally_storable = __and_<is_trivially_copyable<FT>, bool_constant<sizeof(FT) <= __max_size && __alignof__(FT) <= __max_align && __max_align % __alignof__(FT) == 0>>;
            constexpr static bool __is_local_store = __is_locally_storable::value;
            static FT* __get_pointer(__data_store const& data) { if constexpr(__is_local_store) return addressof(const_cast<FT&>(data.__access<FT>())); else return data.__access<FT*>(); }
        public:
            template<typename F2> constexpr static void __init_fn(__data_store& dest, F2&& src) noexcept(__and_<__is_locally_storable, is_nothrow_constructible<F2, F2>>::value) { __create_wrapper(dest, forward<F2>(src), __is_locally_storable{}); }
            template<typename F2> constexpr static bool __not_empty(F2* f2) noexcept { return f2 != nullptr; }
            template<typename R, typename C, typename ... Args> constexpr static bool __not_empty(member_fn<R, C, Args...> mf) noexcept { return mf != nullptr; }
            template<typename F2> constexpr static bool __not_empty(function<F2> const& f2) noexcept { return static_cast<bool>(f2); }
            template<typename T> constexpr static bool __not_empty(T const&) noexcept { return true; }
            static bool __manager(__data_store& dest, __data_store const& src, __func_manage_op op)
            {
                switch (op)
                {
                case __get_functor_ptr:
                    dest.__access<FT*>() = __get_pointer(src);
                    break;
                case __clone_functor:
                    __init_fn(dest, *const_cast<FT const*>(__get_pointer(src)));
                    break;
                case __destroy_functor:
                    __delete_wrapper(dest, __is_locally_storable{});
                    break;
                case __get_type_info:
                default:
                    break;
                }
                return false;
            }
        };
        using __manager_type = bool(*)(__data_store&, __data_store const&, __func_manage_op);
        __data_store __my_functor{};
        __manager_type __my_manager{};
        constexpr __function_base() noexcept = default;
        constexpr ~__function_base() noexcept { if(__my_manager) __my_manager(__my_functor, __my_functor, __destroy_functor); }
        constexpr bool __empty() const noexcept { return !__my_manager; }
    };
    template<typename ST, typename FT> class __function_helper;
    template<typename RT, typename FT, typename ... Args>
    class __function_helper<RT(Args...), FT> : public __function_base::__fn_manager<FT>
    {
        using __base = __function_base::__fn_manager<FT>;
    public:
        static bool __manager(__data_store& dest, __data_store const& src, __func_manage_op op)
        {
            if(op == __get_functor_ptr)
            {
                dest.__access<FT*>() = __base::__get_pointer(src);
                return false;
            }
            return __base::__manager(dest, src, op);
        }
        static RT __invoke_fn(__data_store const& fn, Args&& ... args) noexcept(is_nothrow_invocable_r_v<RT, FT, Args...>) { return __invoke_r<RT>(*(__base::__get_pointer(fn)), forward<Args>(args)...); }
        template<typename F2> constexpr static bool __is_nothrow_init() noexcept { return __and_<is_nothrow_constructible<F2>, typename __base::__is_locally_storable>::value; }
    };
    template<> class __function_helper<void, void> { public: constexpr static bool __manager(__data_store& dest, __data_store const& src, __func_manage_op op) { return false; } };
    template<typename S, typename F> class __target_helper;
    template<typename S, object F> class __target_helper<S, F> : public __function_helper<S, F>{};
    template<typename S, non_object F> class __target_helper<S, F> : public __function_helper<void, void>{};
    template<typename RT, typename ... Args>
    class function<RT(Args...)> : __function_base
    {
        template<not_self<function<RT(Args...)>> F> using __decay = decay_t<F>;
        template<typename FT, typename D = __decay<FT>> struct __is_callable : __is_invocable_impl<__invoke_result<D&, Args...>, RT>::type {};
        template<typename FT> using __helper = __function_helper<RT(Args...), __decay_t<FT>>;
        using __invoker_type = RT (*) (__data_store const&, Args&&...);
        __invoker_type __my_invoker = nullptr;
    public:
        typedef RT result_type;
        template<typename FT> requires (__is_callable<FT>::value)
        constexpr function(FT&& ft) noexcept(__helper<FT>::template __is_nothrow_init<FT>()) : __function_base{}
        {
            static_assert(is_copy_constructible<__decay_t<FT>>::value, "function target must be copy-constructible");
	        static_assert(is_constructible<__decay_t<FT>, FT>::value, "function target must be constructible from the constructor argument");
            using __my_helper = __helper<FT>;
            if(__my_helper::__not_empty(ft))
            {
                __my_helper::__init_fn(__my_functor, forward<FT>(ft));
                __my_invoker = &__my_helper::__invoke_fn;
                __my_manager = &__my_helper::__manager;
            }
        }
        constexpr operator bool() const noexcept { return !__function_base::__empty(); }
        constexpr function(function<RT(Args...)> const& that) : __function_base{} 
        {
            if(that.operator bool())
            {
                that.__my_manager(__my_functor, that.__my_functor, __clone_functor);
                __my_invoker = that.__my_invoker;
                __my_manager = that.__my_manager;
            }
        }
        constexpr function(function<RT(Args...)>&& that) : __function_base{}, __my_invoker{ that.__my_invoker }
        {
            if(that.operator bool())
            {
                __my_functor = that.__my_functor;
                __my_manager = that.__my_manager;
                that.__my_manager = nullptr;
                that.__my_invoker = nullptr;
            }
        }
        constexpr function() noexcept : __function_base{} {}
        template<typename FT> const FT* target() const noexcept 
        {
            if constexpr(is_object_v<FT>)
            {
                if(__my_manager == &__target_helper<RT(Args...), FT>::__manager)
                {
                    __data_store ptr;
                    __my_manager(ptr, __my_functor, __get_functor_ptr);
                    return ptr.__access<FT const*>();
                }
            }
            return nullptr;
        }
        template<typename FT> FT* target() noexcept { const function* __const_this = *this; const FT* __const_ft = __const_this->template target<FT>(); return *const_cast<FT**>(&__const_ft); }
        constexpr void swap(function& that) noexcept { std::swap(__my_functor, that.__my_functor); std::swap(__my_invoker, that.__my_invoker); std::swap(__my_manager, that.__my_manager); }
        constexpr function& operator=(function const& that) noexcept { function{ that }.swap(*this); return *this; }
        constexpr function& operator=(function&& that) noexcept { function{ move(that) }.swap(*this); return *this; }
        constexpr function& operator=(nullptr_t) noexcept { if(__my_manager) { __my_manager(__my_functor, __my_functor, __destroy_functor); __my_manager = nullptr; __my_invoker = nullptr; } return *this; }
        template<typename FT> requires (__is_callable<FT>::value) constexpr function& operator=(FT&& ft) noexcept(__helper<FT>::template __is_nothrow_init<FT>()) { function{ forward<FT>(ft) }.swap(*this); return *this; }
        RT operator()(Args ... args) const { return __my_invoker(__my_functor, forward<Args>(args)...); }
    };
    template<typename> struct __function_guide_helper{};
    template<typename RT, typename ST, bool NT, typename... Args> struct __function_guide_helper<RT (ST::*) (Args...) noexcept(NT)> { using type = RT (Args...); };
    template<typename FT, typename O> using __function_guide_t = typename __function_guide_helper<O>::type;
    template<typename RT, typename... Args> function(RT(*)(Args...)) -> function<RT(Args...)>;
    template<typename FT, typename ST = __function_guide_t<FT, decltype(&FT::operator())>> function(FT) -> function<ST>;
}
#endif