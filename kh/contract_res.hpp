#pragma once

#include "./utils/assert.hpp"
#include "./utils/util.hpp"

#include "./helper/table_type.hpp"

namespace kh {

    class contract_res {
    public:
        contract_res(const account_name self) : __self(self) {}

        [[eosio::action]] void rescreate(eosio::asset supply);

        [[eosio::action]] void resissue(account_name user,
                                        eosio::asset quantity,
                                        std::string memo);

        [[eosio::action]] void resburn(account_name user,
                                       eosio::asset quantity,
                                       std::string memo);

        [[eosio::action]] void restake(account_name from,
                                       account_name to,
                                       eosio::asset quantity,
                                       std::string memo);

        [[eosio::action]] void reschange(account_name user,
                                         eosio::asset from,
                                         eosio::asset to,
                                         std::string memo);

        [[eosio::action]] void resreceipt(account_name user, eosio::asset from, eosio::asset to, std::string memo) {
            require_auth(__self);
            //require_recipient(__self);
            require_recipient(user);
        }

        inline eosio::asset _get_res_supply(eosio::symbol_name sym) const;
        inline eosio::asset _get_res_balance(account_name user, eosio::symbol_name sym) const;

    public:
        struct transfer_args {
            account_name from;
            account_name to;
            eosio::asset quantity;
            std::string memo;
        };

    private:

        account_name __self;

        inline void _send_receipt(account_name user,
                                  eosio::asset origin_balance,
                                  eosio::asset final_balance,
                                  std::string memo) const;

    private:

        typedef eosio::multi_index<N(res.accounts), helper::res_t> account_table_t;
        typedef eosio::multi_index<N(res.info), helper::res_t> stat_table_t;

        std::tuple <eosio::asset, eosio::asset>
        res_sub_balance(account_name user, eosio::asset value, std::string memo);

        std::tuple <eosio::asset, eosio::asset>
        res_add_balance(account_name user, eosio::asset value, std::string memo);
    }; // namespace kh

    void contract_res::_send_receipt(account_name user,
                                     eosio::asset origin_balance,
                                     eosio::asset final_balance,
                                     std::string memo) const {
        kh::utils::call(__self, __self, N(resreceipt),
                make_tuple(user, origin_balance, final_balance, memo)
        );
    }

    eosio::asset contract_res::_get_res_supply(eosio::symbol_name sym) const {
        stat_table_t stats(__self, __self);
        const auto &st = stats.get(sym);
        return st.balance;
    }

    eosio::asset contract_res::_get_res_balance(account_name user, eosio::symbol_name sym) const {
        account_table_t accounts(__self, user);
        const auto &ac = accounts.get(sym);
        return ac.balance;
    }

    std::tuple <eosio::asset, eosio::asset>
    contract_res::res_sub_balance(account_name user, eosio::asset value, std::string memo) {
        account_table_t accounts(__self, user);

        const auto &from = accounts.get(value.symbol.name(), "no balance object found");
        kh::assert::ok(from.balance.amount >= value.amount, "overdrawn balance");

        eosio::asset origin_balance = from.balance;
        eosio::asset final_balance;
        if (from.balance.amount == value.amount) {
            accounts.erase(from);
            final_balance.symbol = value.symbol;
            final_balance.amount = 0;
        } else {
            accounts.modify(from, __self, [&](auto &a) {
                a.balance -= value;
                final_balance = a.balance;
            });
        }

        _send_receipt(user, origin_balance, final_balance, memo);
        return std::make_tuple(origin_balance, final_balance);
    }

    std::tuple <eosio::asset, eosio::asset>
    contract_res::res_add_balance(account_name user, eosio::asset value, std::string memo) {
        account_table_t accounts(__self, user);

        const auto &key = value.symbol.name();
        auto to = accounts.find(key);

        eosio::asset origin_balance;
        eosio::asset final_balance = value;

        if (to == accounts.end()) {
            accounts.emplace(__self, [&](auto &a) {
                a.balance = value;
                origin_balance.symbol = value.symbol;
                origin_balance.amount = 0;
                final_balance = a.balance;
            });
        } else {
            accounts.modify(to, 0, [&](auto &a) {
                a.balance += value;
                origin_balance = to->balance;
                final_balance = a.balance;
            });
        }

        _send_receipt(user, origin_balance, final_balance, memo);
        return std::make_tuple(origin_balance, final_balance);
    }

    void contract_res::rescreate(eosio::asset supply) {
        require_auth(__self);

        kh::assert::is_valid_token(supply);

        const auto &sym_name = supply.symbol.name();
        stat_table_t stats(__self, __self);
        auto existing = stats.find(sym_name);
        eosio_assert(existing == stats.end(), "token with symbol already exists");

        stats.emplace(__self, [&](auto &s) {
            s.balance = supply;
        });

        res_add_balance(__self, supply, std::string("initial resources"));
    }

