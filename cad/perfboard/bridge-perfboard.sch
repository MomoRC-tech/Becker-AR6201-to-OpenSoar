<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.6.2">
<drawing>
<settings><setting alwaysvectorfont="no"/><setting verticaltext="up"/></settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="yes" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers><layer number="1" name="Top" color="4" fill="1" visible="yes" active="yes"/><layer number="16" name="Bottom" color="1" fill="1" visible="yes" active="yes"/><layer number="17" name="Pads" color="2" fill="1" visible="yes" active="yes"/><layer number="18" name="Vias" color="2" fill="1" visible="yes" active="yes"/><layer number="19" name="Unrouted" color="6" fill="1" visible="yes" active="yes"/><layer number="20" name="Dimension" color="24" fill="1" visible="yes" active="yes"/><layer number="21" name="tPlace" color="7" fill="1" visible="yes" active="yes"/><layer number="22" name="bPlace" color="7" fill="1" visible="yes" active="yes"/><layer number="25" name="tNames" color="7" fill="1" visible="yes" active="yes"/><layer number="27" name="tValues" color="7" fill="1" visible="yes" active="yes"/><layer number="39" name="tKeepout" color="4" fill="1" visible="yes" active="yes"/><layer number="44" name="Drills" color="7" fill="1" visible="yes" active="yes"/><layer number="45" name="Holes" color="7" fill="1" visible="yes" active="yes"/><layer number="51" name="tDocu" color="7" fill="1" visible="yes" active="yes"/><layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/><layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/><layer number="93" name="Pins" color="2" fill="1" visible="yes" active="yes"/><layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/><layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/><layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/><layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/><layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/></layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<description>Bridge perfboard: RX divider, pull-ups, ground point, 3.3 V / 5 V distribution.</description>
<libraries>
<library name="bridge">
<description>Parts for the bridge perfboard (generated).</description>
<packages>
<package name="R0204/7">
<description>Axial resistor, DIN0204 body (3.6 x 1.6 mm), leads bent to 7.62 mm (3 holes).</description>
<wire x1="-1.8" y1="0.8" x2="1.8" y2="0.8" width="0.127" layer="21"/>
<wire x1="1.8" y1="0.8" x2="1.8" y2="-0.8" width="0.127" layer="21"/>
<wire x1="1.8" y1="-0.8" x2="-1.8" y2="-0.8" width="0.127" layer="21"/>
<wire x1="-1.8" y1="-0.8" x2="-1.8" y2="0.8" width="0.127" layer="21"/>
<pad name="1" x="-3.81" y="0" drill="1" diameter="1.9"/>
<pad name="2" x="3.81" y="0" drill="1" diameter="1.9"/>
<text x="-2.54" y="1.27" size="1" layer="25">&gt;NAME</text>
<text x="-2.54" y="-2.27" size="1" layer="27">&gt;VALUE</text>
</package>
<package name="WIREPAD">
<description>One perfboard hole with a wire soldered in.</description>
<pad name="1" x="0" y="0" drill="1" diameter="1.9"/>
<text x="-1.27" y="1.27" size="1" layer="25">&gt;NAME</text>
</package>
</packages>
<symbols>
<symbol name="R">
<wire x1="-2.54" y1="0.889" x2="2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="0.889" x2="2.54" y2="-0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="-0.889" x2="-2.54" y2="-0.889" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<text x="-2.54" y="1.524" size="1.778" layer="95">&gt;NAME</text>
<text x="-2.54" y="-3.302" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
<pin name="2" x="5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
<symbol name="WIRE">
<circle x="0" y="0" radius="0.762" width="0.254" layer="94"/>
<text x="-1.524" y="0" size="1.778" layer="96" align="center-right">&gt;VALUE</text>
<text x="-1.524" y="1.778" size="1.27" layer="95" align="bottom-right">&gt;NAME</text>
<pin name="P" x="2.54" y="0" visible="off" length="short" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="R" prefix="R" uservalue="yes">
<description>Resistor, 0.125 W axial.</description>
<gates><gate name="G$1" symbol="R" x="0" y="0"/></gates>
<devices><device name="" package="R0204/7">
<connects><connect gate="G$1" pin="1" pad="1"/><connect gate="G$1" pin="2" pad="2"/></connects>
<technologies><technology name=""/></technologies>
</device></devices>
</deviceset>
<deviceset name="WIRE" prefix="W" uservalue="yes">
<description>Wire landing on the perfboard; value names the far end.</description>
<gates><gate name="G$1" symbol="WIRE" x="0" y="0"/></gates>
<devices><device name="" package="WIREPAD">
<connects><connect gate="G$1" pin="P" pad="1"/></connects>
<technologies><technology name=""/></technologies>
</device></devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes/>
<variantdefs/>
<classes><class number="0" name="default" width="0" drill="0"/></classes>
<parts>
<part name="W1" library="bridge" deviceset="WIRE" device="" value="UBEC 5 V out (after ferrite)"/>
<part name="W2" library="bridge" deviceset="WIRE" device="" value="ESP32 J2-19 5V"/>
<part name="W3" library="bridge" deviceset="WIRE" device="" value="MAX490 board VCC"/>
<part name="W4" library="bridge" deviceset="WIRE" device="" value="UBEC GND out"/>
<part name="W5" library="bridge" deviceset="WIRE" device="" value="ESP32 J2-14 GND"/>
<part name="W6" library="bridge" deviceset="WIRE" device="" value="RS-232-mod1 R4 GND"/>
<part name="W7" library="bridge" deviceset="WIRE" device="" value="RS-232-mod2 R4 GND"/>
<part name="W8" library="bridge" deviceset="WIRE" device="" value="MAX490 board GND"/>
<part name="W9" library="bridge" deviceset="WIRE" device="" value="ESP32 J2-1 3V3"/>
<part name="W10" library="bridge" deviceset="WIRE" device="" value="RS-232-mod1 R3 VCC"/>
<part name="W11" library="bridge" deviceset="WIRE" device="" value="RS-232-mod2 R3 VCC"/>
<part name="W12" library="bridge" deviceset="WIRE" device="" value="RS-232-mod1 R1 (Zander, unused TTL in)"/>
<part name="W13" library="bridge" deviceset="WIRE" device="" value="RS-232-mod2 R1 (FLARM, unused TTL in)"/>
<part name="W14" library="bridge" deviceset="WIRE" device="" value="MAX490 board RO (5 V level)"/>
<part name="W15" library="bridge" deviceset="WIRE" device="" value="ESP32 J3-12 GPIO16 (UART2 RX)"/>
<part name="W16" library="bridge" deviceset="WIRE" device="" value="ESP32 J3-11 GPIO17 (UART2 TX)"/>
<part name="W17" library="bridge" deviceset="WIRE" device="" value="MAX490 board DI"/>
<part name="R-Z" library="bridge" deviceset="R" device="" value="10k"/>
<part name="R-F" library="bridge" deviceset="R" device="" value="10k"/>
<part name="R-B1" library="bridge" deviceset="R" device="" value="2k2"/>
<part name="R-B2" library="bridge" deviceset="R" device="" value="3k3"/>
<part name="R-D" library="bridge" deviceset="R" device="" value="10k"/>
</parts>
<sheets>
<sheet>
<plain><text x="0" y="121.92" size="2.54" layer="97">Bridge perfboard - minimal board</text><text x="0" y="116.84" size="1.778" layer="97">Generated by tools/perfboard_schematic.py; do not edit by hand.</text><text x="0" y="113.03" size="1.778" layer="97">W = wire soldered into the board; value = far end. RS-232 data lines (R2 to GPIO32/GPIO26) bypass the board.</text><text x="101.6" y="15.24" size="1.778" layer="97">RX divider: GPIO16 = 0.6 x RO (5.0-5.25 V -&gt; 3.0-3.15 V). R-D holds DI high while the ESP32 boots.</text><text x="25.4" y="101" size="1.778" layer="97">W1: UBEC 5 V out (after ferrite)</text><text x="25.4" y="95.92" size="1.778" layer="97">W2: ESP32 J2-19 5V</text><text x="25.4" y="90.84" size="1.778" layer="97">W3: MAX490 board VCC</text><text x="25.4" y="80.68" size="1.778" layer="97">W4: UBEC GND out</text><text x="25.4" y="75.6" size="1.778" layer="97">W5: ESP32 J2-14 GND</text><text x="25.4" y="70.52" size="1.778" layer="97">W6: RS-232-mod1 R4 GND</text><text x="25.4" y="65.44" size="1.778" layer="97">W7: RS-232-mod2 R4 GND</text><text x="25.4" y="60.36" size="1.778" layer="97">W8: MAX490 board GND</text><text x="25.4" y="50.2" size="1.778" layer="97">W9: ESP32 J2-1 3V3</text><text x="25.4" y="45.12" size="1.778" layer="97">W10: RS-232-mod1 R3 VCC</text><text x="25.4" y="40.04" size="1.778" layer="97">W11: RS-232-mod2 R3 VCC</text><text x="127" y="101" size="1.778" layer="97">W12: RS-232-mod1 R1 (Zander, unused TTL in)</text><text x="127" y="85.76" size="1.778" layer="97">W13: RS-232-mod2 R1 (FLARM, unused TTL in)</text><text x="127" y="65.44" size="1.778" layer="97">W14: MAX490 board RO (5 V level)</text><text x="127" y="55.28" size="1.778" layer="97">W15: ESP32 J3-12 GPIO16 (UART2 RX)</text><text x="127" y="34.96" size="1.778" layer="97">W16: ESP32 J3-11 GPIO17 (UART2 TX)</text><text x="127" y="29.88" size="1.778" layer="97">W17: MAX490 board DI</text><text x="248.92" y="103.2" size="1.778" layer="97">R-Z 10k</text><text x="248.92" y="87.96" size="1.778" layer="97">R-F 10k</text><text x="248.92" y="67.64" size="1.778" layer="97">R-B1 2k2</text><text x="248.92" y="52.4" size="1.778" layer="97">R-B2 3k3</text><text x="248.92" y="32.08" size="1.778" layer="97">R-D 10k</text></plain>
<instances>
<instance part="W1" gate="G$1" x="0" y="101.6"/>
<instance part="W2" gate="G$1" x="0" y="96.52"/>
<instance part="W3" gate="G$1" x="0" y="91.44"/>
<instance part="W4" gate="G$1" x="0" y="81.28"/>
<instance part="W5" gate="G$1" x="0" y="76.2"/>
<instance part="W6" gate="G$1" x="0" y="71.12"/>
<instance part="W7" gate="G$1" x="0" y="66.04"/>
<instance part="W8" gate="G$1" x="0" y="60.96"/>
<instance part="W9" gate="G$1" x="0" y="50.8"/>
<instance part="W10" gate="G$1" x="0" y="45.72"/>
<instance part="W11" gate="G$1" x="0" y="40.64"/>
<instance part="W12" gate="G$1" x="101.6" y="101.6"/>
<instance part="W13" gate="G$1" x="101.6" y="86.36"/>
<instance part="W14" gate="G$1" x="101.6" y="66.04"/>
<instance part="W15" gate="G$1" x="101.6" y="55.88"/>
<instance part="W16" gate="G$1" x="101.6" y="35.56"/>
<instance part="W17" gate="G$1" x="101.6" y="30.48"/>
<instance part="R-Z" gate="G$1" x="254" y="101.6"/>
<instance part="R-F" gate="G$1" x="254" y="86.36"/>
<instance part="R-B1" gate="G$1" x="254" y="66.04"/>
<instance part="R-B2" gate="G$1" x="254" y="50.8"/>
<instance part="R-D" gate="G$1" x="254" y="30.48"/>
</instances>
<busses/>
<nets>
<net name="+3V3" class="0"><segment><pinref part="W9" gate="G$1" pin="P"/><wire x1="2.54" y1="50.8" x2="10.16" y2="50.8" width="0.1524" layer="91"/><label x="10.16" y="50.8" size="1.778" layer="95"/></segment><segment><pinref part="W10" gate="G$1" pin="P"/><wire x1="2.54" y1="45.72" x2="10.16" y2="45.72" width="0.1524" layer="91"/><label x="10.16" y="45.72" size="1.778" layer="95"/></segment><segment><pinref part="W11" gate="G$1" pin="P"/><wire x1="2.54" y1="40.64" x2="10.16" y2="40.64" width="0.1524" layer="91"/><label x="10.16" y="40.64" size="1.778" layer="95"/></segment><segment><pinref part="R-Z" gate="G$1" pin="2"/><wire x1="259.08" y1="101.6" x2="266.7" y2="101.6" width="0.1524" layer="91"/><label x="266.7" y="101.6" size="1.778" layer="95"/></segment><segment><pinref part="R-F" gate="G$1" pin="2"/><wire x1="259.08" y1="86.36" x2="266.7" y2="86.36" width="0.1524" layer="91"/><label x="266.7" y="86.36" size="1.778" layer="95"/></segment><segment><pinref part="R-D" gate="G$1" pin="2"/><wire x1="259.08" y1="30.48" x2="266.7" y2="30.48" width="0.1524" layer="91"/><label x="266.7" y="30.48" size="1.778" layer="95"/></segment></net><net name="+5V" class="0"><segment><pinref part="W1" gate="G$1" pin="P"/><wire x1="2.54" y1="101.6" x2="10.16" y2="101.6" width="0.1524" layer="91"/><label x="10.16" y="101.6" size="1.778" layer="95"/></segment><segment><pinref part="W2" gate="G$1" pin="P"/><wire x1="2.54" y1="96.52" x2="10.16" y2="96.52" width="0.1524" layer="91"/><label x="10.16" y="96.52" size="1.778" layer="95"/></segment><segment><pinref part="W3" gate="G$1" pin="P"/><wire x1="2.54" y1="91.44" x2="10.16" y2="91.44" width="0.1524" layer="91"/><label x="10.16" y="91.44" size="1.778" layer="95"/></segment></net><net name="B_DI" class="0"><segment><pinref part="W16" gate="G$1" pin="P"/><wire x1="104.14" y1="35.56" x2="111.76" y2="35.56" width="0.1524" layer="91"/><label x="111.76" y="35.56" size="1.778" layer="95"/></segment><segment><pinref part="W17" gate="G$1" pin="P"/><wire x1="104.14" y1="30.48" x2="111.76" y2="30.48" width="0.1524" layer="91"/><label x="111.76" y="30.48" size="1.778" layer="95"/></segment><segment><pinref part="R-D" gate="G$1" pin="1"/><wire x1="248.92" y1="30.48" x2="241.3" y2="30.48" width="0.1524" layer="91"/><label x="241.3" y="30.48" size="1.778" layer="95" rot="R180"/></segment></net><net name="B_GPIO16" class="0"><segment><pinref part="W15" gate="G$1" pin="P"/><wire x1="104.14" y1="55.88" x2="111.76" y2="55.88" width="0.1524" layer="91"/><label x="111.76" y="55.88" size="1.778" layer="95"/></segment><segment><pinref part="R-B1" gate="G$1" pin="2"/><wire x1="259.08" y1="66.04" x2="266.7" y2="66.04" width="0.1524" layer="91"/><label x="266.7" y="66.04" size="1.778" layer="95"/></segment><segment><pinref part="R-B2" gate="G$1" pin="1"/><wire x1="248.92" y1="50.8" x2="241.3" y2="50.8" width="0.1524" layer="91"/><label x="241.3" y="50.8" size="1.778" layer="95" rot="R180"/></segment></net><net name="B_RO" class="0"><segment><pinref part="W14" gate="G$1" pin="P"/><wire x1="104.14" y1="66.04" x2="111.76" y2="66.04" width="0.1524" layer="91"/><label x="111.76" y="66.04" size="1.778" layer="95"/></segment><segment><pinref part="R-B1" gate="G$1" pin="1"/><wire x1="248.92" y1="66.04" x2="241.3" y2="66.04" width="0.1524" layer="91"/><label x="241.3" y="66.04" size="1.778" layer="95" rot="R180"/></segment></net><net name="F_R1" class="0"><segment><pinref part="W13" gate="G$1" pin="P"/><wire x1="104.14" y1="86.36" x2="111.76" y2="86.36" width="0.1524" layer="91"/><label x="111.76" y="86.36" size="1.778" layer="95"/></segment><segment><pinref part="R-F" gate="G$1" pin="1"/><wire x1="248.92" y1="86.36" x2="241.3" y2="86.36" width="0.1524" layer="91"/><label x="241.3" y="86.36" size="1.778" layer="95" rot="R180"/></segment></net><net name="GND" class="0"><segment><pinref part="W4" gate="G$1" pin="P"/><wire x1="2.54" y1="81.28" x2="10.16" y2="81.28" width="0.1524" layer="91"/><label x="10.16" y="81.28" size="1.778" layer="95"/></segment><segment><pinref part="W5" gate="G$1" pin="P"/><wire x1="2.54" y1="76.2" x2="10.16" y2="76.2" width="0.1524" layer="91"/><label x="10.16" y="76.2" size="1.778" layer="95"/></segment><segment><pinref part="W6" gate="G$1" pin="P"/><wire x1="2.54" y1="71.12" x2="10.16" y2="71.12" width="0.1524" layer="91"/><label x="10.16" y="71.12" size="1.778" layer="95"/></segment><segment><pinref part="W7" gate="G$1" pin="P"/><wire x1="2.54" y1="66.04" x2="10.16" y2="66.04" width="0.1524" layer="91"/><label x="10.16" y="66.04" size="1.778" layer="95"/></segment><segment><pinref part="W8" gate="G$1" pin="P"/><wire x1="2.54" y1="60.96" x2="10.16" y2="60.96" width="0.1524" layer="91"/><label x="10.16" y="60.96" size="1.778" layer="95"/></segment><segment><pinref part="R-B2" gate="G$1" pin="2"/><wire x1="259.08" y1="50.8" x2="266.7" y2="50.8" width="0.1524" layer="91"/><label x="266.7" y="50.8" size="1.778" layer="95"/></segment></net><net name="Z_R1" class="0"><segment><pinref part="W12" gate="G$1" pin="P"/><wire x1="104.14" y1="101.6" x2="111.76" y2="101.6" width="0.1524" layer="91"/><label x="111.76" y="101.6" size="1.778" layer="95"/></segment><segment><pinref part="R-Z" gate="G$1" pin="1"/><wire x1="248.92" y1="101.6" x2="241.3" y2="101.6" width="0.1524" layer="91"/><label x="241.3" y="101.6" size="1.778" layer="95" rot="R180"/></segment></net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
