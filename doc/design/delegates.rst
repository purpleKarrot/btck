Delegates / Callbacks
*********************

Unowned Delegates
^^^^^^^^^^^^^^^^^

The following C++ code demonstrates a higher-order function:

.. code-block:: cpp

   template <typename F>
   void foo(std::initializer_list<int> args, F fun) {
       for (auto const& elem : args) {
           fun(elem);
       }
   }

Suppose we want to move the implementation of this function to a library
that only provides a C API. We then want to provide a thin C++ wrapper for
that library. An initial draft might look like this:

.. code-block:: cpp

   extern "C" void foo_c(
       int const* args, size_t args_len,
       void(*fun)(void*, int), void* fun_user);

   template <typename F>
   void foo(std::initializer_list<int> args, F fun) {
       auto cb = +[](void* user, int arg) {
           auto& f = *reinterpret_cast<F*>(user);
           f(arg);
       };

       foo_c(std::data(args), std::size(args), cb, &fun);
   }

The ``std::initializer_list<int>`` argument is translated to a pointer and a
size, and the function argument is translated to a function pointer with an
additional ``void*`` parameter, which allows passing the address of the
function object. This approach should be familiar if you have worked with C
APIs before.

What may not be familiar is the unary ``+`` in front of the lambda expression.
The unary ``+`` operator is sometimes called the "integer promotion operator"
and is commonly used in C++ to explicitly trigger `integer promotion
<https://en.cppreference.com/w/c/language/conversion.html#Integer_promotions>`_,
such as converting a ``char`` to an ``int``. In the context of lambdas, placing
a unary ``+`` in front of a captureless lambda explicitly converts it to a
function pointer type, which is required for C APIs that expect a plain function
pointer.

We translated the return type and the argument types from C++ into their C
equivalents. However, we did not translate information that is not explicitly
mentioned in the code from C++ to C: for example, the fact that the callback
function was **not marked** ``noexcept``.

What happens when we call the function like this?

.. code-block:: cpp

   int main()
   {
       auto strings = std::vector{"hello", "goodbye", "kaesekuchen"};

       try {
           foo({2, 1, 4}, [&](int idx) {
               std::println("{}", strings.at(idx));
           });
       }
       catch (std::exception& err) {
           std::println("ERROR: {}", err.what());
       }
   }

Before the C API was introduced, exceptions thrown in the callback function
were propagated properly. To fix this, we need to catch exceptions thrown in the
callback and then rethrow them in the higher-order function:

.. code-block:: cpp

   extern "C" int foo_c(
       int const* args, size_t args_len,
       void (*fun)(void*, int), void* fun_user);

   template <typename F>
   void foo(std::initializer_list<int> args, F fun) {
       struct closure_t {
           F fun_;
           std::exception_ptr err_;
       };

       auto closure = closure_t{
           .fun_ = std::move(fun),
           .err_ = nullptr,
       };

       auto cb = +[](void* user, int arg) {
           auto& c = *reinterpret_cast<closure_t*>(user);
           try {
               c.fun_(arg);
               return 0;
           }
           catch (...) {
               c.err_ = std::current_exception();
               return -1;
           }
       };

       if (foo_c(std::data(args), std::size(args), cb, &closure) < 0) {
           std::rethrow_exception(closure.err_);
       }
   }

In this example, we assume that the only reason the higher-order function may
fail is because an exception was raised in the callback function. It is
therefore sufficient to indicate failure with ``-1`` and to propagate the actual
error information out of band.

Owned Delegates
^^^^^^^^^^^^^^^

Imagine we don't want to pass a callback to a higher-order function, but instead
store it on a class instance that may call it later. Consider the following C++
example:

.. code-block:: cpp

   class gadget
   {
   public:
       using log_fn = std::function<void(std::string_view)>;

       void set_logger(log_fn lf) {
           this->log_ = std::move(lf);
       }

   private:
       log_fn log_;
   };

Now suppose ``gadget`` needs to be exposed to a C API. What should be the
signature of the ``Gadget_SetLogger`` function? If it just takes a function
pointer and a userdata pointer, we have a problem in the C++ wrapper:

