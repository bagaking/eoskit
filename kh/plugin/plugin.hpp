#pragma once

#include "../utils/assert.hpp"
#include "../contract.hpp"
#include "./context.hpp"

namespace kh {

    template<typename context_t, typename contract_t>
    class plugin {

    public:
        static plugin<context_t, contract_t> *create(plugin<context_t, contract_t> *&pointer) {
            pointer = new plugin<context_t, contract_t>();
            return pointer;
        }

    public:
        template<typename plg_t>
        plugin<context_t, contract_t> *create() {
//          eosio::print("plugin.create");
            kh::assert::ok(nullptr == _next, "next plugin is already exist");
            _next = new plg_t();
            kh::assert::ok(nullptr != dynamic_cast<plugin<context_t, contract_t> *>(_next),
                           "type cast error"); //todo : need protection here
            return _next;
        }

        void next(const context_t &ctx, contract_t &contract_) {
            eosio::print("plugin.next");
            _wait_next = false;
            if (_next) {
                _next->trigger(ctx, contract_);
            }
        }

        void trigger(const context_t &ctx, contract_t &contract_) {
            eosio::print("plugin.trigger");
            _wait_next = true;
            on_trigger(ctx, contract_);
            kh::assert::ok(!_wait_next, "must call next on trigger method");
        }

        virtual void on_trigger(const context_t &ctx, contract_t &contract_) {
            // do nothing
            next(ctx, contract_);
        };

    public:
        plugin<context_t, contract_t>() {}

    private:
        plugin<context_t, contract_t> *_next = nullptr;
        bool _wait_next = false;

    };


}