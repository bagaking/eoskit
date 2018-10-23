#pragma once
namespace kh {
    namespace utils {

        class time {
        public:
        static const uint32_t origin = 1512151512;
        static const uint32_t day = 86400;
        static const uint32_t hour = 3600;
        static const uint32_t min = 60;

        public:
            static uint32_t now()  {
                return publication_time() / 1'000'000 - origin;
            }
        };
    }
}