.. code-block:: cpp

   class gadget
   {
   public:
       template <typename LogFn>
       void set_logger(LogFn lf) {
           auto cb = +[](char const* str, size_t len, void* user) {
               auto& f = *reinterpret_cast<LogFn*>(user);
               f(std::string_view{str, len});
           };
           Gadget_SetLogger(impl_.get(), cb, &lf); // BUG HIDDEN HERE
       }

   private:
       struct deleter {
           void operator()(Gadget* g) { Gadget_Free(g); )
       };
       std::unique_ptr<Gadget, deleter> impl_;
   };

Owned callbacks cannot be represented with just two pointers; they need three:
a function pointer, userdata, and a destructor for the userdata:

.. code-block:: cpp

   extern "C" void Gadget_SetLogger(struct Gadget* self,
       void(*fun)(void*, char const*, size_t),
       void* userdata,
       void(*destroy)(void*));

   template <typename LogFn>
   void gadget::set_logger(LogFn lf) {
       auto cb = +[](void* user, char const* str, size_t len) {
           auto& f = *reinterpret_cast<LogFn*>(user);
           f(std::string_view{str, len});
       };
       auto del = +[](void* user) {
           delete reinterpret_cast<LogFn*>(user);
       };
       auto user = new LogFn(std::move(lf);
       Gadget_SetLogger(impl_.get(), cb, user, del);
   }

That solves the ownership, but it does not solve error propagation. We cannot
make the same assumption about failures as in higher-order functions, so we need
a way to translate C++ exceptions into the error type provided by our library:

.. code-block:: cpp

   auto translate_current_exception() -> Error*
   {
     try {
       throw;
     }
     catch (std::bad_alloc const& e) {
       return Error_New(-1, "Memory", e.what());
     }
     catch (std::system_error const& e) {
       auto const& code = e.code();
       return Error_New(code.value(), code.category().name(), e.what());
     }
     catch (std::exception const& e) {
       return Error_New(-1, "Unknown", e.what());
     }
     catch (...) {
       return Error_New(-1, "Unknown", "Unknown exception");
     }
   }

Then we can use that in a wrapper like this:

.. code-block:: cpp

   extern "C" void Gadget_SetLogger(struct Gadget* self,
       int(*fun)(void*, char const*, size_t, Error** err),
       void* userdata,
       void(*destroy)(void*));

   template <typename LogFn>
   void gadget::set_logger(LogFn lf) {
       auto cb = +[](void* user, char const* str, size_t len, Error** err) {
           try {
               auto& f = *reinterpret_cast<LogFn*>(user);
               f(std::string_view{str, len});
               return 0;
           }
           catch (...) {
               if (err != NULL) {
                   Error_Free(*err);
                   *err = translate_current_exception();
               }
               return -1;
           }
       };
       auto del = +[](void* user) {
           delete reinterpret_cast<LogFn*>(user);
       };
       auto user = new LogFn(std::move(lf);
       Gadget_SetLogger(impl_.get(), cb, user, del);
   }

Or, we can wrap the error capture into a helper function:

.. code-block:: cpp

   extern "C" void Gadget_SetLogger(struct Gadget* self,
       int(*fun)(void*, char const*, size_t, Error** err),
       void* userdata,
       void(*destroy)(void*));

   template <typename F>
   int capture_error(Error** err, F&& function)
   {
       try {
           function();
           return 0;
       }
       catch (...) {
           if (err != nullptr) {
               Error_Free(*err);
               *err = translate_current_exception();
           }
           return -1;
       }
   }

   template <typename LogFn>
   void gadget::set_logger(LogFn lf) {
       auto cb = +[](void* user, char const* str, size_t len, Error** err) {
           return capture_error(err, [=] {
               auto& f = *reinterpret_cast<LogFn*>(user);
               f(std::string_view{str, len});
           });
       };
       auto del = +[](void* user) {
           delete reinterpret_cast<LogFn*>(user);
       };
       auto user = new LogFn(std::move(lf);
       Gadget_SetLogger(impl_.get(), cb, user, del);
   }
