#pragma once

#include <eosio.system/eosio.system.hpp>
#include <eosiolib/privileged.h>

using namespace  eosio;

bool has_ram_managed( const name& account ) {

   eosiosystem::voters_table _voters("eosio"_n, "eosio"_n.value);
   auto vitr = _voters.find( account.value );
   if( vitr == _voters.end() )
      return false;
   
   return eosiosystem::has_field( vitr->flags1, eosiosystem::voter_info::flags1_fields::ram_managed );
}

int64_t ram_balance_to_bytes(const asset& ram_balance) {

   uint128_t new_ram_bytes = uint128_t(ram_balance.amount);
   
   new_ram_bytes *= uint128_t(1024);      //1 RAM token represents 1Kb of RAM
   new_ram_bytes += uint128_t(49999999);  //round up any value > 0.5 bytes to 1 byte
   new_ram_bytes /= uint128_t(100000000); //divide by the precision of RAM token (8)
   
   return static_cast<int64_t>(new_ram_bytes);
}

void update_account_ram_limit(const name& account, const asset& ram_balance) {

   if( has_ram_managed(account) )
      return;

   int64_t ram_bytes, net, cpu;
   get_resource_limits( account.value, &ram_bytes, &net, &cpu );
   int64_t new_ram_bytes = ram_balance_to_bytes(ram_balance);
   set_resource_limits( account.value, new_ram_bytes, net, cpu );      
}
