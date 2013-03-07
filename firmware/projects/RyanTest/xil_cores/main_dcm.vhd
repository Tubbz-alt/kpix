--------------------------------------------------------------------------------
-- Copyright (c) 1995-2011 Xilinx, Inc.  All rights reserved.
--------------------------------------------------------------------------------
--   ____  ____ 
--  /   /\/   / 
-- /___/  \  /    Vendor: Xilinx 
-- \   \   \/     Version : 13.4
--  \   \         Application : xaw2vhdl
--  /   /         Filename : main_dcm.vhd
-- /___/   /\     Timestamp : 06/22/2012 15:19:32
-- \   \  /  \ 
--  \___\/\___\ 
--
--Command: xaw2vhdl-st /afs/slac.stanford.edu/u/re/bareese/projects/kpix/firmware/projects/KpixSmall/xil_cores/./main_dcm.xaw /afs/slac.stanford.edu/u/re/bareese/projects/kpix/firmware/projects/KpixSmall/xil_cores/./main_dcm
--Design Name: main_dcm
--Device: xc5vlx30t-2ff323
--
-- Module main_dcm
-- Generated by Xilinx Architecture Wizard
-- Written for synthesis tool: XST
-- Period Jitter (unit interval) for block DCM_ADV_INST = 0.038 UI
-- Period Jitter (Peak-to-Peak) for block DCM_ADV_INST = 0.189 ns

library ieee;
use ieee.std_logic_1164.ALL;
use ieee.numeric_std.ALL;
library UNISIM;
use UNISIM.Vcomponents.ALL;

entity main_dcm is
   port ( CLKIN_IN   : in    std_logic; 
          RST_IN     : in    std_logic; 
          CLKFX_OUT  : out   std_logic; 
          CLK0_OUT   : out   std_logic; 
          LOCKED_OUT : out   std_logic);
end main_dcm;

architecture BEHAVIORAL of main_dcm is
   signal CLKFB_IN   : std_logic;
   signal CLKFX_BUF  : std_logic;
   signal CLK0_BUF   : std_logic;
   signal GND_BIT    : std_logic;
   signal GND_BUS_7  : std_logic_vector (6 downto 0);
   signal GND_BUS_16 : std_logic_vector (15 downto 0);
begin
   GND_BIT <= '0';
   GND_BUS_7(6 downto 0) <= "0000000";
   GND_BUS_16(15 downto 0) <= "0000000000000000";
   CLK0_OUT <= CLKFB_IN;
   CLKFX_BUFG_INST : BUFG
      port map (I=>CLKFX_BUF,
                O=>CLKFX_OUT);
   
   CLK0_BUFG_INST : BUFG
      port map (I=>CLK0_BUF,
                O=>CLKFB_IN);
   
   DCM_ADV_INST : DCM_ADV
   generic map( CLK_FEEDBACK => "1X",
            CLKDV_DIVIDE => 2.0,
            CLKFX_DIVIDE => 5,
            CLKFX_MULTIPLY => 8,
            CLKIN_DIVIDE_BY_2 => FALSE,
            CLKIN_PERIOD => 8.000,
            CLKOUT_PHASE_SHIFT => "NONE",
            DCM_AUTOCALIBRATION => TRUE,
            DCM_PERFORMANCE_MODE => "MAX_SPEED",
            DESKEW_ADJUST => "SYSTEM_SYNCHRONOUS",
            DFS_FREQUENCY_MODE => "HIGH",
            DLL_FREQUENCY_MODE => "LOW",
            DUTY_CYCLE_CORRECTION => TRUE,
            FACTORY_JF => x"F0F0",
            PHASE_SHIFT => 0,
            STARTUP_WAIT => FALSE,
            SIM_DEVICE => "VIRTEX5")
      port map (CLKFB=>CLKFB_IN,
                CLKIN=>CLKIN_IN,
                DADDR(6 downto 0)=>GND_BUS_7(6 downto 0),
                DCLK=>GND_BIT,
                DEN=>GND_BIT,
                DI(15 downto 0)=>GND_BUS_16(15 downto 0),
                DWE=>GND_BIT,
                PSCLK=>GND_BIT,
                PSEN=>GND_BIT,
                PSINCDEC=>GND_BIT,
                RST=>RST_IN,
                CLKDV=>open,
                CLKFX=>CLKFX_BUF,
                CLKFX180=>open,
                CLK0=>CLK0_BUF,
                CLK2X=>open,
                CLK2X180=>open,
                CLK90=>open,
                CLK180=>open,
                CLK270=>open,
                DO=>open,
                DRDY=>open,
                LOCKED=>LOCKED_OUT,
                PSDONE=>open);
   
end BEHAVIORAL;


