Error Handling
**************

Error handling is the process of communicating information about an error from
where it occurs to where it is handled. In BtcK, this is especially important
because errors may need to be transported across different programming
languages.

Error information in BtcK consists of three components:

- a numeric code identifying the specific error,
- a domain that defines the context in which the code is meaningful (such as a
  module or subsystem),
- an optional message string providing further details.

The BtcK library represents errors using the ``BtcK_Error`` type, which
encapsulates all three components. This structure makes it straightforward to
transport and interpret error details across language boundaries.

In principle, there are three ways to store error information in a C API.
Simply signaling failure by returning ``NULL`` without communicating any error
details is not sufficient. The main approaches are:

- through the return value,
- through an output parameter, or
- in global or thread-local state accessed by a function like ``GetError()``.

BtcK uses output parameters for error information. This method provides clear
and explicit error reporting, avoids issues with global state, and works
reliably across language boundaries.

In the C API, functions that may fail include a parameter of type
``struct BtcK_Error**`` as their final argument. This allows callers to
optionally receive detailed error information. If the caller passes ``NULL`` for
this parameter, no error details are returned and only the success or failure of
the operation is signaled. If a non-null pointer is provided, the function will
allocate and populate a ``BtcK_Error`` object with the relevant error code,
domain, and message.

It is the caller’s responsibility to release any allocated error objects by
invoking ``BtcK_Error_Free`` once the error information is no longer needed.
This explicit ownership model allows callers to encapsulate the ``BtcK_Error``
within their own error handling abstractions, allowing integration with custom
error types or frameworks.
