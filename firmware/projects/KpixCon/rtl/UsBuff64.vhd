-------------------------------------------------------------------------------
-- Title         : Upstream Data Buffer
-- Project       : 
-------------------------------------------------------------------------------
-- File          : UsBuff64.vhd
-- Author        : Ryan Herbst, rherbst@slac.stanford.edu
-- Created       : 01/11/2010
-------------------------------------------------------------------------------
-- Description:
-- VHDL source file for buffer block for upstream data.
-------------------------------------------------------------------------------
-- Copyright (c) 2010 by Ryan Herbst. All rights reserved.
-------------------------------------------------------------------------------
-- Modification history:
-- 01/11/2010: created.
-------------------------------------------------------------------------------
library ieee;
use work.all;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity UsBuff is
  port (

    -- Clock and reset     
    sysClk    : in std_logic;
    sysClkRst : in std_logic;

    -- Local data transfer signals
    frameTxValid : in  std_logic;
    frameTxSOF   : in  std_logic;
    frameTxEOF   : in  std_logic;
    frameTxEOFE  : in  std_logic;
    frameTxData  : in  std_logic_vector(63 downto 0);
    frameTxAFull : out std_logic;

    -- PGP Transmit Signals
    vcFrameTxValid : out std_logic;
    vcFrameTxReady : in  std_logic;
    vcFrameTxSOF   : out std_logic;
    vcFrameTxEOF   : out std_logic;
    vcFrameTxEOFE  : out std_logic;
    vcFrameTxData  : out std_logic_vector(15 downto 0);
    vcRemBuffAFull : in  std_logic;
    vcRemBuffFull  : in  std_logic
    );
end UsBuff;


-- Define architecture
architecture UsBuff of UsBuff is

  -- Local Signals
  signal fifoDin   : std_logic_vector(71 downto 0);
  signal fifoDout  : std_logic_vector(17 downto 0);
  signal fifoRd    : std_logic;
  signal fifoEmpty : std_logic;
  signal fifoFull  : std_logic;
  signal fifoValid : std_logic;


  -- Black Box Attributes
--  attribute syn_black_box                             : boolean;
--  attribute syn_noprune                               : boolean;
--  attribute syn_black_box of fifo_72x512_18x2048_fwft_1 : component is true;
--  attribute syn_noprune of   fifo_72x512_18x2048_fwft_1   : component is true;

begin

  -- Data going into FIFO
  -- Variable width fifo reads out MS Word first
  -- We want LS Word first so swap words on input
  fifoDin(71 downto 54) <= frameTxSOF & "0" & frameTxData(63 downto 48); --(15 downto 0);
  fifoDin(53 downto 36) <= "0" & frameTxEOF & frameTxData(47 downto 32); --(31 downto 16);
  fifoDin(35 downto 18) <= frameTxEOF & frameTxEOF & frameTxData(31 downto 16); --(47 downto 32);
  fifoDin(17 downto 0)  <= frameTxEOF & frameTxEOF & frameTxData(15 downto 0); --(63 downto 48);


  -- Fifo 72 bit write, 18 bit read
  fifo_72x512_18x2048_fwft_1 : entity work.fifo_72x512_18x2048_fwft
    port map (
      rst         => sysClkRst,
      wr_clk      => sysClk,
      rd_clk      => sysClk,
      din         => fifoDin,
      wr_en       => frameTxValid,
      rd_en       => fifoRd,
      dout        => fifoDout,
      full        => fifoFull,
      almost_full => frameTxAFull,
      empty       => fifoEmpty,
      valid       => fifoValid);

  -- Control reads
  fifoRd <= fifoValid and (vcFrameTxReady or (fifoDout(17) and fifoDout(16)));

  -- MUX Data
  vcFrameTxValid <= fifoValid and (fifoDout(17) nand fifoDout(16));
  vcFrameTxSOF   <= fifoDout(17) and not fifoDout(16);
  vcFrameTxEOF   <= fifoDout(16) and not fifoDout(17);
  vcFrameTxEOFE  <= fifoFull;
  vcFrameTxData  <= fifoDout(15 downto 0);

end UsBuff;

