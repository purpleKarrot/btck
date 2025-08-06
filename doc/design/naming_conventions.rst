Naming Conventions
******************

Consistent naming conventions make APIs easier to read and use, reduce
ambiguity, and help developers quickly recognize components. They also support
knowledge transfer, allowing experience in one language to carry over smoothly
to another.

Library Name
^^^^^^^^^^^^

The name ``BtcK`` was carefully selected to serve as the package, namespace, or
prefix across all supported programming languages, using the lowercase form
``btck`` where convention dictates. This choice ensures consistency and
recognizability in every language binding.

``BtcK`` is intentionally short, making it easy to type and remember. Its
brevity discourages developers from introducing their own aliases -- such as
``namespace bk = bitcoinkernel;`` -- which could fragment usage and reduce
clarity across projects.

Importantly, the name is unambiguous. Unlike more generic alternatives such as
``kernel``, ``BtcK`` clearly refers to the Bitcoin Kernel library, minimizing
confusion with other software components or libraries that might use similar
terminology. This distinctiveness helps maintain a clear identity for the
project in diverse codebases and documentation.

Identifier Names
^^^^^^^^^^^^^^^^

Identifier names in the C library follow a strict ``<prefix>_<type>`` or
``<prefix>_<type>_<member>`` naming convention. The ``<prefix>`` is always
``BtcK``, ensuring consistency and clear association with the library. Both
``<type>`` and ``<member>`` use PascalCase, which makes the three components --
prefix, type, and member -- easy to distinguish at a glance. For example,
``BtcK_Transaction_New`` and ``BtcK_Transaction_CountOutputs` adhere to this
pattern.

In higher-level languages, identifier naming follows the conventions of each
language. Depending on the language, this may mean using camelCase, PascalCase,
or snake_case, so that the API feels idiomatic and natural to users of that
language.

Examples
^^^^^^^^

The following examples demonstrate how the BtcK naming conventions are applied
across different programming languages. Each snippet shows how to create a
transaction and access its outputs using the idiomatic style of the respective
language.

.. tabs::

  .. code-tab:: c

    #include <btck/btck.h>

    int main() {
      struct BtcK_Transaction* transaction = BtcK_Transaction_New(data, sizeof(data), NULL);
      printf("outputs: %zu\n", BtcK_Transaction_CountOutputs(tx));
      BtcK_Transaction_Free(tx);
    }

  .. code-tab:: cpp

    #include <btck/btck.hpp>

    int main() {
      auto const tx = btck::transaction(data);
      std::println("outputs: {}", tx.outputs().size());
    }

  .. code-tab:: go

    import "btck"

    func main() {
      tx, _ := btck.NewTransaction(data)
      fmt.Println("outputs:", tx.CountOutputs())
    }

  .. code-tab:: py

    import btck

    tx = btck.Transaction(data)
    print("outputs:", len(tx.outputs))

  .. code-tab:: js

    const btck = require("btck.node");

    const tx = new btck.Transaction(data);
    console.log("outputs:", tx.outputs.length);

  .. code-tab:: rs

    fn main() {
      let tx = btck::Transaction::new(data).unwrap();
      println!("outputs: {}", tx.outputs().count());
    }

  .. code-tab:: swift

    import BtcK

    let tx = try Transaction(raw: data)
    print("outputs:", tx.outputs.count)
