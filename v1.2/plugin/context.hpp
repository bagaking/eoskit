#pragma once

#include "../utils/assert.hpp"

namespace kh {

    struct ctx_transfer {
        account_name &from;
        account_name &to;
        eosio::asset &quantity;
        std::string &memo;
    };

    struct ctx_transcal {
        account_name &from;
        account_name &to;
        eosio::asset &quantity;
        std::string &func;
        std::vector<std::string> &args;
    };

}