
library IEEE;
use IEEE.std_logic_1164.ALL;

use IEEE.NUMERIC_STD.ALL;

entity cpld_tester is
    Port ( clock : in   std_logic;
           leds  : out  std_logic_vector (31 downto 0));
end cpld_tester;

architecture Behavioral of cpld_tester is

signal toggle         : std_logic   := '0';
signal counter        : integer range 0 to 100;
 
begin

   process(clock)
   begin
   if rising_edge(clock) then
      if (counter < 99) then
         counter <= counter + 1;
      else
         counter <= 0;
         toggle  <= not toggle ;
      end if;
      
      -- Stagger LED output changes to avoid noise problems
      case counter is
         when 1 =>
            leds(31 downto 24) <= (others=>toggle);
         when 2 =>
            leds(23 downto 16) <= (others=>toggle);
         when 3 =>
            leds(15 downto 8)  <= (others=>toggle);
         when 4 =>
            leds(7 downto 0)   <= (others=>toggle);
         when others =>
            null;
      end case; 
    end if;
   end process;
   
end Behavioral;
