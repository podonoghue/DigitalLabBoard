library IEEE;
use IEEE.std_logic_1164.ALL;

use IEEE.NUMERIC_STD.ALL;

entity cpld_simple is
    Port ( 
       Reset      : in   std_logic;
       debugLed   : out  std_logic
    );
end cpld_simple;

architecture Behavioral of cpld_simple is

begin

   debugLed <= Reset;
   
end Behavioral;
