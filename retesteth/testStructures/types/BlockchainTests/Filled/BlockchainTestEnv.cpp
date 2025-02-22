#include "BlockchainTestEnv.h"
#include <retesteth/EthChecks.h>
#include <retesteth/testStructures/Common.h>
namespace test
{
namespace teststruct
{
// Env. Build a type representation
BlockchainTestEnv::BlockchainTestEnv(DataObject const& _data)
{
    try
    {
        if (!_data.count("baseFeePerGas"))
        {
            // Legacy header Allowed fields for this structure
            REQUIRE_JSONFIELDS(_data, "GenesisBlockHeader(BlockchainTestEnv) " + _data.getKey(),
                {{"bloom", {{DataType::String}, jsonField::Optional}},
                    {"logsBloom", {{DataType::String}, jsonField::Optional}},
                    {"coinbase", {{DataType::String}, jsonField::Optional}},
                    {"author", {{DataType::String}, jsonField::Optional}},
                    {"miner", {{DataType::String}, jsonField::Optional}},
                    {"difficulty", {{DataType::String}, jsonField::Required}},
                    {"extraData", {{DataType::String}, jsonField::Required}},
                    {"gasLimit", {{DataType::String}, jsonField::Required}},
                    {"gasUsed", {{DataType::String}, jsonField::Required}},
                    {"hash", {{DataType::String}, jsonField::Optional}},
                    {"mixHash", {{DataType::String}, jsonField::Optional}},
                    {"nonce", {{DataType::String}, jsonField::Optional}},
                    {"number", {{DataType::String}, jsonField::Required}},
                    {"parentHash", {{DataType::String}, jsonField::Required}},
                    {"receiptTrie", {{DataType::String}, jsonField::Optional}},
                    {"receiptsRoot", {{DataType::String}, jsonField::Optional}},
                    {"stateRoot", {{DataType::String}, jsonField::Required}},
                    {"timestamp", {{DataType::String}, jsonField::Required}},
                    {"transactionsTrie", {{DataType::String}, jsonField::Optional}},
                    {"transactionsRoot", {{DataType::String}, jsonField::Optional}},
                    {"sha3Uncles", {{DataType::String}, jsonField::Optional}},
                    {"uncleHash", {{DataType::String}, jsonField::Optional}}});
        }
        else
        {
            // Allowed fields for this structure
            REQUIRE_JSONFIELDS(_data, "GenesisBlockHeader(BlockchainTestEnv) " + _data.getKey(),
                {{"bloom", {{DataType::String}, jsonField::Optional}},
                    {"logsBloom", {{DataType::String}, jsonField::Optional}},
                    {"coinbase", {{DataType::String}, jsonField::Optional}},
                    {"author", {{DataType::String}, jsonField::Optional}},
                    {"miner", {{DataType::String}, jsonField::Optional}},
                    {"difficulty", {{DataType::String}, jsonField::Required}},
                    {"extraData", {{DataType::String}, jsonField::Required}},
                    {"gasLimit", {{DataType::String}, jsonField::Required}},
                    {"baseFeePerGas", {{DataType::String}, jsonField::Required}},
                    {"gasUsed", {{DataType::String}, jsonField::Required}},
                    {"hash", {{DataType::String}, jsonField::Optional}},
                    {"mixHash", {{DataType::String}, jsonField::Optional}},
                    {"nonce", {{DataType::String}, jsonField::Optional}},
                    {"number", {{DataType::String}, jsonField::Required}},
                    {"parentHash", {{DataType::String}, jsonField::Required}},
                    {"receiptTrie", {{DataType::String}, jsonField::Optional}},
                    {"receiptsRoot", {{DataType::String}, jsonField::Optional}},
                    {"stateRoot", {{DataType::String}, jsonField::Required}},
                    {"timestamp", {{DataType::String}, jsonField::Required}},
                    {"transactionsTrie", {{DataType::String}, jsonField::Optional}},
                    {"transactionsRoot", {{DataType::String}, jsonField::Optional}},
                    {"sha3Uncles", {{DataType::String}, jsonField::Optional}},
                    {"uncleHash", {{DataType::String}, jsonField::Optional}}});

            // EIP1559 style
            m_currentBaseFee = spVALUE(new VALUE(_data.atKey("baseFeePerGas")));
        }

        m_currentCoinbase = spFH20(new FH20(_data.atKey("coinbase")));
        m_currentDifficulty = spVALUE(new VALUE(_data.atKey("difficulty")));
        m_currentNumber = spVALUE(new VALUE(_data.atKey("number")));
        m_currentTimestamp = spVALUE(new VALUE(_data.atKey("timestamp")));
        m_previousHash = spFH32(new FH32(_data.atKey("parentHash")));
        m_currentExtraData = spBYTES(new BYTES(_data.atKey("extraData")));
        m_currentNonce = spFH8(new FH8(_data.atKey("nonce")));
        m_currentMixHash = spFH32(new FH32(_data.atKey("mixHash")));
        m_currentGasLimit = spVALUE(new VALUE(_data.atKey("gasLimit")));
    }
    catch (std::exception const& _ex)
    {
        throw UpwardsException(string("BlockchainTestEnv convertion error: ") + _ex.what() + _data.asJson());
    }
}

}  // namespace teststruct
}  // namespace test
