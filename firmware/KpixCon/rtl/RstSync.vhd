-------------------------------------------------------------------------------
-- Title      : Reset Synchronizer
-- Project    : 
-------------------------------------------------------------------------------
-- File       : RstSync.vhd
-- Standard   : VHDL'93/02, Math Packages
-------------------------------------------------------------------------------
-- Description: Synchronizes the trailing edge of an asynchronous reset to a
--              given clock.
-------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use work.StdRtlPkg.all;

entity RstSync is
  generic (
    DELAY_G    : in time := 1 ns;       -- Simulation FF output delay
    POLARITY_G : in sl   := '1');       -- 0 for active low rst, 1 for high
  port (
    clk      : in  sl;
    asyncRst : in  sl;
    syncRst  : out sl);
end RstSync;

architecture rtl of RstSync is

  signal syncReg : sl;

begin

  process (clk, asyncRst)
  begin
    if (asyncRst = POLARITY_G) then
      syncReg <= POLARITY_G after DELAY_G;
      syncRst <= POLARITY_G after DELAY_G;
    elsif (rising_edge(clk)) then
      syncReg <= asyncRst after DELAY_G;
      syncRst <= syncReg  after DELAY_G;
    end if;
  end process;

end rtl;

