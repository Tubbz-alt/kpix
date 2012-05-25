//-----------------------------------------------------------------------------
// File          : KpixAsic.cpp
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 11/17/2011
// Project       : Kpix ASIC
//-----------------------------------------------------------------------------
// Description :
// Kpix ASIC container
//-----------------------------------------------------------------------------
// Copyright (c) 2011 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 11/17/2011: created
//-----------------------------------------------------------------------------
#include <KpixAsic.h>
#include <Register.h>
#include <Variable.h>
#include <Command.h>
#include <sstream>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

// Function to convert dac value into a voltage
double KpixAsic::dacToVolt(uint dac) {
   double       volt;

   // Change to voltage
   if ( dac >= 0xf6 ) volt = 2.5 - ((double)(0xff-dac))*50.0*0.0001;
   else volt =(double)dac * 100.0 * 0.0001;

   return(volt);
}

// Function to convert dac value into a voltage
string KpixAsic::dacToVoltString(uint dac) {
   stringstream tmp;
   tmp.str("");
   tmp << dacToVolt(dac) << " V";
   return(tmp.str());
}

// Channel count
uint KpixAsic::channels() {
   if ( dummy_ ) return(0);
   switch(getVariable("Version")->getInt()) {
      case  9: return(512);  break;
      case 10: return(1024); break;
      default: return(0);    break;
   }
}

