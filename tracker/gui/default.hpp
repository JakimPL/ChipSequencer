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
Pos=776,19
Size=1072,171
Collapsed=0
DockId=0x00000005,0

[Window][General]
Pos=0,19
Size=774,171
Collapsed=0
DockId=0x00000004,0

[Window][Channels]
Pos=1489,192
Size=359,430
Collapsed=0
DockId=0x00000011,0

[Window][DSPs]
Pos=1489,624
Size=359,385
Collapsed=0
DockId=0x00000012,0

[Window][Envelopes]
Pos=0,192
Size=639,315
Collapsed=0
DockId=0x00000006,0

[Window][Orders]
Pos=0,509
Size=234,220
Collapsed=0
DockId=0x0000000C,0

[Window][Oscillators]
Pos=641,192
Size=443,315
Collapsed=0
DockId=0x00000008,0

[Window][Sequences]
Pos=0,731
Size=234,278
Collapsed=0
DockId=0x0000000D,0

[Window][Wavetables]
Pos=1086,192
Size=401,315
Collapsed=0
DockId=0x00000009,0

[Window][Patterns]
Pos=236,509
Size=1251,500
Collapsed=0
DockId=0x0000000F,0

[Window][GeneralDock]
Pos=0,19
Size=1848,990
Collapsed=0

[Window][Error]
Pos=794,469
Size=247,71
Collapsed=0

[Window][Load error]
Pos=726,471
Size=18,19
Collapsed=0

[Window][Routings]
Pos=236,509
Size=1251,500
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
Size=1920,19
Collapsed=0

[Window][Summary]
Pos=236,509
Size=1251,500
Collapsed=0
DockId=0x0000000F,2

[Window][Commands]
Pos=0,731
Size=234,278
Collapsed=0
DockId=0x0000000D,1

[Window][Commands sequences]
Pos=0,731
Size=234,278
Collapsed=0
DockId=0x0000000D,1

[Window][Commands channels]
Pos=1489,192
Size=359,430
Collapsed=0
DockId=0x00000011,1

[Table][0x8E77C46A,2]
Column 0  Weight=1.0000
Column 1  Weight=1.0000

[Table][0x0957609C,1]
Column 0  Weight=1.0000

[Docking][Data]
DockSpace           ID=0xE241FA1E Window=0xA7EE3C72 Pos=0,19 Size=1848,990 Split=Y Selected=0xAAA808CD
  DockNode          ID=0x00000001 Parent=0xE241FA1E SizeRef=1848,171 Split=X Selected=0x81353102
    DockNode        ID=0x00000004 Parent=0x00000001 SizeRef=774,170 Selected=0x81353102
    DockNode        ID=0x00000005 Parent=0x00000001 SizeRef=1072,170 Selected=0xDF0EC458
  DockNode          ID=0x00000003 Parent=0xE241FA1E SizeRef=1848,817 Split=X
    DockNode        ID=0x0000000B Parent=0x00000003 SizeRef=1559,818 Split=Y Selected=0xAAA808CD
      DockNode      ID=0x00000002 Parent=0x0000000B SizeRef=913,315 Split=X Selected=0x7D30DF90
        DockNode    ID=0x00000006 Parent=0x00000002 SizeRef=670,295 Selected=0xDB196F36
        DockNode    ID=0x00000007 Parent=0x00000002 SizeRef=887,295 Split=X Selected=0x6589AB14
          DockNode  ID=0x00000008 Parent=0x00000007 SizeRef=464,315 Selected=0x866420A7
          DockNode  ID=0x00000009 Parent=0x00000007 SizeRef=421,315 Selected=0x9D983EBA
      DockNode      ID=0x0000000A Parent=0x0000000B SizeRef=913,500 Split=X Selected=0xAAA808CD
        DockNode    ID=0x0000000E Parent=0x0000000A SizeRef=234,520 Split=Y Selected=0x5C5BDF3D
          DockNode  ID=0x0000000C Parent=0x0000000E SizeRef=304,220 Selected=0x575D137D
          DockNode  ID=0x0000000D Parent=0x0000000E SizeRef=304,278 Selected=0xE1E91444
        DockNode    ID=0x0000000F Parent=0x0000000A SizeRef=1323,520 CentralNode=1 Selected=0xD94DCD9B
    DockNode        ID=0x00000010 Parent=0x00000003 SizeRef=359,818 Split=Y Selected=0xCA9451E3
      DockNode      ID=0x00000011 Parent=0x00000010 SizeRef=326,430 Selected=0x2E93F19D
      DockNode      ID=0x00000012 Parent=0x00000010 SizeRef=326,385 Selected=0x1CD7E199
)INI";

#endif // GUI_DEFAULT_HPP
