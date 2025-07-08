// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

namespace BtcK {

[Compact]
[CCode (cname = "struct BtcK_ScriptPubkey", ref_function = "BtcK_ScriptPubkey_Retain", unref_function = "BtcK_ScriptPubkey_Release")]
public class ScriptPubkey {
    [CCode (cname = "BtcK_ScriptPubkey_New")]
    public ScriptPubkey (uchar[] raw);
}

[Compact]
[CCode (cname = "struct BtcK_TransactionOutput", ref_function = "BtcK_TransactionOutput_Retain", unref_function = "BtcK_TransactionOutput_Release")]
public class TransactionOutput {
    [CCode (cname = "BtcK_TransactionOutput_New")]
    public TransactionOutput (ScriptPubkey script_pubkey, int64 amount);
    public ScriptPubkey script_pubkey { [CCode (cname = "BtcK_TransactionOutput_GetScriptPubkey")] get; }
    public ScriptPubkey amount { [CCode (cname = "BtcK_TransactionOutput_GetAmount")] get; }
}

[Compact]
[CCode (cname = "struct BtcK_Transaction", ref_function = "BtcK_Transaction_Retain", unref_function = "BtcK_Transaction_Release")]
public class Transaction {
    [CCode (cname = "BtcK_ScriptPubkey_New")]
    public Transaction (uchar[] raw);
    [CCode (cname = "BtcK_Transaction_ToString")]
    public string to_string ();
    [CCode (cname = "BtcK_Transaction_At")]
    public unowned TransactionOutput get (int index);
    public int size { [CCode (cname = "BtcK_Transaction_GetSize")] get; }
}

[SimpleType]
[CCode (cname = "struct BtcK_BlockHash", has_type_id = false)]
public struct BlockHash {
    [CCode (cname = "BtcK_BlockHash_Init")]
    public BlockHash (uchar[] raw);
}

[Compact]
[CCode (cname = "struct BtcK_Block", ref_function = "BtcK_Block_Retain", unref_function = "BtcK_Block_Release")]
public class Block {
    [CCode (cname = "BtcK_Block_New")]
    public Block (uchar[] raw);
    [CCode (cname = "BtcK_Block_At")]
    public unowned Transaction get (int index);
    public int size { [CCode (cname = "BtcK_Block_GetSize")] get; }
    public BlockHash hash { [CCode (cname = "BtcK_Block_GetHash")] get; }
}

} // namespace BtcK
