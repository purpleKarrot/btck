// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

[CCode (cheader_filename = "btck/btck.h")]
namespace BtcK {

[Compact]
[CCode (cname = "struct BtcK_Error", free_function = "BtcK_Error_Free")]
public class Error {
    [CCode (cname = "BtcK_Error_New")]
    public Error (string domain, int code, string message);
}

[Compact]
[CCode (cname = "struct BtcK_ScriptPubkey", free_function = "BtcK_ScriptPubkey_Free")]
public class ScriptPubkey {
    [CCode (cname = "BtcK_ScriptPubkey_New")]
    public ScriptPubkey (uchar[] raw);
}

[Compact]
[CCode (cname = "struct BtcK_TransactionOutput", free_function = "BtcK_TransactionOutput_Free")]
public class TransactionOutput {
    [CCode (cname = "BtcK_TransactionOutput_New")]
    public TransactionOutput (ScriptPubkey script_pubkey, int64 amount);

    public ScriptPubkey script_pubkey {
        [CCode (cname = "BtcK_TransactionOutput_GetScriptPubkey")]
        get;
    }

    public int64 amount {
        [CCode (cname = "BtcK_TransactionOutput_GetAmount")]
        get;
    }
}

[Compact]
[CCode (cname = "struct BtcK_Transaction", free_function = "BtcK_Transaction_Free")]
public class Transaction {
    [CCode (cname = "BtcK_Transaction_New")]
    public Transaction(uchar[] raw, out Error error);

    [CCode (cname = "BtcK_Transaction_ToString")]
    public string to_string ();

    [CCode (cname = "BtcK_Transaction_GetOutput")]
    public TransactionOutput get (size_t index);

    public size_t size {
        [CCode (cname = "BtcK_Transaction_CountOutputs")]
        get;
    }
}

[SimpleType]
[CCode (cname = "struct BtcK_BlockHash", has_type_id = false)]
public struct BlockHash {
    [CCode (cname = "BtcK_BlockHash_Init")]
    public BlockHash (uchar[] raw);
}

[Compact]
[CCode (cname = "struct BtcK_Block", free_function = "BtcK_Block_Free")]
public class Block {
    [CCode (cname = "BtcK_Block_New")]
    public Block (uchar[] raw, out Error error);

    [CCode (cname = "BtcK_Block_GetTransaction")]
    public Transaction get (size_t index);

    public size_t size {
        [CCode (cname = "BtcK_Block_CountTransactions")]
        get;
    }

    public BlockHash hash {
        [CCode (cname = "BtcK_Block_GetHash")]
        get;
    }
}

} // namespace BtcK
