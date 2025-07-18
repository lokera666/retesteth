#include "TransactionBaseFee.h"
#include <libdevcore/CommonIO.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Common.h>
#include <retesteth/EthChecks.h>
#include <retesteth/Options.h>
#include <retesteth/helpers/TestHelper.h>
#include <retesteth/helpers/TestOutputHelper.h>
#include <retesteth/testStructures/Common.h>

using namespace std;
using namespace dev;

namespace test::teststruct
{

void TransactionBaseFee::checkDataScheme(DataObject const& _data) const
{
    REQUIRE_JSONFIELDS(_data, "TransactionBaseFee " + _data.getKey(),
        {
            {"data", {{DataType::String}, jsonField::Required}},
            {"gasLimit", {{DataType::String}, jsonField::Required}},
            {"gasPrice", {{DataType::String}, jsonField::Optional}},
            {"nonce", {{DataType::String}, jsonField::Required}},
            {"value", {{DataType::String}, jsonField::Required}},
            {"to", {{DataType::String, DataType::Null}, jsonField::Required}},
            {"secretKey", {{DataType::String}, jsonField::Optional}},
            {"sender", {{DataType::String}, jsonField::Optional}},
            {"v", {{DataType::String}, jsonField::Optional}},
            {"r", {{DataType::String}, jsonField::Optional}},
            {"s", {{DataType::String}, jsonField::Optional}},

            // Transaction type 1
            {"type", {{DataType::String}, jsonField::Optional}},
            {"chainId", {{DataType::String, DataType::Null}, jsonField::Optional}},
            {"accessList", {{DataType::Array}, jsonField::Required}},

            // Transaction type 2
            {"maxFeePerGas", {{DataType::String}, jsonField::Required}},
            {"maxPriorityFeePerGas", {{DataType::String}, jsonField::Required}},

            {"publicKey", {{DataType::String}, jsonField::Optional}},  // Besu EthGetBlockBy transaction
            {"raw", {{DataType::String}, jsonField::Optional}},        // Besu EthGetBlockBy transaction

            {"blockHash", {{DataType::String}, jsonField::Optional}},         // EthGetBlockBy transaction
            {"blockNumber", {{DataType::String}, jsonField::Optional}},       // EthGetBlockBy transaction
            {"from", {{DataType::String}, jsonField::Optional}},              // EthGetBlockBy transaction
            {"hash", {{DataType::String}, jsonField::Optional}},              // EthGetBlockBy transaction
            {"transactionIndex", {{DataType::String}, jsonField::Optional}},  // EthGetBlockBy transaction
            {"expectException", {{DataType::Object}, jsonField::Optional}}    // BlockchainTest filling
        });
}

void TransactionBaseFee::_fromData(DataObject const& _data)
{
    m_accessList = spAccessList(new AccessList(_data.atKey("accessList")));
    m_maxFeePerGas = sVALUE(_data.atKey("maxFeePerGas"));
    m_maxPriorityFeePerGas = sVALUE(_data.atKey("maxPriorityFeePerGas"));

    m_data = sBYTES(_data.atKey("data"));
    m_gasLimit = sVALUE(_data.atKey("gasLimit"));
    m_nonce = sVALUE(_data.atKey("nonce"));
    m_value = sVALUE(_data.atKey("value"));
    if (_data.count("sender"))
        m_sender = sFH20(_data.atKey("sender"));

    if (_data.count("chainId"))
        m_chainID = sVALUE(_data.atKey("chainId"));

    if (_data.atKey("to").type() == DataType::Null || _data.atKey("to").asString().empty())
        m_creation = true;
    else
    {
        m_creation = false;
        m_to = sFH20(_data.atKey("to"));
    }
}

TransactionBaseFee::TransactionBaseFee(dev::RLP const& _rlp) : Transaction()
{
    fromRLP(_rlp);
}

TransactionBaseFee::TransactionBaseFee(BYTES const& _rlp) : Transaction()
{
    dev::bytes decodeRLP = sfromHex(_rlp.asString());
    dev::RLP rlp(decodeRLP, dev::RLP::VeryStrict);
    fromRLP(rlp);
}

void TransactionBaseFee::fromRLP(dev::RLP const& _rlp)
{
    if (_rlp.itemCount() != _rlpHeaderSize())
        throw test::UpwardsException(TransactionTypeToString(type())
                                     + "::fromRLP(RLP) expected to have exactly "
                                     + test::fto_string(_rlpHeaderSize()) + " elements!");

    size_t i = 0;
    m_chainID = sVALUE(_rlp[i++]);

    m_nonce = sVALUE(_rlp[i++]);
    m_maxPriorityFeePerGas = sVALUE(_rlp[i++]);
    m_maxFeePerGas = sVALUE(_rlp[i++]);
    m_gasLimit = sVALUE(_rlp[i++]);

    auto const r = _rlp[i++];
    std::ostringstream stream;
    stream << r.toBytes();
    m_creation = false;
    if (stream.str() == "0x")
        m_creation = true;
    else
        m_to = sFH20(r);

    m_value = sVALUE(_rlp[i++]);
    m_data = sBYTES(_rlp[i++]);

    // read access list
    m_accessList = spAccessList(new AccessList(_rlp[i++]));

    m_v = sVALUE(_rlp[i++]);
    m_r = sVALUE(_rlp[i++]);
    m_s = sVALUE(_rlp[i++]);

    m_secretKey = sVALUE(0);
    rebuildRLP();
}

dev::h256 TransactionBaseFee::buildVRSHash() const
{
    dev::RLPStream stream;
    stream.appendList(9);
    streamHeader(stream);

    // Alter output with prefixed 02 byte + tr.rlp
    dev::bytes outa = stream.out();
    outa.insert(outa.begin(), dev::byte(2));  // txType
    return dev::sha3(outa);
}

void TransactionBaseFee::streamHeader(dev::RLPStream& _s) const
{
    // rlp([chainId, nonce, maxPriorityFeePerGas, maxFeePerGas, gasLimit, to, value, data, access_list, y_parity, r, s])
    _s << m_chainID->asBigInt();

    _s << nonce().serializeRLP();
    _s << m_maxPriorityFeePerGas->serializeRLP();
    _s << m_maxFeePerGas->serializeRLP();
    _s << gasLimit().serializeRLP();
    if (Transaction::isCreation())
        _s << "";
    else
        _s << to().serializeRLP();
    _s << value().serializeRLP();
    _s << test::sfromHex(data().asString());

    // Access Listist
    dev::RLPStream accessList(m_accessList->list().size());
    for (auto const& el : m_accessList->list())
        accessList.appendRaw(el->asRLPStream().out());

    _s.appendRaw(accessList.out());
}

const spDataObject TransactionBaseFee::asDataObject(ExportOrder _order) const
{
    // Because we don't use gas_price field need to explicitly output
    spDataObject out;
    (*out)["data"] = m_data->asString();
    (*out)["gasLimit"] = m_gasLimit->asString();
    (*out)["nonce"] = m_nonce->asString();
    if (m_creation)
    {
        if (_order != ExportOrder::ToolStyle)
            (*out)["to"] = "";
    }
    else
        (*out)["to"] = m_to->asString();
    (*out)["value"] = m_value->asString();
    (*out)["v"] = m_v->asString();
    (*out)["r"] = m_r->asString();
    (*out)["s"] = m_s->asString();
    auto const sender_key = sender();
    if (sender_key != FH20::zero())
        (*out)["sender"] = sender_key.asString();

    if (_order == ExportOrder::ToolStyle)
    {
        (*out).performModifier(mod_removeLeadingZerosFromHexValues, DataObject::ModifierOption::RECURSIVE, {"data", "to"});
        (*out).renameKey("gasLimit", "gas");
        (*out).renameKey("data", "input");
        if (!m_secretKey.isEmpty() && m_secretKey.getCContent() != 0)
            (*out)["secretKey"] = m_secretKey->asString();
    }

    // standard transaction output without gas_price end
    // begin eip1559 transaction info
    (*out)["chainId"] = m_chainID->asString();
    (*out)["type"] = "0x02";
    (*out)["maxFeePerGas"] = m_maxFeePerGas->asString();
    (*out)["maxPriorityFeePerGas"] = m_maxPriorityFeePerGas->asString();
    if (_order == ExportOrder::ToolStyle)
    {
        DataObject chainIDs(m_chainID->asString());
        chainIDs.performModifier(mod_removeLeadingZerosFromHexValues);
        (*out)["chainId"] = chainIDs.asString();
        (*out)["type"] = "0x2";

        spDataObject t8ntoolFields;
        (*t8ntoolFields)["maxFeePerGas"] = m_maxFeePerGas->asString();
        (*t8ntoolFields)["maxPriorityFeePerGas"] = m_maxPriorityFeePerGas->asString();
        (*t8ntoolFields).performModifier(mod_removeLeadingZerosFromHexValues);
        (*out)["maxFeePerGas"] = (*t8ntoolFields)["maxFeePerGas"].asString();
        (*out)["maxPriorityFeePerGas"] = (*t8ntoolFields)["maxPriorityFeePerGas"].asString();

        if (!m_secretKey.isEmpty() && m_secretKey.getCContent() != 0)
            (*out)["secretKey"] = m_secretKey->asString();
    }
    (*out).atKeyPointer("accessList") = m_accessList->asDataObject();
    (*out).performModifier(mod_removeBigIntHint);
    return out;
}

void TransactionBaseFee::rebuildRLP()
{
    // RLP(02 + tr.rlp)
    dev::RLPStream wrapper;
    dev::RLPStream out;
    out.appendList(_rlpHeaderSize());
    TransactionBaseFee::streamHeader(out);
    out << v().serializeRLP();
    out << r().serializeRLP();
    out << s().serializeRLP();

    // Alter output with prefixed 02 byte + tr.rlp
    dev::bytes outa = out.out();
    outa.insert(outa.begin(), dev::byte(2));

    // Encode bytearray into rlp
    wrapper << outa;
    m_outRlpStream = wrapper;
    m_rawRLPdata = spBYTES(new BYTES(dev::toHexPrefixed(outa)));
    m_hash = spFH32(new FH32("0x" + dev::toString(dev::sha3(outa))));
}


}  // namespace teststruct
