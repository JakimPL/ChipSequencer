#ifndef GUI_DEFAULT_HPP
#define GUI_DEFAULT_HPP

#include <string>

const std::string default_ini = R"INI(
[Window][DockSpace]
Pos=0,19
Size=1280,701
Collapsed=0

[Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0

[Window][Editor]
Pos=538,19
Size=742,171
Collapsed=0
DockId=0x00000005,0

[Window][General]
Pos=0,19
Size=536,171
Collapsed=0
DockId=0x00000004,0

[Window][Channel Editor]
Pos=954,192
Size=326,261
Collapsed=0
DockId=0x00000011,0

[Window][DSP Editor]
Pos=954,455
Size=326,265
Collapsed=0
DockId=0x00000012,0

[Window][Envelope Editor]
Pos=0,192
Size=399,226
Collapsed=0
DockId=0x00000006,0

[Window][Order Editor]
Pos=0,420
Size=234,157
Collapsed=0
DockId=0x0000000C,0

[Window][Oscillator Editor]
Pos=401,192
Size=275,226
Collapsed=0
DockId=0x00000008,0

[Window][Sequence Editor]
Pos=0,579
Size=234,141
Collapsed=0
DockId=0x0000000D,0

[Window][Wavetable Editor]
Pos=678,192
Size=274,226
Collapsed=0
DockId=0x00000009,0

[Window][Pattern Editor]
Pos=236,420
Size=716,300
Collapsed=0
DockId=0x0000000F,0

[Window][GeneralDock]
Pos=0,19
Size=1280,701
Collapsed=0

[Table][0x8E77C46A,6]
Column 0  Weight=1.0000
Column 1  Weight=1.0000
Column 2  Weight=1.0000
Column 3  Weight=1.0000
Column 4  Weight=1.0000
Column 5  Weight=1.0000

[Docking][Data]
DockSpace           ID=0xE241FA1E Window=0xA7EE3C72 Pos=0,19 Size=1280,701 Split=Y Selected=0xAAA808CD
  DockNode          ID=0x00000001 Parent=0xE241FA1E SizeRef=1848,171 Split=X Selected=0x81353102
    DockNode        ID=0x00000004 Parent=0x00000001 SizeRef=774,170 Selected=0x81353102
    DockNode        ID=0x00000005 Parent=0x00000001 SizeRef=1072,170 Selected=0xDF0EC458
  DockNode          ID=0x00000003 Parent=0xE241FA1E SizeRef=1848,817 Split=X
    DockNode        ID=0x0000000B Parent=0x00000003 SizeRef=913,818 Split=Y Selected=0xAAA808CD
      DockNode      ID=0x00000002 Parent=0x0000000B SizeRef=913,226 Split=X Selected=0x7D30DF90
        DockNode    ID=0x00000006 Parent=0x00000002 SizeRef=638,295 Selected=0x7D30DF90
        DockNode    ID=0x00000007 Parent=0x00000002 SizeRef=880,295 Split=X Selected=0x6589AB14
          DockNode  ID=0x00000008 Parent=0x00000007 SizeRef=440,295 Selected=0x6589AB14
          DockNode  ID=0x00000009 Parent=0x00000007 SizeRef=438,295 Selected=0xAA9836B6
      DockNode      ID=0x0000000A Parent=0x0000000B SizeRef=913,300 Split=X Selected=0xAAA808CD
        DockNode    ID=0x0000000E Parent=0x0000000A SizeRef=234,520 Split=Y Selected=0x5C5BDF3D
          DockNode  ID=0x0000000C Parent=0x0000000E SizeRef=304,157 Selected=0x5C5BDF3D
          DockNode  ID=0x0000000D Parent=0x0000000E SizeRef=304,141 Selected=0xE250DE7C
        DockNode    ID=0x0000000F Parent=0x0000000A SizeRef=1284,520 CentralNode=1 Selected=0xAAA808CD
    DockNode        ID=0x00000010 Parent=0x00000003 SizeRef=326,818 Split=Y Selected=0xCA9451E3
      DockNode      ID=0x00000011 Parent=0x00000010 SizeRef=326,261 Selected=0xF94EBDC0
      DockNode      ID=0x00000012 Parent=0x00000010 SizeRef=326,265 Selected=0xCA9451E3
)INI";

#endif // GUI_DEFAULT_HPP
