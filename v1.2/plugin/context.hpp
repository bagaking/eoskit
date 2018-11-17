#pragma once

#include "../utils/assert.hpp"

namespace kh {

    class ctx_transfer {
        account_name &from;
        account_name &to;
        eosio::asset &quantity;
        std::string &memo;
    }

    class ctx_transcal {
        account_name &from;
        account_name &to;
        eosio::asset &quantity;
        std::string &func;
        std::vector<std::string> &args;
    }


}