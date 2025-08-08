Memory Management
*****************

BtcK enables clients to use a different standard library than the one used to
build the library itself. For example, the library may be built with GCC, while
the client application uses MSVC. As a result, memory allocated by the library
with ``malloc`` or ``new`` cannot be safely deallocated by the client with
``free`` and ``delete``, and vice versa.

Single objects that are instances of types provided by BtcK are allocated using
a constructor function, ``BtcK_<type>_New``, and deallocated with the
corresponding destructor function, ``BtcK_<type>_Free``. See
:ref:`object-lifetime` for more details about the lifetime of single objects.

There are two types that BtcK deliberately does not provide: ``String`` and
``Buffer``.

ToString
^^^^^^^^

Many types in BtcK provide a function to convert an object to a string. In C,
this function is called ``BtcK_<type>_ToString``, while higher-level languages
may use special methods such as ``__str__``. For the reasons highlighted above,
this function cannot return a ``char*`` that must be freed with ``free``.

One approach would be to introduce a dedicated string type, ``BtcK_String``,
with ``data`` and ``size`` members and a corresponding destructor function
``BtcK_String_Free``. The problem with that approach is that language bindings
would need to convert from this string type into their own string type using a
function like :c:func:`PyUnicode_FromStringAndSize`, which would involve
additional memory allocations.

Instead, the ``_ToString`` functions do not allocate any memory at all, but
write into a buffer that is provided by the caller. They then return the number
of characters that would have been written if the buffer had been sufficiently
large. This API, which is modelled after ``snprintf``, allows clients to use a
static buffer for small strings and only fall back to dynamic allocation when
the static buffer is not large enough. It also allows passing ``NULL`` as the
buffer in order to determine the required buffer length.

For an example of client-side usage, this is how the Python bindings determine
the required length and then use that information to allocate and write into a
string:

.. code-block:: c

   PyObject* tx_string(struct BtcK_Transaction const* tx)
   {
       int const length = BtcK_Transaction_ToString(tx, NULL, 0);
       if (length < 0) {
           PyErr_SetNone(PyExc_ValueError);
           return NULL;
       }

       PyObject* ret = PyUnicode_New(length, 127);
       if (ret == NULL) {
           return NULL;
       }

       BtcK_Transaction_ToString(tx, PyUnicode_DATA(ret), length + 1);
       return ret;
   }

On the implementation side, if the ``_ToString`` function is implemented in
terms of ``snprintf``, allowing ``NULL`` as the buffer and returning the
theoretical length rather than the number of bytes actually written is natural.

If the function is implemented in terms of :cpp:func:`std::format`, it should
write into the buffer with :cpp:func:`std::format_to_n` (see `cppreference
<https://en.cppreference.com/w/cpp/utility/format/format_to_n.html>`__)
only if the buffer is not ``nullptr``, but should always return
:cpp:func:`std::formatted_size` (see `cppreference
<https://en.cppreference.com/w/cpp/utility/format/formatted_size.html>`__).

If the function currently constructs a ``std::string`` by other means, such as
using ``std::stringstream`` or by repeatedly calling ``+=`` or ``append``, it
should copy the string into the buffer with :cpp:func:`std::string::copy`
(see `cppreference <https://en.cppreference.com/w/cpp/string/basic_string/copy>`__)
and then return the result of :cpp:func:`std::string::size`
(see `cppreference <https://en.cppreference.com/w/cpp/string/basic_string/size>`__).
With such an implementation and the example code above, the same string will be
constructed twice. It is therefore recommended to refactor the implementation
using the C++20 format library. Such a refactoring is possible without affecting
the API of the function.

ToBytes
^^^^^^^

Many types in BtcK also provide a function to convert an object to a buffer. In
C, this function is called ``BtcK_<type>_ToBytes``, while higher-level languages
may use special methods such as ``__bytes__``. Like ``_ToString``, this
function cannot return memory that must be deallocated with ``free``. It is
also not desirable to introduce a ``BtcK_Buffer`` type that would require
copying from BtcK's buffer into the language's buffer type with a function
like :c:func:`PyBytes_FromStringAndSize`, which would result in additional
memory allocations.

But unlike ``_ToString``, there is no easy way to predetermine the required
size of a buffer. Hence, BtcK uses an approach that is inspired by and
compatible with `PEP 782 <https://peps.python.org/pep-0782>`__. The following
extract from the Python bindings shows an example of the client-side usage:

.. code-block:: c

   static int write_bytes(void const* bytes, size_t size, void* writer)
   {
       return PyBytesWriter_WriteBytes(
           (PyBytesWriter*)writer, bytes, (Py_ssize_t)size);
   }

   PyObject* tx_bytes(struct BtcK_Transaction const* tx)
   {
       PyBytesWriter* writer = PyBytesWriter_Create(0);
       if (BtcK_Transaction_ToBytes(tx, write_bytes, writer) != 0) {
           PyBytesWriter_Discard(writer);
           return NULL;
       }

       return PyBytesWriter_Finish(writer);
   }
