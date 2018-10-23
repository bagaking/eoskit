# eoskit

Blockchain development framework (for eos contract development)

## usage

### core

1. Include contract.hpp file and extend the kh::contract  
2. Create an constructor with two parameters, `account_name self` and `account_name code`  
3. Override funtion to implement two transfer event handlers:  
    `on_transcal(const account_name from, const account_name to, eosio::asset token, std::string func, std::vector <std::string> args)`  
    `on_transfer(const account_name from, const account_name to, eosio::asset token, std::string memo)`  
    in a transcal or transfer event handler, _code and token must be verified.  
4. Implement logic functions:  
    1. Functions can be called directly, require authority of then `from`(it means the user who signed the action).  
    2. Functions can be called through transfer receipt, require authority of `_self`(equal to `get_self()`).  
        and then, the function should be called in the handler `on_transcal`  
    3. Export them with macro `EXPORT_ABI`
5. Inline call : In the handler or implemented methods, `_inline_action(const char *act, T &&value)` can be used to execute an inline action.  
    In order to use this method, the correct setting of permissions is necessary.  
    [check the official guide](https://developers.eos.io/eosio-home/docs#section-step-1-adding-eosiocode-to-permissions)  
6. External call : the transfer are supported by defualt in kh::contract.  
    `_transfer_token(const account_name to, const account_name token_code, eosio::asset token, std::string memo)`  