// Constructor
KpixAsic::KpixAsic ( uint destination, uint baseAddress, uint index, bool dummy, Device *parent ) : 
                     Device(destination,baseAddress,"kpixAsic",index,parent) {
   stringstream tmp;
   uint         x;

   // Description
   desc_    = "Kpix ASIC Object.";
   dummy_   = dummy;

   // Version value
   addVariable(new Variable("Version", Variable::Configuration));
   getVariable("Version")->setDescription("KPIX Version");
   getVariable("Version")->setComp(0,1,0,"");

   // Serial number & variable
   addVariable(new Variable("SerialNumber", Variable::Configuration));
   getVariable("SerialNumber")->setDescription("ASIC serial number");
   getVariable("SerialNumber")->setPerInstance(true);
   getVariable("SerialNumber")->setComp(0,1,0,"");

   // Status register & variables
   addRegister(new Register("Status", baseAddress_ + 0x00000000));

   addVariable(new Variable("StatCmdPerr", Variable::Status));
   getVariable("StatCmdPerr")->setDescription("Command header parity error");
   getVariable("StatCmdPerr")->setComp(0,1,0,"");

   addVariable(new Variable("StatDataPerr", Variable::Status));
   getVariable("StatDataPerr")->setDescription("Command data parity error");
   getVariable("StatDataPerr")->setComp(0,1,0,"");

   addVariable(new Variable("StatTempEn", Variable::Status));
   getVariable("StatTempEn")->setDescription("Temperature read enable");

   addVariable(new Variable("StatTempIdValue", Variable::Status));
   getVariable("StatTempIdValue")->setDescription("Temperature or ID value");

   // Config register & variables
   addRegister(new Register("Config", baseAddress_ + 0x00000001));

   addVariable(new Variable("CfgTestDataEn",Variable::Configuration));
   getVariable("CfgTestDataEn")->setDescription("Enable test data");
   getVariable("CfgTestDataEn")->setTrueFalse();

   addVariable(new Variable("CfgAutoReadDisable",Variable::Configuration));
   getVariable("CfgAutoReadDisable")->setDescription("Disable auto data readout");
   getVariable("CfgAutoReadDisable")->setTrueFalse();

   addVariable(new Variable("CfgForceTemp",Variable::Configuration));
   getVariable("CfgForceTemp")->setDescription("Force temperature power on");
   getVariable("CfgForceTemp")->setTrueFalse();

   addVariable(new Variable("CfgDisableTemp",Variable::Configuration));
   getVariable("CfgDisableTemp")->setDescription("Disable temperature power on");
   getVariable("CfgDisableTemp")->setTrueFalse();

   addVariable(new Variable("CfgAutoStatusReadEn",Variable::Configuration));
   getVariable("CfgAutoStatusReadEn")->setDescription("Enable auto status register read with data");
   getVariable("CfgAutoStatusReadEn")->setTrueFalse();

   // Timing registers & variables
   addRegister(new Register("TimerA", baseAddress_ + 0x00000008));
   addRegister(new Register("TimerB", baseAddress_ + 0x00000009));
   addRegister(new Register("TimerC", baseAddress_ + 0x0000000a));
   addRegister(new Register("TimerD", baseAddress_ + 0x0000000b));
   addRegister(new Register("TimerE", baseAddress_ + 0x0000000c));
   addRegister(new Register("TimerF", baseAddress_ + 0x0000000d));
   //addRegister(new Register("TimerG", baseAddress_ + 0x0000000e));
   //addRegister(new Register("TimerH", baseAddress_ + 0x0000000f));

   addVariable(new Variable("TimeResetOn",Variable::Configuration));
   getVariable("TimeResetOn")->setDescription("Reset assertion delay from run start");
   getVariable("TimeResetOn")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimeResetOn")->setRange(0,65535);

   addVariable(new Variable("TimeResetOff",Variable::Configuration));
   getVariable("TimeResetOff")->setDescription("Reset de-assertion delay from run start");
   getVariable("TimeResetOff")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimeResetOff")->setRange(0,65535);

   addVariable(new Variable("TimeLeakageNullOff",Variable::Configuration));
   getVariable("TimeLeakageNullOff")->setDescription("LeakageNull signal turn off delay from run start");
   getVariable("TimeLeakageNullOff")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimeLeakageNullOff")->setRange(0,65535);

   addVariable(new Variable("TimeOffsetNullOff",Variable::Configuration));
   getVariable("TimeOffsetNullOff")->setDescription("OffsetNull signal turn off delay from run start");
   getVariable("TimeOffsetNullOff")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimeOffsetNullOff")->setRange(0,65535);

   addVariable(new Variable("TimeThreshOff",Variable::Configuration));
   getVariable("TimeThreshOff")->setDescription("Threshold signal turn off delay from run start");
   getVariable("TimeThreshOff")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimeThreshOff")->setRange(0,65535);

   addVariable(new Variable("TrigInhibitOff",Variable::Configuration));
   getVariable("TrigInhibitOff")->setDescription("Trigger inhibit turn off bunch crossing");
   getVariable("TrigInhibitOff")->setComp(0,1,0,"");
   getVariable("TrigInhibitOff")->setRange(0,8191);

   addVariable(new Variable("TimePowerUpOn",Variable::Configuration));
   getVariable("TimePowerUpOn")->setDescription("Power up delay from run start");
   getVariable("TimePowerUpOn")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimePowerUpOn")->setRange(0,65535);

   addVariable(new Variable("TimeDeselDelay",Variable::Configuration));
   getVariable("TimeDeselDelay")->setDescription("Deselect sequence delay from run start");
   getVariable("TimeDeselDelay")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimeDeselDelay")->setRange(0,255);

   addVariable(new Variable("TimeBunchClkDelay",Variable::Configuration));
   getVariable("TimeBunchClkDelay")->setDescription("Bunch clock start delay from from run start");
   getVariable("TimeBunchClkDelay")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimeBunchClkDelay")->setRange(0,65535);

   addVariable(new Variable("TimeDigitizeDelay",Variable::Configuration));
   getVariable("TimeDigitizeDelay")->setDescription("Digitization delay after power down");
   getVariable("TimeDigitizeDelay")->setComp(1,KpixAcqPeriod,0,"nS (@50nS)");
   getVariable("TimeDigitizeDelay")->setRange(0,255);

   addVariable(new Variable("BunchClockCount",Variable::Configuration));
   getVariable("BunchClockCount")->setDescription("Bunch cock count");
   getVariable("BunchClockCount")->setComp(0,1,1,"");
   getVariable("BunchClockCount")->setRange(0,8191);

   // Calibration control registers & variables
   addRegister(new Register("CalDelay0", baseAddress_ + 0x00000010));
   addRegister(new Register("CalDelay1", baseAddress_ + 0x00000011));

   addVariable(new Variable("CalCount",Variable::Configuration));
   getVariable("CalCount")->setDescription("Calibration injection count");
   getVariable("CalCount")->setRange(0,4);

   addVariable(new Variable("Cal0Delay",Variable::Configuration));
   getVariable("Cal0Delay")->setDescription("Calibration injection 0 delay in bunch crossings");
   getVariable("Cal0Delay")->setComp(1,400,0,"nS");
   getVariable("Cal0Delay")->setRange(0,4095);

   addVariable(new Variable("Cal1Delay",Variable::Configuration));
   getVariable("Cal1Delay")->setDescription("Calibration injection 1 delay in bunch crossings");
   getVariable("Cal1Delay")->setComp(1,400,0,"nS");
   getVariable("Cal1Delay")->setRange(0,4095);

   addVariable(new Variable("Cal2Delay",Variable::Configuration));
   getVariable("Cal2Delay")->setDescription("Calibration injection 2 delay in bunch crossings");
   getVariable("Cal2Delay")->setComp(1,400,0,"nS");
   getVariable("Cal2Delay")->setRange(0,4095);

   addVariable(new Variable("Cal3Delay",Variable::Configuration));
   getVariable("Cal3Delay")->setDescription("Calibration injection 3 delay in bunch crossings");
   getVariable("Cal3Delay")->setComp(0,400,0,"nS");
   getVariable("Cal3Delay")->setRange(0,4095);

   // DAC registers and variables
   addRegister(new Register("Dac0", baseAddress_ + 0x00000020));
   addRegister(new Register("Dac1", baseAddress_ + 0x00000021));
   addRegister(new Register("Dac2", baseAddress_ + 0x00000022));
   addRegister(new Register("Dac3", baseAddress_ + 0x00000023));
   addRegister(new Register("Dac4", baseAddress_ + 0x00000024));
   addRegister(new Register("Dac5", baseAddress_ + 0x00000025));
   addRegister(new Register("Dac6", baseAddress_ + 0x00000026));
   addRegister(new Register("Dac7", baseAddress_ + 0x00000027));
   addRegister(new Register("Dac8", baseAddress_ + 0x00000028));
   addRegister(new Register("Dac9", baseAddress_ + 0x00000029));

   addVariable(new Variable("DacThresholdA",Variable::Configuration));
   getVariable("DacThresholdA")->setDescription("Trigger Threshold A dac\nDAC 0/8");
   getVariable("DacThresholdA")->setPerInstance(true);
   getVariable("DacThresholdA")->setRange(0,255);

   addVariable(new Variable("DacThresholdAVolt",Variable::Feedback));
   getVariable("DacThresholdAVolt")->setDescription("Trigger Threshold A dac voltage feedback\nDAC 0/8");
   getVariable("DacThresholdAVolt")->setPerInstance(true);

   addVariable(new Variable("DacThresholdB",Variable::Configuration));
   getVariable("DacThresholdB")->setDescription("Trigger Threshold B dac\nDAC 1/9");
   getVariable("DacThresholdB")->setPerInstance(true);
   getVariable("DacThresholdB")->setRange(0,255);

   addVariable(new Variable("DacThresholdBVolt",Variable::Feedback));
   getVariable("DacThresholdBVolt")->setDescription("Trigger Threshold B dac voltage feedback\nDAC 1/9");
   getVariable("DacThresholdBVolt")->setPerInstance(true);

   addVariable(new Variable("DacRampThresh",Variable::Configuration));
   getVariable("DacRampThresh")->setDescription("Ramp threshold dac\nDAC 2");
   getVariable("DacRampThresh")->setRange(0,255);

   addVariable(new Variable("DacRampThreshVolt",Variable::Feedback));
   getVariable("DacRampThreshVolt")->setDescription("Ramp threshold dac voltage feedback\nDAC 2");

   addVariable(new Variable("DacRangeThreshold",Variable::Configuration));
   getVariable("DacRangeThreshold")->setDescription("Range threshold dac\nDAC 3");
   getVariable("DacRangeThreshold")->setRange(0,255);

   addVariable(new Variable("DacRangeThresholdVolt",Variable::Feedback));
   getVariable("DacRangeThresholdVolt")->setDescription("Range threshold dac voltage feedback\nDAC 3");

   addVariable(new Variable("DacCalibration",Variable::Configuration));
   getVariable("DacCalibration")->setDescription("Calibration dac\nDAC 4");
   getVariable("DacCalibration")->setRange(0,255);

   addVariable(new Variable("DacCalibrationVolt",Variable::Feedback));
   getVariable("DacCalibrationVolt")->setDescription("Calibration dac voltage feedback\nDAC 4");

   addVariable(new Variable("DacCalibrationCharge",Variable::Feedback));
   getVariable("DacCalibrationCharge")->setDescription("Calibration dac charge");

   addVariable(new Variable("DacEventThreshold",Variable::Configuration));
   getVariable("DacEventThreshold")->setDescription("Event threshold dac\nDAC 5");
   getVariable("DacEventThreshold")->setRange(0,255);

   addVariable(new Variable("DacEventThresholdVoltage",Variable::Feedback));
   getVariable("DacEventThresholdVoltage")->setDescription("Event threshold dac voltage feedback\nDAC 5");

   addVariable(new Variable("DacShaperBias",Variable::Configuration));
   getVariable("DacShaperBias")->setDescription("Shaper bias dac\nDAC 6");
   getVariable("DacShaperBias")->setRange(0,255);

   addVariable(new Variable("DacShaperBiasVolt",Variable::Feedback));
   getVariable("DacShaperBiasVolt")->setDescription("Shaper bias dac voltage feedback\nDAC 6");

   addVariable(new Variable("DacDefaultAnalog",Variable::Configuration));
   getVariable("DacDefaultAnalog")->setDescription("Default analog bus dac\nDAC 7");
   getVariable("DacDefaultAnalog")->setRange(0,255);

   addVariable(new Variable("DacDefaultAnalogVolt",Variable::Feedback));
   getVariable("DacDefaultAnalogVolt")->setDescription("Default analog bus dac voltage feedback\nDAC 7");

   // Control register and variables
   addRegister(new Register("Control", baseAddress_ + 0x00000030));

   addVariable(new Variable("CntrlCalibHigh",Variable::Configuration));
   getVariable("CntrlCalibHigh")->setDescription("Force bucket 0 high range calibration");
   getVariable("CntrlCalibHigh")->setTrueFalse();

   addVariable(new Variable("CntrlForceLowGain",Variable::Configuration));
   getVariable("CntrlForceLowGain")->setDescription("Force low gain");
   getVariable("CntrlForceLowGain")->setTrueFalse();

   addVariable(new Variable("CntrlLeakNullDisable",Variable::Configuration));
   getVariable("CntrlLeakNullDisable")->setDescription("Disable leakage null compensation");
   getVariable("CntrlLeakNullDisable")->setTrueFalse();

   addVariable(new Variable("CntrlHighGain",Variable::Configuration));
   getVariable("CntrlHighGain")->setDescription("Enable high gain");
   getVariable("CntrlHighGain")->setTrueFalse();

   addVariable(new Variable("CntrlNearNeighbor",Variable::Configuration));
   getVariable("CntrlNearNeighbor")->setDescription("Enable neareast neighbor trigger logic");
   getVariable("CntrlNearNeighbor")->setTrueFalse();

   addVariable(new Variable("CntrlPolarity",Variable::Configuration));
   getVariable("CntrlPolarity")->setDescription("Set input polarity");
   vector<string> pol;
   pol.resize(2);
   pol[0] = "Negative";
   pol[1] = "Positive";
   getVariable("CntrlPolarity")->setEnums(pol);

   addVariable(new Variable("CntrlDisPerReset",Variable::Configuration));
   getVariable("CntrlDisPerReset")->setDescription("Disable periodic reset circuitry");
   getVariable("CntrlDisPerReset")->setTrueFalse();

   addVariable(new Variable("CntrlEnDcReset",Variable::Configuration));
   getVariable("CntrlEnDcReset")->setDescription("Enable DC reset circuitry");
   getVariable("CntrlEnDcReset")->setTrueFalse();

   addVariable(new Variable("CntrlCalSource",Variable::Configuration));
   getVariable("CntrlCalSource")->setDescription("Set calibration pulse source");
   vector<string> src;
   src.resize(3);
   src[0] = "Disable";
   src[1] = "Internal";
   src[2] = "External";
   getVariable("CntrlCalSource")->setEnums(src);

   addVariable(new Variable("CntrlForceTrigSource",Variable::Configuration));
   getVariable("CntrlForceTrigSource")->setDescription("Set force trigger source");
   getVariable("CntrlForceTrigSource")->setEnums(src);

   addVariable(new Variable("CntrlShortIntEn",Variable::Configuration));
   getVariable("CntrlShortIntEn")->setDescription("Short integration enable");
   getVariable("CntrlShortIntEn")->setTrueFalse();

   addVariable(new Variable("CntrlDisPwrCycle",Variable::Configuration));
   getVariable("CntrlDisPwrCycle")->setDescription("Disable power cycle");
   getVariable("CntrlDisPwrCycle")->setTrueFalse();

   addVariable(new Variable("CntrlFeCurr",Variable::Configuration));
   getVariable("CntrlFeCurr")->setDescription("Set front end current");
   vector<string> curr;
   curr.resize(8);
   curr[0] = "1uA";
   curr[1] = "31uA";
   curr[2] = "61uA";
   curr[3] = "91uA";
   curr[4] = "121uA";
   curr[5] = "151uA";
   curr[6] = "181uA";
   curr[7] = "211uA";
   getVariable("CntrlFeCurr")->setEnums(curr);

   addVariable(new Variable("CntrlHoldTime",Variable::Configuration));
   getVariable("CntrlHoldTime")->setDescription("Set shaper hold time");
   vector<string> holdTime;
   holdTime.resize(8);
   holdTime[0] = "8x";
   holdTime[1] = "16x";
   holdTime[2] = "24x";
   holdTime[3] = "32x";
   holdTime[4] = "40x";
   holdTime[5] = "48x";
   holdTime[6] = "56x";
   holdTime[7] = "64x";
   getVariable("CntrlHoldTime")->setEnums(holdTime);

   addVariable(new Variable("CntrlDiffTime",Variable::Configuration));
   getVariable("CntrlDiffTime")->setDescription("Set shaper differentiation time");
   vector<string> diffTime;
   diffTime.resize(4);
   diffTime[0] = "Normal";
   diffTime[1] = "Half";
   diffTime[2] = "Third";
   diffTime[3] = "Quarter";
   getVariable("CntrlDiffTime")->setEnums(diffTime);

   addVariable(new Variable("CntrlMonSource", Variable::Configuration));
   getVariable("CntrlMonSource")->setDescription("Set monitor port source");
   vector<string> monSource;
   monSource.resize(3);
   monSource[0] = "None";
   monSource[1] = "Amp";
   monSource[2] = "Shaper";
   getVariable("CntrlMonSource")->setEnums(monSource);

   addVariable(new Variable("CntrlTrigDisable", Variable::Configuration));
   getVariable("CntrlTrigDisable")->setDescription("Disable self trigger");
   getVariable("CntrlTrigDisable")->setTrueFalse();

   // Mode registers
   for (x=0; x < 32; x++) {
      tmp.str("");
      tmp << "ColMode_" << setw(2) << setfill('0') << dec << x;
      addVariable(new Variable(tmp.str(),Variable::Configuration));
      getVariable(tmp.str())->setDescription("Channel configuration for column.\n"
                                            "Each charactor represents a row in the column with row 0 being the leftmost value\n"
                                            "The following charactors are allowed:\n"
                                            "D = Channel trigger disabled\n"
                                            "A = Channel trigger threshold A\n"
                                            "B = Channel trigger threshold B\n"
                                            "C = Channel trigger threshold A, with calibration enabled");
      getVariable(tmp.str())->set("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
      getVariable(tmp.str())->setPerInstance(true);

      tmp.str("");
      tmp << "ChanModeA_0x" << setw(2) << setfill('0') << hex << x;
      addRegister(new Register(tmp.str(), baseAddress_ + 0x00000040 + x));
      tmp.str("");
      tmp << "ChanModeB_0x" << setw(2) << setfill('0') << hex << x;
      addRegister(new Register(tmp.str(), baseAddress_ + 0x00000060 + x));
   }

   if ( ! dummy ) getVariable("enabled")->set("False");
}

// Deconstructor
KpixAsic::~KpixAsic ( ) { }

// Method to read status registers and update variables
void KpixAsic::readStatus ( ) {
   REGISTER_LOCK

   // Read status register
   readRegister(getRegister("Status"));

   getVariable("StatCmdPerr")->setInt(getRegister("Status")->get(0,0x1));
   getVariable("StatDataPerr")->setInt(getRegister("Status")->get(1,0x1));
   getVariable("StatTempEn")->setInt(getRegister("Status")->get(2,0x1));
   getVariable("StatTempIdValue")->setInt(getRegister("Status")->get(24,0xFF));
   REGISTER_UNLOCK
}

// Method to read configuration registers and update variables
void KpixAsic::readConfig ( ) {
   stringstream tmp;
   stringstream regA;
   stringstream regB;
   stringstream varName;
   string       varTemp;
   uint val;
   uint col;
   uint row;
   uint calCount;
   uint oldControl;

   REGISTER_LOCK

   // Config register & variables
   readRegister(getRegister("Config"));

   getVariable("CfgTestDataEn")->setInt(getRegister("Config")->get(0,0x1));
   getVariable("CfgAutoReadDisable")->setInt(getRegister("Config")->get(2,0x1));
   getVariable("CfgForceTemp")->setInt(getRegister("Config")->get(3,0x1));
   getVariable("CfgDisableTemp")->setInt(getRegister("Config")->get(4,0x1));
   getVariable("CfgAutoStatusReadEn")->setInt(getRegister("Config")->get(5,0x1));

   // Timing registers
   readRegister(getRegister("TimerA"));
   getVariable("TimeResetOn")->setInt(getRegister("TimerA")->get(0,0xFFFF));
   getVariable("TimeResetOff")->setInt(getRegister("TimerA")->get(16,0xFFFF));

   readRegister(getRegister("TimerB"));
   getVariable("TimeOffsetNullOff")->setInt(getRegister("TimerB")->get(0,0xFFFF));
   getVariable("TimeLeakageNullOff")->setInt(getRegister("TimerB")->get(16,0xFFFF));

   readRegister(getRegister("TimerC"));
   getVariable("TimePowerUpOn")->setInt(getRegister("TimerC")->get(0,0xFFFF));
   getVariable("TimeThreshOff")->setInt(getRegister("TimerC")->get(16,0xFFFF));

   readRegister(getRegister("TimerE"));
   getVariable("BunchClockCount")->setInt(getRegister("TimerE")->get(0,0xFFFF));

   readRegister(getRegister("TimerF"));
   getVariable("TimeDeselDelay")->setInt(getRegister("TimerF")->get(0,0xFF));
   getVariable("TimeBunchClkDelay")->setInt(getRegister("TimerF")->get(8,0xFFFF));
   getVariable("TimeDigitizeDelay")->setInt(getRegister("TimerF")->get(24,0xFF));

   readRegister(getRegister("TimerD"));
   val = getRegister("TimerD")->get();
   val = val - getVariable("TimeBunchClkDelay")->getInt();
   val = val - 1;
   val = val / 8;
   getVariable("TrigInhibitOff")->setInt(val);

   // Calibration control registers & variables
   readRegister(getRegister("CalDelay0"));
   readRegister(getRegister("CalDelay1"));

   getVariable("Cal0Delay")->setInt(getRegister("CalDelay0")->get(0,0x1FFF));
   getVariable("Cal1Delay")->setInt(getRegister("CalDelay0")->get(16,0x1FFF));
   getVariable("Cal2Delay")->setInt(getRegister("CalDelay1")->get(0,0x1FFF));
   getVariable("Cal3Delay")->setInt(getRegister("CalDelay1")->get(16,0x1FFF));

   calCount = 0;
   calCount += getRegister("CalDelay0")->get(15,0x1);
   calCount += getRegister("CalDelay0")->get(31,0x1);
   calCount += getRegister("CalDelay1")->get(15,0x1);
   calCount += getRegister("CalDelay1")->get(31,0x1);
   getVariable("CalCount")->setInt(calCount);

   // Some registers don't exist in dummy
   if ( !dummy_ ) {

      // Turn front end power on in kpix 9 before reading dacs
      if ( getVariable("Version")->getInt() == 9 && getVariable("enabled")->getInt() == 1 ) {
         cout << "KpixAsic::readConfig -> Forcing power on for DAC read!" << endl;
         oldControl = getRegister("Control")->get();
         getRegister("Control")->set(1,24,0x1); // Disable power cycle
         writeRegister(getRegister("Control"),true);
      }

      // DAC registers and variables
      readRegister(getRegister("Dac0"));
      val = getRegister("Dac0")->get(0,0xFF);
      getVariable("DacThresholdA")->setInt(val);
      getVariable("DacThresholdAVolt")->set(dacToVoltString(val));

      readRegister(getRegister("Dac1"));
      val = getRegister("Dac1")->get(0,0xFF);
      getVariable("DacThresholdB")->setInt(val);
      getVariable("DacThresholdBVolt")->set(dacToVoltString(val));

      readRegister(getRegister("Dac2"));
      val = getRegister("Dac2")->get(0,0xFF);
      getVariable("DacRampThresh")->setInt(val);
      getVariable("DacRampThreshVolt")->set(dacToVoltString(val));

      readRegister(getRegister("Dac3"));
      val = getRegister("Dac3")->get(0,0xFF);
      getVariable("DacRangeThreshold")->setInt(val);
      getVariable("DacRangeThresholdVolt")->set(dacToVoltString(val));

      readRegister(getRegister("Dac4"));
      val = getRegister("Dac4")->get(0,0xFF);
      getVariable("DacCalibration")->setInt(val);
      getVariable("DacCalibrationVolt")->set(dacToVoltString(val));

      tmp.str("");
      if ( getVariable("CntrlPolarity")->get() == "Positive" ) {
         tmp << ((2.5 - dacToVolt(val)) * 200e-15);
         tmp << " / ";
         tmp << (((2.5 - dacToVolt(val)) * 200e-15) * 22.0);
      }
      else {
         tmp << (dacToVolt(val) * 200e-15);
         tmp << " / ";
         tmp << ((dacToVolt(val) * 200e-15) * 22.0);
      }
      getVariable("DacCalibrationCharge")->set(tmp.str());
      
      readRegister(getRegister("Dac5"));
      val = getRegister("Dac5")->get(0,0xFF);
      getVariable("DacEventThreshold")->setInt(val);
      getVariable("DacEventThresholdVoltage")->set(dacToVoltString(val));

      readRegister(getRegister("Dac6"));
      val = getRegister("Dac6")->get(0,0xFF);
      getVariable("DacShaperBias")->setInt(val);
      getVariable("DacShaperBiasVolt")->set(dacToVoltString(val));

      readRegister(getRegister("Dac7"));
      val = getRegister("Dac7")->get(0,0xFF);
      getVariable("DacDefaultAnalog")->setInt(val);
      getVariable("DacDefaultAnalogVolt")->set(dacToVoltString(val));

      // Restore control register settings
      if ( getVariable("Version")->getInt() == 9 && getVariable("enabled")->getInt() == 1 ) {
         cout << "KpixAsic::readConfig -> Restoring power setting!" << endl;
         getRegister("Control")->set(oldControl);
         writeRegister(getRegister("Control"),true);
      }

      // Control register and variables
      readRegister(getRegister("Control"));

      getVariable("CntrlDisPerReset")->setInt(getRegister("Control")->get(0,0x1));
      getVariable("CntrlEnDcReset")->setInt(getRegister("Control")->get(1,0x1));
      getVariable("CntrlHighGain")->setInt(getRegister("Control")->get(2,0x1));
      getVariable("CntrlNearNeighbor")->setInt(getRegister("Control")->get(3,0x1));

      val = 0;
      if ( getRegister("Control")->get(6,0x1) == 1 ) val = 1;
      if ( getRegister("Control")->get(4,0x1) == 1 ) val = 2;
      getVariable("CntrlCalSource")->setInt(val);

      val = 0;
      if ( getRegister("Control")->get(7,0x1) == 1 ) val = 1;
      if ( getRegister("Control")->get(5,0x1) == 1 ) val = 2;
      getVariable("CntrlForceTrigSource")->setInt(val);

      getVariable("CntrlHoldTime")->setInt(getRegister("Control")->get(8,0x7));
      getVariable("CntrlCalibHigh")->setInt(getRegister("Control")->get(11,0x1));
      getVariable("CntrlShortIntEn")->setInt(getRegister("Control")->get(12,0x1));
      getVariable("CntrlForceLowGain")->setInt(getRegister("Control")->get(13,0x1));
      getVariable("CntrlLeakNullDisable")->setInt(getRegister("Control")->get(14,0x1));
      getVariable("CntrlPolarity")->setInt(getRegister("Control")->get(15,0x1));
      getVariable("CntrlTrigDisable")->setInt(getRegister("Control")->get(16,0x1));
      getVariable("CntrlDisPwrCycle")->setInt(getRegister("Control")->get(24,0x1));

      // bit order of FeCurr is reversed
      val  = (getRegister("Control")->get(25,0x1) << 2) & 0x4;
      val |= (getRegister("Control")->get(26,0x1) << 1) & 0x2;
      val |= (getRegister("Control")->get(27,0x1)     ) & 0x1;
      getVariable("CntrlFeCurr")->setInt(val);

      getVariable("CntrlDiffTime")->setInt(getRegister("Control")->get(28,0x3));

      val = 0;
      if ( getRegister("Control")->get(30,0x1) == 1 ) val = 2;
      if ( getRegister("Control")->get(31,0x1) == 1 ) val = 1;
      getVariable("CntrlMonSource")->setInt(val);

      // Calibration Mask Registers
      for (col=0; col < (channels()/32); col++) {
         regA.str("");
         regA << "ChanModeA_0x" << setw(2) << setfill('0') << hex << col;
         regB.str("");
         regB << "ChanModeB_0x" << setw(2) << setfill('0') << hex << col;

         varName.str("");
         varName << "ColMode_" << setw(2) << setfill('0') << dec << col;
         varTemp = "";

         readRegister(getRegister(regB.str()));
         readRegister(getRegister(regA.str()));

         for (row=0; row < 32; row++) {
            switch(getRegister(regB.str())->get(row,0x1),getRegister(regA.str())->get(row,0x1)) {
               case  0: varTemp.append("B"); break;
               case  1: varTemp.append("D"); break;
               case  2: varTemp.append("A"); break;
               case  3: varTemp.append("C"); break;
               default: varTemp.append("D"); break;
            }
         }
         getVariable(varName.str())->set(varTemp);
      }
   }

   REGISTER_UNLOCK
}

// Method to write configuration registers
void KpixAsic::writeConfig ( bool force ) {
   stringstream tmp;
   stringstream regA;
   stringstream regB;
   stringstream varName;
   string       varOld;
   string       varNew;
   uint val;
   uint col;
   uint row;
   uint calCount;
   bool dacStale;

   REGISTER_LOCK

   // Config register & variables
   getRegister("Config")->set(getVariable("CfgTestDataEn")->getInt(),0,0x1);
   getRegister("Config")->set(getVariable("CfgAutoReadDisable")->getInt(),2,0x1);
   getRegister("Config")->set(getVariable("CfgForceTemp")->getInt(),3,0x1);
   getRegister("Config")->set(getVariable("CfgDisableTemp")->getInt(),4,0x1);
   getRegister("Config")->set(getVariable("CfgAutoStatusReadEn")->getInt(),5,0x1);
   writeRegister(getRegister("Config"),force);

   // Timing registers
   getRegister("TimerA")->set(getVariable("TimeResetOn")->getInt(),0,0xFFFF);
   getRegister("TimerA")->set(getVariable("TimeResetOff")->getInt(),16,0xFFFF);
   writeRegister(getRegister("TimerA"),force);

   getRegister("TimerB")->set(getVariable("TimeOffsetNullOff")->getInt(),0,0xFFFF);
   getRegister("TimerB")->set(getVariable("TimeLeakageNullOff")->getInt(),16,0xFFFF);
   writeRegister(getRegister("TimerB"),force);

   getRegister("TimerC")->set(getVariable("TimePowerUpOn")->getInt(),0,0xFFFF);
   getRegister("TimerC")->set(getVariable("TimeThreshOff")->getInt(),16,0xFFFF);
   writeRegister(getRegister("TimerC"),force);

   val = (getVariable("TrigInhibitOff")->getInt() * 8) + getVariable("TimeBunchClkDelay")->getInt() + 1;
   getRegister("TimerD")->set(val);
   writeRegister(getRegister("TimerD"),force);

   getRegister("TimerE")->set(getVariable("BunchClockCount")->getInt(),0,0xFFFF);
   getRegister("TimerE")->set(getVariable("TimePowerUpOn")->getInt(),16,0xFFFF);
   writeRegister(getRegister("TimerE"),force);

   getRegister("TimerF")->set(getVariable("TimeDeselDelay")->getInt(),0,0xFF);
   getRegister("TimerF")->set(getVariable("TimeBunchClkDelay")->getInt(),8,0xFFFF);
   getRegister("TimerF")->set(getVariable("TimeDigitizeDelay")->getInt(),24,0xFF);
   writeRegister(getRegister("TimerF"),force);

   // Calibration control registers & variables
   getRegister("CalDelay0")->set(getVariable("Cal0Delay")->getInt(),0,0x1FFF);
   getRegister("CalDelay0")->set(getVariable("Cal1Delay")->getInt(),16,0x1FFF);
   getRegister("CalDelay1")->set(getVariable("Cal2Delay")->getInt(),0,0x1FFF);
   getRegister("CalDelay1")->set(getVariable("Cal3Delay")->getInt(),16,0x1FFF);

   calCount = getVariable("CalCount")->getInt();
   getRegister("CalDelay0")->set((calCount>0)?1:0,15,0x1);
   getRegister("CalDelay0")->set((calCount>1)?1:0,31,0x1);
   getRegister("CalDelay1")->set((calCount>2)?1:0,15,0x1);
   getRegister("CalDelay1")->set((calCount>3)?1:0,31,0x1);
   writeRegister(getRegister("CalDelay0"),force);
   writeRegister(getRegister("CalDelay1"),force);

   // Some registers don't exist in dummy
   if ( !dummy_ ) {

      // DAC registers and variables
      val = getVariable("DacThresholdA")->getInt();
      getVariable("DacThresholdAVolt")->set(dacToVoltString(val));
      getRegister("Dac0")->set(val,0,0xFF);
      getRegister("Dac0")->set(val,8,0xFF);
      getRegister("Dac0")->set(val,16,0xFF);
      getRegister("Dac0")->set(val,24,0xFF);
      getRegister("Dac8")->set(val,0,0xFF);
      getRegister("Dac8")->set(val,8,0xFF);
      getRegister("Dac8")->set(val,16,0xFF);
      getRegister("Dac8")->set(val,24,0xFF);

      val = getVariable("DacThresholdB")->getInt();
      getVariable("DacThresholdBVolt")->set(dacToVoltString(val));
      getRegister("Dac1")->set(val,0,0xFF);
      getRegister("Dac1")->set(val,8,0xFF);
      getRegister("Dac1")->set(val,16,0xFF);
      getRegister("Dac1")->set(val,24,0xFF);
      writeRegister(getRegister("Dac1"),force);
      getRegister("Dac9")->set(val,0,0xFF);
      getRegister("Dac9")->set(val,8,0xFF);
      getRegister("Dac9")->set(val,16,0xFF);
      getRegister("Dac9")->set(val,24,0xFF);

      val = getVariable("DacRampThresh")->getInt();
      getVariable("DacRampThreshVolt")->set(dacToVoltString(val));
      getRegister("Dac2")->set(val,0,0xFF);
      getRegister("Dac2")->set(val,8,0xFF);
      getRegister("Dac2")->set(val,16,0xFF);
      getRegister("Dac2")->set(val,24,0xFF);

      val = getVariable("DacRangeThreshold")->getInt();
      getVariable("DacRangeThresholdVolt")->set(dacToVoltString(val));
      getRegister("Dac3")->set(val,0,0xFF);
      getRegister("Dac3")->set(val,8,0xFF);
      getRegister("Dac3")->set(val,16,0xFF);
      getRegister("Dac3")->set(val,24,0xFF);

      val = getVariable("DacCalibration")->getInt();
      getVariable("DacCalibrationVolt")->set(dacToVoltString(val));
      getRegister("Dac4")->set(val,0,0xFF);
      getRegister("Dac4")->set(val,8,0xFF);
      getRegister("Dac4")->set(val,16,0xFF);
      getRegister("Dac4")->set(val,24,0xFF);

      tmp.str("");
      if ( getVariable("CntrlPolarity")->get() == "Positive" ) {
         tmp << ((2.5 - dacToVolt(val)) * 200e-15);
         tmp << " / ";
         tmp << (((2.5 - dacToVolt(val)) * 200e-15) * 22.0);
      }
      else {
         tmp << (dacToVolt(val) * 200e-15);
         tmp << " / ";
         tmp << ((dacToVolt(val) * 200e-15) * 22.0);
      }
      getVariable("DacCalibrationCharge")->set(tmp.str());
      
      val = getVariable("DacEventThreshold")->getInt();
      getVariable("DacEventThresholdVoltage")->set(dacToVoltString(val));
      getRegister("Dac5")->set(val,0,0xFF);
      getRegister("Dac5")->set(val,8,0xFF);
      getRegister("Dac5")->set(val,16,0xFF);
      getRegister("Dac5")->set(val,24,0xFF);

      val = getVariable("DacShaperBias")->getInt();
      getVariable("DacShaperBiasVolt")->set(dacToVoltString(val));
      getRegister("Dac6")->set(val,0,0xFF);
      getRegister("Dac6")->set(val,8,0xFF);
      getRegister("Dac6")->set(val,16,0xFF);
      getRegister("Dac6")->set(val,24,0xFF);

      val = getVariable("DacDefaultAnalog")->getInt();
      getVariable("DacDefaultAnalogVolt")->set(dacToVoltString(val));
      getRegister("Dac7")->set(val,0,0xFF);
      getRegister("Dac7")->set(val,8,0xFF);
      getRegister("Dac7")->set(val,16,0xFF);
      getRegister("Dac7")->set(val,24,0xFF);

      // Determine if dac registers are stale
      dacStale = force;
      if ( getRegister("Dac0")->stale() ) dacStale = true;
      if ( getRegister("Dac1")->stale() ) dacStale = true;
      if ( getRegister("Dac2")->stale() ) dacStale = true;
      if ( getRegister("Dac3")->stale() ) dacStale = true;
      if ( getRegister("Dac4")->stale() ) dacStale = true;
      if ( getRegister("Dac5")->stale() ) dacStale = true;
      if ( getRegister("Dac6")->stale() ) dacStale = true;
      if ( getRegister("Dac7")->stale() ) dacStale = true;
      if ( getRegister("Dac8")->stale() ) dacStale = true;
      if ( getRegister("Dac9")->stale() ) dacStale = true;

      // Turn front end power on in kpix 9 before writing dacs
      // Real front end power mode will be updated when control
      // register is written later
      if ( getVariable("Version")->getInt() == 9 && dacStale && getVariable("enabled")->getInt() == 1 ) {
         cout << "KpixAsic::writeConfig -> Forcing power on for DAC update!" << endl;
         getRegister("Control")->set(1,24,0x1); // Disable power cycle
         writeRegister(getRegister("Control"),true);
      }
  
      // Now safe to write dac registers
      writeRegister(getRegister("Dac0"),force);
      writeRegister(getRegister("Dac1"),force);
      writeRegister(getRegister("Dac2"),force);
      writeRegister(getRegister("Dac3"),force);
      writeRegister(getRegister("Dac4"),force);
      writeRegister(getRegister("Dac5"),force);
      writeRegister(getRegister("Dac6"),force);
      writeRegister(getRegister("Dac7"),force);
      writeRegister(getRegister("Dac8"),force);
      writeRegister(getRegister("Dac9"),force);

      // Control register and variables
      getRegister("Control")->set(getVariable("CntrlDisPerReset")->getInt(),0,0x1);
      getRegister("Control")->set(getVariable("CntrlEnDcReset")->getInt(),1,0x1);
      getRegister("Control")->set(getVariable("CntrlHighGain")->getInt(),2,0x1);
      getRegister("Control")->set(getVariable("CntrlNearNeighbor")->getInt(),3,0x1);

      val = getVariable("CntrlCalSource")->getInt();
      getRegister("Control")->set((val==1)?1:0,6,0x1);
      getRegister("Control")->set((val==2)?1:0,4,0x1);

      val = getVariable("CntrlForceTrigSource")->getInt();
      getRegister("Control")->set((val==1)?1:0,7,0x1);
      getRegister("Control")->set((val==2)?1:0,5,0x1);

      getRegister("Control")->set(getVariable("CntrlHoldTime")->getInt(),8,0x7);
      getRegister("Control")->set(getVariable("CntrlCalibHigh")->getInt(),11,0x1);
      getRegister("Control")->set(getVariable("CntrlShortIntEn")->getInt(),12,0x1);
      getRegister("Control")->set(getVariable("CntrlForceLowGain")->getInt(),13,0x1);
      getRegister("Control")->set(getVariable("CntrlLeakNullDisable")->getInt(),14,0x1);
      getRegister("Control")->set(getVariable("CntrlPolarity")->getInt(),15,0x1);
      getRegister("Control")->set(getVariable("CntrlTrigDisable")->getInt(),16,0x1);
      getRegister("Control")->set(getVariable("CntrlDisPwrCycle")->getInt(),24,0x1);

      // bit order of FeCurr is reversed
      val = getVariable("CntrlFeCurr")->getInt();
      getRegister("Control")->set(((val   )&0x1),27,0x1);
      getRegister("Control")->set(((val>>1)&0x1),26,0x1);
      getRegister("Control")->set(((val>>2)&0x1),25,0x1);

      getRegister("Control")->set(getVariable("CntrlDiffTime")->getInt(),28,0x3);

      val = getVariable("CntrlMonSource")->getInt();
      getRegister("Control")->set((val==2)?1:0,30,0x1);
      getRegister("Control")->set((val==1)?1:0,31,0x1);

      writeRegister(getRegister("Control"),force);

      // Calibration Mask Registers
      for (col=0; col < (channels()/32); col++) {
         regA.str("");
         regA << "ChanModeA_0x" << setw(2) << setfill('0') << hex << col;
         regB.str("");
         regB << "ChanModeB_0x" << setw(2) << setfill('0') << hex << col;

         varName.str("");
         varName << "ColMode_" << setw(2) << setfill('0') << dec << col;
         varOld = getVariable(varName.str())->get();
         varNew = "";

         for (row=0; row < 32; row++) {
            if ( varOld.length() < (row+1) ) varOld.append("D");
            switch(varOld[row]) {
               case 'B':
                  getRegister(regB.str())->set(0,row,0x1);
                  getRegister(regA.str())->set(0,row,0x1);
                  varNew.append("B");
                  break;
               case 'D':
                  getRegister(regB.str())->set(0,row,0x1);
                  getRegister(regA.str())->set(1,row,0x1);
                  varNew.append("D");
                  break;
               case 'C':
                  getRegister(regB.str())->set(1,row,0x1);
                  getRegister(regA.str())->set(1,row,0x1);
                  varNew.append("C");
                  break;
               case 'A':
                  getRegister(regB.str())->set(1,row,0x1);
                  getRegister(regA.str())->set(0,row,0x1);
                  varNew.append("A");
                  break;
               default : 
                  getRegister(regB.str())->set(0,row,0x1);
                  getRegister(regA.str())->set(1,row,0x1);
                  varNew.append("D");
                  break;
            }
         }
         writeRegister(getRegister(regB.str()),force);
         writeRegister(getRegister(regA.str()),force);
         getVariable(varName.str())->set(varNew);
      }
   }

   REGISTER_UNLOCK
}

// Verify hardware state of configuration
void KpixAsic::verifyConfig ( ) {
   stringstream tmp;
   uint         x;
   uint         oldControl;

   REGISTER_LOCK

   verifyRegister(getRegister("Config"));
   verifyRegister(getRegister("TimerA"));
   verifyRegister(getRegister("TimerB"));
   verifyRegister(getRegister("TimerC"));
   verifyRegister(getRegister("TimerD"));
   verifyRegister(getRegister("TimerE"));
   verifyRegister(getRegister("TimerF"));
   //verifyRegister(getRegister("TimerG"));
   //verifyRegister(getRegister("TimerH"));
   verifyRegister(getRegister("CalDelay0"));
   verifyRegister(getRegister("CalDelay1"));

   if ( !dummy_ ) {

      verifyRegister(getRegister("Control"));

      // Turn front end power on in kpix 9 before reading dacs
      if ( getVariable("Version")->getInt() == 9 && getVariable("enabled")->getInt() == 1 ) {
         cout << "KpixAsic::verifyConfig -> Forcing power on for DAC Verify!" << endl;
         oldControl = getRegister("Control")->get();
         getRegister("Control")->set(1,24,0x1); // Disable power cycle
         writeRegister(getRegister("Control"),true);
      }

      verifyRegister(getRegister("Dac0"));
      verifyRegister(getRegister("Dac1"));
      verifyRegister(getRegister("Dac2"));
      verifyRegister(getRegister("Dac3"));
      verifyRegister(getRegister("Dac4"));
      verifyRegister(getRegister("Dac5"));
      verifyRegister(getRegister("Dac6"));
      verifyRegister(getRegister("Dac7"));
      verifyRegister(getRegister("Dac8"));
      verifyRegister(getRegister("Dac9"));

      // Restore control register settings
      if ( getVariable("Version")->getInt() == 9 && getVariable("enabled")->getInt() == 1 ) {
         cout << "KpixAsic::verifyConfig -> Restoring power state!" << endl;
         getRegister("Control")->set(oldControl);
         writeRegister(getRegister("Control"),true);
      }

      for (x=0; x < (channels()/32); x++) {
         tmp.str("");
         tmp << "ChanModeA_0x" << setw(2) << setfill('0') << hex << x;
         verifyRegister(getRegister(tmp.str()));
         tmp.str("");
         tmp << "ChanModeB_0x" << setw(2) << setfill('0') << hex << x;
         verifyRegister(getRegister(tmp.str()));
      }
   }
   REGISTER_UNLOCK
}

