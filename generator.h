/***
*generator
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*       Purpose: Library support of coroutines. generator class
*       http://open-std.org/JTC1/SC22/WG21/docs/papers/2015/p0057r0.pdf
*
*       [Public]
*
****/
#pragma once
#ifndef _EXPERIMENTAL_GENERATOR_
#define _EXPERIMENTAL_GENERATOR_
#ifndef RC_INVOKED

// #ifndef _RESUMABLE_FUNCTIONS_SUPPORTED
// #error <experimental/generator> requires /await compiler option
// #endif /* _RESUMABLE_FUNCTIONS_SUPPORTED */

#include "coroutine"

#pragma pack(push, _CRT_PACKING)
#pragma push_macro("new")
#undef new

// std::BEGIN

namespace std {

template<class T>
struct _always_fault {
    bool value = false;
};

namespace experimental {

    template <typename _Ty, typename _Alloc = std::allocator<char>>
    struct generator
    {
        struct promise_type
        {
            _Ty const *_CurrentValue;

            promise_type &get_return_object()
            {
                return *this;
            }

            bool initial_suspend()
            {
                return (true);
            }

            bool final_suspend()
            {
                return (true);
            }

            void yield_value(_Ty const &_Value)
            {
                _CurrentValue = std::addressof(_Value);
            }

            template <typename _Uty>
            _Uty && await_transform(_Uty &&_Whatever)
            {
                static_assert(_always_fault<_Uty>::value,
                    "co_await is not supported in coroutines of type std::experiemental::generator");
                return std::forward<_Uty>(_Whatever);
            }

            using _Alloc_traits = std::allocator_traits<_Alloc>;
            using _Alloc_of_char_type =
                typename _Alloc_traits::template rebind_alloc<char>;

            void *operator new(size_t _Size)
            {
                _Alloc_of_char_type _Al;
                return _Al.allocate(_Size);
            }

            void operator delete(void *_Ptr, size_t _Size) noexcept
            {
                _Alloc_of_char_type _Al;
                return _Al.deallocate(static_cast<char *>(_Ptr), _Size);
            }
        };

        struct iterator : std::iterator<std::input_iterator_tag, _Ty>
        {
            coroutine_handle<promise_type> _Coro;

            iterator(std::nullptr_t) : _Coro(nullptr)
            {
            }

            iterator(coroutine_handle<promise_type> _CoroArg) : _Coro(_CoroArg)
            {
            }

            iterator &operator++()
            {
                _Coro.resume();
                if (_Coro.done())
                    _Coro = nullptr;
                return *this;
            }

            iterator operator++(int) = delete;
            // generator iterator current_value
            // is a reference to a temporary on the coroutine frame
            // implementing postincrement will require storing a copy
            // of the value in the iterator.
            //{
            //  auto _Result = *this;
            //  ++(*this);
            //  return _Result;
            //}

            bool operator==(iterator const &_Right) const
            {
                return _Coro == _Right._Coro;
            }

            bool operator!=(iterator const &_Right) const
            {
                return !(*this == _Right);
            }

            _Ty const &operator*() const
            {
                return *_Coro.promise()._CurrentValue;
            }

            _Ty const *operator->() const
            {
                return std::addressof(operator*());
            }
        };

        iterator begin()
        {
            if (_Coro) {
                _Coro.resume();
                if (_Coro.done())
                    return {nullptr};
            }

            return {_Coro};
        }

        iterator end()
        {
            return {nullptr};
        }

        explicit generator(promise_type &_Prom)
            : _Coro(coroutine_handle<promise_type>::from_promise(
                  _Prom))
        {
        }

        generator() = default;

        generator(generator const &) = delete;

        generator &operator=(generator const &) = delete;

        generator(generator &&_Right) : _Coro(_Right._Coro)
        {
            _Right._Coro = nullptr;
        }

        generator &operator=(generator &&_Right)
        {
            if (&_Right != this) {
                _Coro = _Right._Coro;
                _Right._Coro = nullptr;
            }
            return *this;
        }

        ~generator()
        {
            if (_Coro) {
                _Coro.destroy();
            }
        }

    private:
        coroutine_handle<promise_type> _Coro = nullptr;
    };

} // namespace experimental

} // namespace std
// std::END

#pragma pop_macro("new")
#pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _EXPERIMENTAL_GENERATOR_ */
