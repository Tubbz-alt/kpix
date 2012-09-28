
-------------------------------------------------------------------------------
-- Title      : 
-------------------------------------------------------------------------------
-- File       : TriggerPkg.vhd
-- Author     : Benjamin Reese  <bareese@slac.stanford.edu>
-- Company    : SLAC National Accelerator Laboratory
-- Created    : 2012-05-14
-- Last update: 2012-09-26
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2012 SLAC National Accelerator Laboratory
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use work.StdRtlPkg.all;

package TriggerPkg is

  constant TRIGGER_OPCODE_C : slv(7 downto 0) := "00000000";

  constant TRIGGER_ACQUIRE_C   : sl := '0';
  constant TRIGGER_CALIBRATE_C : sl := '1';

  constant TRIGGER_ACQ_SOFTWARE_C : slv(1 downto 0) := "00";
  constant TRIGGER_ACQ_EVR_C      : slv(1 downto 0) := "01";
  constant TRIGGER_ACQ_CMOSA_C    : slv(1 downto 0) := "10";

  type TriggerRegsInType is record
    extTriggerSrc   : slv(2 downto 0);
    extTimestampSrc : slv(2 downto 0);
    acquisitionSrc  : slv(1 downto 0);
    calibrate       : sl;
  end record TriggerRegsInType;

  type TriggerExtInType is record
    nimA  : sl;
    nimB  : sl;
    cmosA : sl;
    cmosB : sl;
  end record TriggerExtInType;

  type TriggerOutType is record
    trigger        : sl;
    startAcquire   : sl;
    startCalibrate : sl;
    startReadout   : sl;
  end record TriggerOutType;

  type TimestampOutType is record
    bunchCount : slv(12 downto 0);
    subCount   : slv(2 downto 0);
    valid      : sl;
  end record TimestampOutType;

  type TimestampInType is record
    rdEn : sl;
  end record TimestampInType;

end package TriggerPkg;
