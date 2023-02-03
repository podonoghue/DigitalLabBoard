
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY TestBench IS
END TestBench;
 
ARCHITECTURE behavior OF TestBench IS 
 
    --Inputs
   signal clock : std_logic := '0';

 	--Outputs
   signal leds : std_logic_vector(31 downto 0);

   -- Clock period definitions
   constant clock_period : time := 10 ns;
 
   signal complete : boolean := false;
	
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: 
	entity work.cpld_tester(Behavioral)
	port map (
		 clock => clock,
		 leds  => leds
   );

   -- Clock process definitions
   clock_process :process
   begin
	   while not complete loop
         clock <= '0';
         wait for clock_period/2;
         clock <= '1';
         wait for clock_period/2;
		end loop;
		wait;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for 2000 ms;

      -- insert stimulus here 

      complete <= true;
		
      wait;
		
   end process;

END;
