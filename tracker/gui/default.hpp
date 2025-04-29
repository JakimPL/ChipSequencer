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
Pos=806,19
Size=1114,171
Collapsed=0
DockId=0x00000005,0

[Window][General]
Pos=0,19
Size=804,171
Collapsed=0
DockId=0x00000004,0

[Window][Channels]
Pos=1561,192
Size=359,439
Collapsed=0
DockId=0x00000011,0

[Window][DSPs]
Pos=1561,633
Size=359,447
Collapsed=0
DockId=0x00000012,0

[Window][Envelopes]
Pos=0,192
Size=670,315
Collapsed=0
DockId=0x00000006,0

[Window][Orders]
Pos=0,509
Size=234,251
Collapsed=0
DockId=0x0000000C,0

[Window][Oscillators]
Pos=672,192
Size=464,315
Collapsed=0
DockId=0x00000008,0

[Window][Sequences]
Pos=0,762
Size=234,318
Collapsed=0
DockId=0x0000000D,0

[Window][Wavetables]
Pos=1138,192
Size=421,315
Collapsed=0
DockId=0x00000009,0

[Window][Patterns]
Pos=236,509
Size=1323,571
Collapsed=0
DockId=0x0000000F,0

[Window][GeneralDock]
Pos=0,19
Size=1920,1061
Collapsed=0

[Window][Error]
Pos=794,469
Size=247,71
Collapsed=0

[Window][Load error]
Pos=726,471
Size=18,19
Collapsed=0

[Window][Routing Editor]
Pos=236,509
Size=1323,571
Collapsed=0
DockId=0x0000000F,1

[Window][Compilation success]
Pos=537,324
Size=205,71
Collapsed=0

[Window][Render success]
Pos=823,469
Size=205,71
Collapsed=0

[Window][Compilation failure]
Pos=565,324
Size=149,71
Collapsed=0

[Window][Render failure]
Pos=885,504
Size=149,71
Collapsed=0

[Window][##MainMenuBar]
Pos=0,0
Size=1920,19
Collapsed=0

[Table][0x8E77C46A,2]
Column 0  Weight=1.0000
Column 1  Weight=1.0000

[Docking][Data]
DockSpace           ID=0xE241FA1E Window=0xA7EE3C72 Pos=0,19 Size=1920,1061 Split=Y Selected=0xAAA808CD
  DockNode          ID=0x00000001 Parent=0xE241FA1E SizeRef=1848,171 Split=X Selected=0x81353102
    DockNode        ID=0x00000004 Parent=0x00000001 SizeRef=774,170 Selected=0x81353102
    DockNode        ID=0x00000005 Parent=0x00000001 SizeRef=1072,170 Selected=0xDF0EC458
  DockNode          ID=0x00000003 Parent=0xE241FA1E SizeRef=1848,817 Split=X
    DockNode        ID=0x0000000B Parent=0x00000003 SizeRef=1559,818 Split=Y Selected=0xAAA808CD
      DockNode      ID=0x00000002 Parent=0x0000000B SizeRef=913,315 Split=X Selected=0x7D30DF90
        DockNode    ID=0x00000006 Parent=0x00000002 SizeRef=670,295 Selected=0x7D30DF90
        DockNode    ID=0x00000007 Parent=0x00000002 SizeRef=887,295 Split=X Selected=0x6589AB14
          DockNode  ID=0x00000008 Parent=0x00000007 SizeRef=464,315 Selected=0x6589AB14
          DockNode  ID=0x00000009 Parent=0x00000007 SizeRef=421,315 Selected=0xAA9836B6
      DockNode      ID=0x0000000A Parent=0x0000000B SizeRef=913,500 Split=X Selected=0xAAA808CD
        DockNode    ID=0x0000000E Parent=0x0000000A SizeRef=234,520 Split=Y Selected=0x5C5BDF3D
          DockNode  ID=0x0000000C Parent=0x0000000E SizeRef=304,220 Selected=0x5C5BDF3D
          DockNode  ID=0x0000000D Parent=0x0000000E SizeRef=304,278 Selected=0xE250DE7C
        DockNode    ID=0x0000000F Parent=0x0000000A SizeRef=1323,520 CentralNode=1 Selected=0x751C9F38
    DockNode        ID=0x00000010 Parent=0x00000003 SizeRef=359,818 Split=Y Selected=0xCA9451E3
      DockNode      ID=0x00000011 Parent=0x00000010 SizeRef=326,404 Selected=0xF94EBDC0
      DockNode      ID=0x00000012 Parent=0x00000010 SizeRef=326,411 Selected=0xCA9451E3
)INI";

#endif // GUI_DEFAULT_HPP
