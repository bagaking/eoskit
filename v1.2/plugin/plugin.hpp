#pragma once

#include "../utils/assert.hpp"
#include "../contract.hpp"

namespace kh {

    template<typename context_t, typename contract_t>
    class plugin {

    public:

    private:
        plugin<context_t, contract_t>() { }

        plugin<context_t, contract_t> *_next = nullptr;
        bool _wait_next = false;

    public:

        template<typename T>
        plugin<context_t, contract_t> *create() {
            kh::assert::equal(_next, nullptr, "next plugin is already exist");
            _next = new T();
            kh::assert::not_equal(nullptr, dynamic_cast<plugin<context_t, contract_t>>(*_next),
                                  "type cast error"); //todo : need protection here
            return next;
        }

        void next(const context_t &ctx, const contract_t &cont) {
            _wait_next = false;
            if (_next) {
                _next->trigger(ctx, cont);
            }
        }

        void trigger(const context_t &ctx, const contract_t &cont) {
            _wait_next = true;
            on_trigger(ctx, cont);
            kh::assert::ok(!_wait_next, "must call next on trigger method");
        }

        virtual void on_trigger(const context_t &ctx, const contract_t &cont) = 0;

    };

}