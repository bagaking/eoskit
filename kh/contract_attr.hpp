#pragma once

#include "./helper/system_attr.hpp"
#include "./utils/assert.hpp"

namespace kh {

    class contract_attr {
    public: /** constructor */
        contract_attr(const account_name self) : __self(self) {}

        [[eosio::action]] void setattr(account_name key, const std::string val) {
//            eosio::print("setattr", key, ",", val, " ||| ");
            _set_my(key, val);
        }

    public:
        /** region for utility */
        template<typename T>
        void _set_my(account_name key, const T val) {
            require_auth(__self);
            if (__p_sa == nullptr) {
                __p_sa = new helper::system_attr(__self);
            }
            __p_sa->set(key, val);
        }

        template<typename T>
        std::string _get_my(const uint64_t key, const T default_val) {
            if (__p_sa == nullptr) {
                __p_sa = new helper::system_attr(__self);
            }
            return __p_sa->ensure(key, default_val);
        }

    private:
        account_name __self;
        helper::system_attr *__p_sa;
    }; // namespace kh

}