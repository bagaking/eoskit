#pragma once

#include "../utils/assert.hpp"

namespace kh {

    template<typename context_t>
    class plugin {

    private:
        plugin<context_t> *_next;
        bool _wait_next;

    public:

        template<typename T>
        plugin *create() {
            kh::assert::equal(next, nullptr, "next plugin is already exist");
            _next = new T();
            kh::assert::not_equal(nullptr, dynamic_cast<plugin<context_t>>(*_next),
                                  "type cast error"); //todo : need protection here
            return next;
        }

        next() {
            _wait_next = false;
            if (next)
        }

        void trigger(const context_t &ctx) {
            _wait_next = true;
            on_trigger()
            kh::assert::ok(!_wait_next, "must call next on trigger method");
        }

        virtual void on_trigger(const context_t &ctx) = 0;

    };

}