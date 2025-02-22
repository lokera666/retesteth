#pragma once
#include <retesteth/TestHelper.h>

#include <retesteth/testStructures/basetypes/VALUE.h>
#include <retesteth/testStructures/configs/ClientConfigFile.h>
#include <retesteth/testStructures/configs/FORK.h>

#include <boost/asio.hpp>
#include <mutex>
#include <string>
namespace fs = boost::filesystem;
using namespace test::teststruct;

namespace test
{
struct ClientConfigID
{
    /// ClientConfigID handles the unique id logic so not to store it inside int and accedentially
    /// change or mistake with some other value. ???possibly a class for unique object ids???
    ClientConfigID();
    bool operator==(ClientConfigID const& _right) const { return m_id == _right.id(); }
    bool operator!=(ClientConfigID const& _right) const { return m_id != _right.id(); }
    unsigned id() const { return m_id; }
    static ClientConfigID null()
    {
        static ClientConfigID id;
        return id;
    }

private:
    unsigned m_id;
};

// Replace fields according to the client configs if needed
enum class FieldReplaceDir
{
    ClientToRetesteth,
    RetestethToClient
};

class ClientConfig
{
public:
    ClientConfig(fs::path const& _clientConfigPath);
    ClientConfigID const& getId() const { return m_id; }

    // Main config file
    ClientConfigFile const& cfgFile() const { return m_clientConfigFile; }

    // Path to name.sh file for IPC client initialization
    fs::path const getShellPath() const { return cfgFile().shell(); }

    // Functionality
    // Verify FORK is allowed by Fork + AdditionalForks and throw an error if not
    bool validateForkAllowed(FORK const& _net, bool _bail = true) const;
    bool checkForkAllowed(FORK const& _net) const;

    // Translate smart network names into network names ( `<=Homestead` to `Frontier, Homestead`)
    std::vector<FORK> translateNetworks(set<string> const& _networks) const;
    static std::vector<FORK> translateNetworks(set<string> const& _networks, std::vector<FORK> const& _netOrder);

    // Translate exceptionID from tests into client error string from configs
    // Print suggestions if no match found
    std::string const& translateException(string const& _exceptionName) const;

    // Get Contents of genesis template for specified FORK
    spDataObject const& getGenesisTemplate(FORK const& _fork) const;

    // Get reward information info for each fork
    std::map<FORK, spVALUE> const& getRewardMap() const { return m_correctReward; }

    // Get path to correct mining reward info file
    fs::path const& getRewardMapPath() const { return m_correctMiningRewardPath; }

    // Get path to scripts
    void initializeFirstSetup();
    fs::path const& getSetupScript() const { return m_setupScriptPath; }
    fs::path const& getStartScript() const { return m_starterScriptPath; }
    fs::path const& getStopperScript() const { return m_stopperScriptPath; }

    // Replace notations in requests if needed
    void performFieldReplace(DataObject& _data, FieldReplaceDir const& _dir) const;

private:
    ClientConfigID m_id;                                ///< Internal id
    GCP_SPointer<ClientConfigFile> m_clientConfigFile;  ///< <clientname>/config file
    std::map<FORK, spVALUE> m_correctReward;            ///< Correct mining reward info for StateTests->BlockchainTests
    std::map<FORK, spDataObject> m_genesisTemplate;       ///< Template For test_setChainParams
    fs::path m_correctMiningRewardPath;                 ///< Path to correct mining reward info file

    bool m_initialized = false;    ///< If setup script has run
    fs::path m_setupScriptPath;    ///< Path to setup script (run once before thread exec)
    fs::path m_starterScriptPath;  ///< Path to starter script
    fs::path m_stopperScriptPath;  ///< Path to stopper script
};

}  // namespace test
