#pragma once
namespace kh {
    namespace utils {
        inline uint64_t bitshift(std::string const &value) {
            uint64_t result = 0;

            char const *p = value.c_str();
            char const *q = p + value.size();
            while (p < q) {
                result = (result << 1) + (result << 3) + *(p++) - '0';
            }
            return result;
        }

        inline std::string symbol_to_string(eosio::asset val) {
            uint64_t v = val.symbol.value;
            v >>= 8;
            std::string result;
            while (v > 0) {
                char c = static_cast<char>(v & 0xFF);
                result += c;
                v >>= 8;
            }
            return result;
        }

        inline std::string asset_to_string(eosio::asset val) {
            std::string sign = val.amount < 0 ? "-" : "";
            uint64_t abs_amount = static_cast<uint64_t>(std::abs(val.amount));
            auto precision = val.symbol.precision();

            std::string result = std::to_string(abs_amount);
            if (precision > 0) {
                auto p = precision;
                uint64_t p10 = 1;
                while (p > 0) {
                    p10 *= 10;
                    p--;
                }

                result = std::to_string(static_cast<uint64_t>(abs_amount / p10));
                auto fract = abs_amount % p10;
                result += "." + std::to_string(p10 + fract).erase(0, 1);
            }
            return sign + result + " " + symbol_to_string(val);
        }


        /**
         * convert uint64 to string
         * @param val
         * @return string
         * @test
         * - std::cout <<"|0:|" << int_to_string(0) << "|12:|" << int_to_string(12) <<"|18446744073709551615:|"<<int_to_string(18446744073709551615)<<"||";
         * - |0:|0|12:|12|18446744073709551615:|18446744073709551615||
         */
        inline std::string uint64_to_string(uint64_t val){
            int ind = 20; // max length of uint64 : 18446744073709551615
            char buf[ind + 1];
            buf[ind] = 0;
            do{
                buf[--ind] = '0' + (val % 10);
                val /= 10;
            } while(val);
            return std::string(&buf[ind]);
        }


    }
}