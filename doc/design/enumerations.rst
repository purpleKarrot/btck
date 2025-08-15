Enumerations
************

Enums in C
^^^^^^^^^^

The following code defines an enumerated type:

.. code-block:: c

  enum color { RED, GREEN, BLUE };

The ``enum color`` type has an unspecified width; it could be ``char`` or any
signed or unsigned integer type. The constants ``RED``, ``GREEN``, and ``BLUE``
are guaranteed to be of type ``int``.

The fact that the constants are of a type potentially distinct from the
enumerated type is surprising to many developers. However, the unspecified
width of the enumerated type is a real problem and makes enums unusable in a
portable C API, where the library may be used by an application built with a
different toolchain than the library.

Therefore, BtcK does not use the ``enum`` keyword to define enumerations in the
C API, but instead relies on a combination of ``typedef`` and ``#define``:

.. code-block:: c

  typedef uint8_t BtcK_ChainType;
  #define BtcK_ChainType_MAINNET   (BtcK_ChainType(0))
  #define BtcK_ChainType_TESTNET   (BtcK_ChainType(1))
  #define BtcK_ChainType_TESTNET_4 (BtcK_ChainType(2))
  #define BtcK_ChainType_SIGNET    (BtcK_ChainType(3))
  #define BtcK_ChainType_REGTEST   (BtcK_ChainType(4))

With this approach, all enumeration values must be specified explicitly, but
this is actually an advantage. It makes the code clearer and ensures every
value is intentional and documented, reducing ambiguity and making future
maintenance easier.

Enums in C++
^^^^^^^^^^^^

Using a ``typedef`` with an integral type in the C API has the additional
advantage that it can be used as the underlying type in the C++ wrapper:

.. code-block:: cpp

  namespace btck {

  enum chain_type : BtcK_ChainType {
    mainnet   = BtcK_ChainType_MAINNET,
    testnet   = BtcK_ChainType_TESTNET,
    testnet_4 = BtcK_ChainType_TESTNET_4,
    signet    = BtcK_ChainType_SIGNET,
    regtest   = BtcK_ChainType_REGTEST,
  };

  }  // namespace btck

This would not be possible if ``enum`` was used in the C API, as the underlying
type of an enum cannot be another enum. Therefore, the C API should retain this
approach even in C23.

Enums as Error Codes
^^^^^^^^^^^^^^^^^^^^

All enumerations used as error codes are accompanied by a function
``<prefix>_<enum>_Message`` that converts the numeric code into a descriptive
message. These messages are intended for debugging or logs, not for direct
display to end users. The messages should closely match the comments you would
put on the enum values.

.. code-block:: c

  typedef uint8_t BtcK_VerificationError;
  #define BtcK_VerificationError_TX_INPUT_INDEX ((BtcK_VerificationError)(1))
  #define BtcK_VerificationError_INVALID_FLAGS  ((BtcK_VerificationError)(2))
  ...

  char const* BtcK_VerificationError_Message(BtcK_VerificationError err) {
    switch (err) {
      case BtcK_VerificationError_TX_INPUT_INDEX:
        return "The provided input index is out of range of the "
               "actual number of inputs of the transaction.";

      case BtcK_VerificationError_INVALID_FLAGS:
        return "The provided bitfield for the flags was invalid.";

      ...

      default:
        return "(unrecognized error)";
    }
  }

The C++ wrapper defines an error category, overloads
``make_error_code``, and specializes ``std::is_error_code_enum`` for each error
code enumeration:

.. code-block:: cpp

  namespace btck {

  enum class verification_error : BtcK_VerificationError {
    tx_input_index = BtcK_VerificationError_TX_INPUT_INDEX,
    invalid_flags  = BtcK_VerificationError_INVALID_FLAGS,
    ...
  };

  std::error_category const& verification_error_category() {
    struct category_t : std::error_category {
      char const* name() const noexcept override {
        return "VerificationError";
      }
      std::string message(int ev) const override {
        return BtcK_VerificationError_Message(ev);
      }
    };

    static auto const category = category_t{};
    return category;
  }

  std::error_code make_error_code(verification_error err) {
    return {static_cast<int>(err), verification_error_category()};
  }

  }  // namespace btck

  template <>
  struct std::is_error_code_enum<btck::verification_error> : std::true_type {};

This allows the enum to be used in many ways supported by the
C++ standard library for `error handling
<https://en.cppreference.com/w/cpp/error.html>`_, such as throwing it as an
exception:

.. code-block:: cpp

  if ((flags & ~BtcK_VerificationFlags_ALL) != 0) {
    throw std::system_error(btck::verification_error::invalid_flags);
  }

Enums as Bitwise Flags
^^^^^^^^^^^^^^^^^^^^^^

In C, all enums are essentially integer types and thus allow unsafe and
unintended arithmetic operations, such as:

.. code-block:: c

  BtcK_VerificationFlags flags = BtcK_VerificationFlags_CHECKSEQUENCEVERIFY
    - BtcK_VerificationFlags_CHECKLOCKTIMEVERIFY
    + (BtcK_VerificationFlags_DERSIG * BtcK_VerificationFlags_NULLDUMMY);

In languages that support operator overloading, it is preferable to restrict
enums representing flags to bitwise operations only. In the C++ wrapper,
this is achieved with an ``is_flag_enum`` trait and operator definitions
constrained by the ``flag_enum`` concept:

.. code-block:: cpp

  namespace btck {

  template <typename E>
  struct is_flag_enum : std::false_type {};

  template <typename E>
  concept flag_enum = is_flag_enum<E>::value;

  }  // namespace btck

  template <btck::flag_enum E>
  constexpr E operator|(E left, E right) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(left) | static_cast<U>(right));
  }

  template <btck::flag_enum E>
  constexpr E operator&(E left, E right) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(left) & static_cast<U>(right));
  }

  template <btck::flag_enum E>
  constexpr E operator^(E left, E right) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(left) ^ static_cast<U>(right));
  }

  template <btck::flag_enum E>
  constexpr E operator~(E e) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<U>(e));
  }

  template <btck::flag_enum E>
  constexpr E& operator|=(E& left, E right) {
    left = left | right;
    return left;
  }

  template <btck::flag_enum E>
  constexpr E& operator&=(E& left, E right) {
    left = left & right;
    return left;
  }

  template <btck::flag_enum E>
  constexpr E& operator^=(E& left, E right) {
    left = left ^ right;
    return left;
  }

Once the ``is_flag_enum`` trait is specialized for an enum type, all bitwise
operators can be used:

.. code-block:: cpp

  template <>
  struct btck::is_flag_enum<btck::verification_flags> : std::true_type {};
