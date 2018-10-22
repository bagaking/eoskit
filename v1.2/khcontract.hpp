namespace kh {
  class contract : public eosio::contract {
  public:
    contract(
      const account_name self, 
      const account_name code) 
      : eosio::contract(self), 
      _receiptor(code){};

    virtual void ontransfer(
        const account_name from,
        const account_name to,
        eosio::asset quantity,
        std::string memo) = 0;

  protected:
    account_name _receiptor;
  };  
} // namespace kh

#define EOSIO_ABI_EX(TYPE, MEMBERS)                                                                 \
extern "C" {                                                                                        \
  void apply(uint64_t receiver, uint64_t code, uint64_t action) {                                   \
    if (action == N(onerror)) {                                                                     \
      /* onerror is only valid if it is for the "eosio" code account and authorized by "eosio"'s    \
       * "active permission */                                                                      \
      eosio_assert(code == N(eosio),                                                                \
                   "onerror action's are only valid from the \"eosio\" system account");            \
    }                                                                                               \
    auto self = receiver;                                                                           \
                                                                                                    \
    bool valid_internal_actions = code == self && action != N(transfer);                            \
    bool valid_external_transfer = action == N(transfer); /* && code == N(eosio.token) */   \
                                                                                                    \
    /* put all external actions separated by && */                                                  \
    if (valid_internal_actions || valid_external_transfer || action == N(onerror)) {                \
      TYPE thiscontract(self, code);  \
      switch (action) { \
        case N(transfer): \
          execute_action(&thiscontract, &TYPE::ontransfer); \
          break; \
        EOSIO_API(TYPE, MEMBERS) \
      } \
      /* does not allow destructor of thiscontract to run: eosio_exit(0); */                        \
    }                                                                                               \
  }                                                                                                 \
}