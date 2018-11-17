#pragma once

#include "../utils/assert.hpp"

namespace kh {

    template<typename context_t>
    class plugin {

    public:
        static plugin<context_t> *createRoot() {
            return new plugin<context_t>();
        }

    private:
        plugin<context_t>() { }

        plugin<context_t> *_next = nullptr;
        bool _wait_next = false;

    public:

        template<typename T>
        plugin<context_t> *create() {
            kh::assert::equal(_next, nullptr, "next plugin is already exist");
            _next = new T();
            kh::assert::not_equal(nullptr, dynamic_cast<plugin<context_t>>(*_next),
                                  "type cast error"); //todo : need protection here
            return next;
        }

        next(const context_t &ctx, const contract &contract) {
            _wait_next = false;
            if (_next) {
                _next->trigger(ctx, contract);
            }
        }

        void trigger(const context_t &ctx, const contract &contract) {
            _wait_next = true;
            on_trigger(ctx, contract);
            kh::assert::ok(!_wait_next, "must call next on trigger method");
        }

        virtual void on_trigger(const context_t &ctx, const contract &contract) = 0;

    };

}