    void contract_res::resissue(account_name user, eosio::asset quantity, std::string memo) {
        require_auth(__self);

        auto symbol = quantity.symbol;
        kh::assert::ok(symbol.is_valid(), "invalid symbol name");
        kh::assert::ok(memo.size() <= 256, "memo has more than 256 bytes");

        auto sym_name = symbol.name();
        stat_table_t stats(__self, __self);
        auto existing = stats.find(sym_name);
        eosio_assert(existing != stats.end(), "token with symbol does not exist, create token before issue");

        const auto &st = *existing;

        kh::assert::is_valid_token_of_symbol(quantity, st.balance.symbol);

        stats.modify(st, 0, [&](auto &s) {
            s.balance += quantity;
        });

        res_add_balance(user, quantity, memo);
    }

    void contract_res::resburn(account_name user, eosio::asset quantity, std::string memo) {
        require_auth(__self);

        auto symbol = quantity.symbol;
        kh::assert::ok(symbol.is_valid(), "invalid symbol name");
        kh::assert::ok(memo.size() <= 256, "memo has more than 256 bytes");

        auto sym_name = symbol.name();
        stat_table_t stats(__self, __self);
        auto existing = stats.find(sym_name);
        eosio_assert(existing != stats.end(), "token with symbol does not exist, create token before issue");

        const auto &st = *existing;

        kh::assert::is_valid_token_of_symbol(quantity, st.balance.symbol);

        stats.modify(st, 0, [&](auto &s) {
            s.balance -= quantity;
        });

        res_sub_balance(user, quantity, memo);
    }

    void contract_res::restake(account_name from,
                               account_name to,
                               eosio::asset quantity,
                               std::string memo) {
        require_auth(__self);

        kh::assert::ok(is_account(from), "from account does not exist");
        kh::assert::ok(is_account(to), "to account does not exist");
        kh::assert::not_equal(from, to, "cannot transfer to itself");
        kh::assert::ok(memo.size() <= 256, "memo has more than 256 bytes");

        auto sym = quantity.symbol.name();
        stat_table_t stats(__self, __self);
        const auto &st = stats.get(sym, "symbol not exist");

        kh::assert::is_valid_token_of_symbol(quantity, st.balance.symbol);

        require_recipient(from);
        require_recipient(to);

        res_sub_balance(from, quantity, memo);
        res_add_balance(to, quantity, memo);
    }

    void contract_res::reschange(account_name user,
                                 eosio::asset from,
                                 eosio::asset to,
                                 std::string memo) {
        require_auth(__self);

        kh::assert::ok(is_account(user), "user does not exist");
        kh::assert::not_equal(from, to, "no changes");
        kh::assert::equal(from.symbol, to.symbol, "from and to must have the same symbol");
        kh::assert::ok(memo.size() <= 256, "memo has more than 256 bytes");

        stat_table_t stats(__self, __self);
        auto sym_name = from.symbol.name();
        auto existing = stats.find(sym_name);
        eosio_assert(existing != stats.end(), "token with symbol does not exist, create token before issue");

        const auto &st = *existing;

        kh::assert::is_valid_token_of_symbol(from, st.balance.symbol);
        kh::assert::is_valid_token_of_symbol(to, st.balance.symbol);

        auto balance = _get_res_balance(user, sym_name);
        kh::assert::equal(balance, from, "balance error");

        if (from > to) {
            auto change = from - to;
            res_sub_balance(user, change, memo);
            stats.modify(st, 0, [&](auto &s) {
                s.balance -= (change);
            });
        } else {
            auto change = to - from;
            res_add_balance(user, change, memo);
            stats.modify(st, 0, [&](auto &s) {
                s.balance += change;
            });
        }

    }
}

#define KH_EXPORT_RES (rescreate)(resissue)(resburn)(restake)(reschange)(resreceipt)

/*
 * example
 *
 * cldev get table sco sco res.info
 * cldev get table sco sco res.accounts
 * cldev get table sco kinghandtest res.accounts
 * cldev get table sco eosio res.accounts
 *
 * cldev push action sco rescreate '[ "10.00000000 C" ]' -p sco@active
 * cldev push action sco resissue '[ "kinghandtest", "100.00000000 CT", "test" ]' -p sco@active
 * cldev push action sco restake '[ "kinghandtest", "eosio", "88.00000000 C", "test"]' -p sco@active
 * cldev push action sco resburn '[ "eosio", "88.00000000 C", "test"]' -p sco@active
 * cldev push action sco reschange '[ "kinghandtest", "12.00000000 C", "88.00000000 C", "test"]' -p sco@activec
 *
 */