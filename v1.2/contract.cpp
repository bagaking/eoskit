#pragma once


kh::contract::on_transfer(
        const account_name from,
        const account_name to,
        eosio::asset quantity,
        std::string memo
) {
    if(transfer_plugins != nullptr) {
        return;
    }
    auto ctx = ctx_transfer();
    ctx.from = from;
    ctx.to = to;
    ctx.quantity = quantity;
    ctx.memo = memo;

    transfer_plugins->trigger(ctx, *this);
}

void kh::contract::on_transcal(const account_name from,
                               const account_name to,
                               eosio::asset quantity,
                               std::string func,
                               std::vector<std::string>& args) {
    if(transcal_plugins != nullptr) {
        return;
    }
    auto ctx = ctx_transcal();
    ctx.from = from;
    ctx.to = to;
    ctx.quantity = quantity;
    ctx.func = func;
    ctx.args = args;
    transcal_plugins->trigger(ctx, *this);
}

void kh::contract::setattr(account_name key, const std::string val) {
    _set_my(key, val);
}

template <typename T>
std::string kh::contract::_get_my(const uint64_t key, const T default_val) {
    if (_p_sa == nullptr) {
        _p_sa = new helper::system_attr(_self);
    }
    return _p_sa->ensure(key, default_val);
}

template <typename T>
void kh::contract::_inline_action(const char *act, T &&value) {
    eosio::action(
            eosio::permission_level{get_self(), N(active)},
            get_self(),
            eosio::string_to_name(act),
            value)
            .send();
}

void kh::contract::_transfer_token(const account_name to, const account_name token_code, eosio::asset token, std::string memo) {
    eosio::action(
            eosio::permission_level{get_self(), N(active)},
            token_code,
            N(transfer),
            make_tuple(get_self(), to, token, memo))
            .send();
}