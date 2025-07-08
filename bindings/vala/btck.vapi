// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

namespace btck {

[Compact]
[CCode (cname = "struct btck_ScriptPubkey", ref_function = "btck_ScriptPubkey_Retain", unref_function = "btck_ScriptPubkey_Release")]
public class ScriptPubkey {
    [CCode (cname = "btck_ScriptPubkey_New")]
    public ScriptPubkey (uchar[] raw);
}

[Compact]
[CCode (cname = "struct btck_TransactionOutput", ref_function = "btck_TransactionOutput_Retain", unref_function = "btck_TransactionOutput_Release")]
public class TransactionOutput {
    [CCode (cname = "btck_TransactionOutput_New")]
    public TransactionOutput (ScriptPubkey script_pubkey, int64 amount);
    public ScriptPubkey script_pubkey { [CCode (cname = "btck_TransactionOutput_GetScriptPubkey")] get; }
    public ScriptPubkey amount { [CCode (cname = "btck_TransactionOutput_GetAmount")] get; }
}

[Compact]
[CCode (cname = "struct btck_Transaction", ref_function = "btck_Transaction_Retain", unref_function = "btck_Transaction_Release")]
public class Transaction {
    [CCode (cname = "btck_ScriptPubkey_New")]
    public Transaction (uchar[] raw);
    [CCode (cname = "btck_Transaction_ToString")]
    public string to_string ();
    [CCode (cname = "btck_Transaction_At")]
    public unowned TransactionOutput get (int index);
    public int size { [CCode (cname = "btck_Transaction_GetSize")] get; }
}

[SimpleType]
[CCode (cname = "struct btck_BlockHash", has_type_id = false)]
public struct BlockHash {
    [CCode (cname = "btck_BlockHash_Init")]
    public BlockHash (uchar[] raw);
}

[Compact]
[CCode (cname = "struct btck_Block", ref_function = "btck_Block_Retain", unref_function = "btck_Block_Release")]
public class Block {
    [CCode (cname = "btck_Block_New")]
    public Block (uchar[] raw);
    [CCode (cname = "btck_Block_At")]
    public unowned Transaction get (int index);
    public int size { [CCode (cname = "btck_Block_GetSize")] get; }
    public BlockHash hash { [CCode (cname = "btck_Block_GetHash")] get; }
}

} // namespace btck
