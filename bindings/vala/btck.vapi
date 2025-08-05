// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

[CCode (cheader_filename = "btck/btck.h")]
namespace BtcK {

[Compact]
[CCode (cname = "struct BtcK_Error", free_function = "BtcK_Error_Free")]
private class InternalError {
    [CCode (cname = "BtcK_Error_New")]
    public InternalError (int code, string domain, string message);

    public int code { [CCode(cname="BtcK_Error_Code")] get; }
    public unowned string domain { [CCode(cname="BtcK_Error_Domain")] get; }
    public unowned string message { [CCode(cname="BtcK_Error_Message")] get; }
}

private void throw_if_error(InternalError error) throws GLib.Error
{
    if (error != null) {
        var quark = GLib.Quark.from_string(error.domain);
        throw new GLib.Error(quark, error.code, error.message);
    }
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
    public TransactionOutput(int64 amount, ScriptPubkey script_pubkey) throws GLib.Error
    {
        InternalError error;
        this.internal(amount, script_pubkey, out error);
        throw_if_error(error);
    }

    [CCode (cname = "BtcK_TransactionOutput_New")]
    private TransactionOutput.internal (int64 amount, ScriptPubkey script_pubkey, out InternalError error);

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
    public Transaction(uchar[] raw) throws GLib.Error
    {
        InternalError error;
        this.internal(raw, out error);
        throw_if_error(error);
    }

    [CCode (cname = "BtcK_Transaction_New")]
    private Transaction.internal(uchar[] raw, out InternalError error);

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
    public Block(uchar[] raw) throws GLib.Error
    {
        InternalError error;
        this.internal(raw, out error);
        throw_if_error(error);
    }

    [CCode (cname = "BtcK_Block_New")]
    private Block.internal (uchar[] raw, out InternalError error);

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
