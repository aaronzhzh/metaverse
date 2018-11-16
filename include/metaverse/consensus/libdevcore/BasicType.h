#pragma once

#include <mutex>
#include "FixedHash.h"
#include "SHA3.h"
#include "Guards.h"
#include <boost/throw_exception.hpp>
#include "Exceptions.h"
#include <metaverse/consensus/libethash/internal.h>
#include <metaverse/consensus/libethash/ethash.h>
#include <metaverse/bitcoin/chain/header.hpp>
#include <metaverse/bitcoin/chain/output_point.hpp>


namespace libbitcoin
{

class HeaderAux
{
public:
    static HeaderAux* get();
    static h256 seedHash(const chain::header& bi);
    static h256 hashHead(const chain::header& bi);
    static h256 boundary(const chain::header& bi);

    static uint64_t number(h256& seedHash);
    static uint64_t cacheSize(const chain::header& header);
    static uint64_t dataSize(uint64_t blockNumber);

    static void set_as_testnet(){ is_testnet = true; }

    static u256 calculate_difficulty(const chain::header& current, const chain::header::ptr prev, bool is_staking=false);
    static h256 hash_head_pos(const chain::header& header, const chain::output_info& stateOutput);

private:
    HeaderAux() {}
    Mutex x_epochs;
    h256s m_seedHashes;
    std::unordered_map<h256, unsigned> m_epochs;
    static HeaderAux* s_this;
    static bool is_testnet;
};
struct Solution
{
    Nonce nonce;
    h256 mixHash;
};
struct Result
{
    h256 value;
    h256 mixHash;
};

struct WorkPackage
{
    WorkPackage() = default;
    WorkPackage(chain::header& _bh);
    h256 boundary;
    h256 headerHash;///< When h256() means "pause until notified a new work package is available".
    h256 seedHash;
};

struct LightAllocation
{
    LightAllocation(h256& _seedHash);
    ~LightAllocation();
    Result compute(h256& _headerHash, Nonce& _nonce);
    ethash_light_t light;
    uint64_t size;
};

struct FullAllocation
{
    FullAllocation(ethash_light_t light, ethash_callback_t _cb);
    ~FullAllocation();
    Result compute(h256& _headerHash, Nonce& _nonce);
    uint64_t size() const { return ethash_full_dag_size(full); }
    ethash_full_t full;
};

using LightType = std::shared_ptr<LightAllocation>;
using FullType = std::shared_ptr<FullAllocation>;

}
