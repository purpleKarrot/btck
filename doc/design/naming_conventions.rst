Naming Conventions
******************

.. tabs::

  .. code-tab:: c

    #include <btck/btck.h>

    int main() {
      struct BtcK_Transaction* transaction = BtcK_Transaction_New(data, sizeof(data), NULL);
      printf("outputs: %zu\n", BtcK_Transaction_CountOutputs(tx));
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
