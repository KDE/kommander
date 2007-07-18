/**********************************************************************
** Copyright (C) 2000-2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qvariant.h> // HP-UX compiler needs this here
//Added by qt3to4:
#include <QPixmap>

#include "pixmapchooser.h"
#include "formwindow.h"
#if defined(DESIGNER) && !defined(RESOURCE)
#include "pixmapfunction.h"
#endif
#include "metadatabase.h"
#include "mainwindow.h"
#ifndef KOMMANDER
#include "pixmapcollectioneditor.h"
#endif
#ifndef KOMMANDER
#include "pixmapcollection.h"
#endif
#ifndef KOMMANDER
#include "project.h"
#endif

#include <qapplication.h>
#include <qimage.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <q3iconview.h>

#include <klocale.h>

#if defined(DESIGNER)
#include "pics/images.h"

#ifdef Q_WS_MACX
//logo is 40x40 on the mac, if it changes - please notify the Qt/Mac team
static const char * const logo_xpm[] = {
"40 40 543 2",
"  	c None",
". 	c #C9D98D",
"+ 	c #A0BC30",
"@ 	c #A4C132",
"# 	c #ABC92F",
"$ 	c #AAC733",
"% 	c #8FA733",
"& 	c #657821",
"* 	c #3F4E17",
"= 	c #2A360C",
"- 	c #182306",
"; 	c #121B06",
"> 	c #141C06",
", 	c #1D2608",
"' 	c #2A350E",
") 	c #405017",
"! 	c #667C1C",
"~ 	c #8FAA24",
"{ 	c #A8C12D",
"] 	c #728326",
"^ 	c #313A14",
"/ 	c #090B08",
"( 	c #000003",
"_ 	c #000001",
": 	c #000002",
"< 	c #000004",
"[ 	c #000000",
"} 	c #080D01",
"| 	c #333E11",
"1 	c #6E8529",
"2 	c #A2C230",
"3 	c #ACC735",
"4 	c #7D8F2C",
"5 	c #2A300E",
"6 	c #000300",
"7 	c #000007",
"8 	c #010009",
"9 	c #020104",
"0 	c #020301",
"a 	c #020300",
"b 	c #010200",
"c 	c #010101",
"d 	c #020010",
"e 	c #010010",
"f 	c #000005",
"g 	c #000503",
"h 	c #28360C",
"i 	c #789128",
"j 	c #AAC835",
"k 	c #A4BE36",
"l 	c #515F1E",
"m 	c #040800",
"n 	c #00000C",
"o 	c #02000D",
"p 	c #040303",
"q 	c #020200",
"r 	c #020203",
"s 	c #020202",
"t 	c #000008",
"u 	c #000105",
"v 	c #000202",
"w 	c #000201",
"x 	c #020207",
"y 	c #02020C",
"z 	c #00000B",
"A 	c #02010E",
"B 	c #000013",
"C 	c #030900",
"D 	c #4C5E1B",
"E 	c #A1BD33",
"F 	c #9BB52F",
"G 	c #333E10",
"H 	c #010002",
"I 	c #03020B",
"J 	c #020109",
"K 	c #030204",
"L 	c #010104",
"M 	c #020009",
"N 	c #02000A",
"O 	c #000009",
"P 	c #000006",
"Q 	c #04050B",
"R 	c #020106",
"S 	c #020307",
"T 	c #040507",
"U 	c #000104",
"V 	c #34410D",
"W 	c #99B236",
"X 	c #9BB332",
"Y 	c #2B360A",
"Z 	c #020006",
"` 	c #040301",
" .	c #030304",
"..	c #01010A",
"+.	c #020113",
"@.	c #010011",
"#.	c #010500",
"$.	c #0A0F05",
"%.	c #131C07",
"&.	c #0B1205",
"*.	c #020706",
"=.	c #010309",
"-.	c #030503",
";.	c #030402",
">.	c #2C350C",
",.	c #9DB52F",
"'.	c #A3BD2F",
").	c #03000A",
"!.	c #030102",
"~.	c #020101",
"{.	c #030209",
"].	c #02010B",
"^.	c #010301",
"/.	c #242C08",
"(.	c #5A6720",
"_.	c #81922B",
":.	c #93A732",
"<.	c #9AB530",
"[.	c #9CB734",
"}.	c #90AA32",
"|.	c #7E942C",
"1.	c #566820",
"2.	c #273310",
"3.	c #000403",
"4.	c #00000E",
"5.	c #020211",
"6.	c #03030B",
"7.	c #020205",
"8.	c #01000C",
"9.	c #33410B",
"0.	c #A7BF37",
"a.	c #ADC935",
"b.	c #4F5F18",
"c.	c #020103",
"d.	c #080401",
"e.	c #050205",
"f.	c #232C06",
"g.	c #768722",
"h.	c #A4BB2E",
"i.	c #D5E38D",
"j.	c #A2BE3A",
"k.	c #738833",
"l.	c #202C0A",
"m.	c #05030A",
"n.	c #020206",
"o.	c #030303",
"p.	c #505B20",
"q.	c #ACC934",
"r.	c #7C8E2B",
"s.	c #050900",
"t.	c #010201",
"u.	c #030200",
"v.	c #020001",
"w.	c #050207",
"x.	c #030004",
"y.	c #000203",
"z.	c #49531D",
"A.	c #A2B836",
"B.	c #9FBD30",
"C.	c #4E5A1C",
"D.	c #000200",
"E.	c #04040A",
"F.	c #040403",
"G.	c #010005",
"H.	c #040600",
"I.	c #7A9222",
"J.	c #ACCB2D",
"K.	c #A9C22F",
"L.	c #282F0B",
"M.	c #020304",
"N.	c #030109",
"O.	c #55671A",
"P.	c #AEC93D",
"Q.	c #A9C931",
"R.	c #546523",
"S.	c #00000A",
"T.	c #020204",
"U.	c #010300",
"V.	c #272F0B",
"W.	c #A7BE34",
"X.	c #728327",
"Y.	c #020108",
"Z.	c #030305",
"`.	c #030208",
" +	c #46511B",
".+	c #ACC739",
"++	c #93AC39",
"@+	c #263605",
"#+	c #566A1D",
"$+	c #ADC93C",
"%+	c #ACC840",
"&+	c #44541E",
"*+	c #010205",
"=+	c #010206",
"-+	c #020107",
";+	c #010007",
">+	c #030400",
",+	c #728226",
"'+	c #AAC732",
")+	c #323A14",
"!+	c #030401",
"~+	c #01000D",
"{+	c #242D06",
"]+	c #A2B835",
"^+	c #95AD3E",
"/+	c #212C0F",
"(+	c #010604",
"_+	c #5F7327",
":+	c #ABC934",
"<+	c #A2BD3A",
"[+	c #202C09",
"}+	c #01010B",
"|+	c #020105",
"1+	c #333B14",
"2+	c #ABC23C",
"3+	c #8EA631",
"4+	c #0A0C08",
"5+	c #010008",
"6+	c #010102",
"7+	c #020011",
"8+	c #030500",
"9+	c #778823",
"0+	c #96AD3A",
"a+	c #232D0D",
"b+	c #020212",
"c+	c #000109",
"d+	c #030800",
"e+	c #5F7126",
"f+	c #ABCB35",
"g+	c #71862A",
"h+	c #020401",
"i+	c #090C03",
"j+	c #8FA238",
"k+	c #647822",
"l+	c #02000B",
"m+	c #232B08",
"n+	c #A5BB30",
"o+	c #96AC35",
"p+	c #242C0A",
"q+	c #020402",
"r+	c #030403",
"s+	c #000100",
"t+	c #576D1D",
"u+	c #A2BC37",
"v+	c #232D0A",
"w+	c #010004",
"x+	c #677822",
"y+	c #596620",
"z+	c #CBDC71",
"A+	c #96B034",
"B+	c #232B0E",
"C+	c #01020B",
"D+	c #010109",
"E+	c #030502",
"F+	c #040603",
"G+	c #010107",
"H+	c #253704",
"I+	c #B2CE3A",
"J+	c #586721",
"K+	c #425014",
"L+	c #C3D771",
"M+	c #29360C",
"N+	c #020302",
"O+	c #81912F",
"P+	c #94AE38",
"Q+	c #23300B",
"R+	c #040408",
"S+	c #010105",
"T+	c #010207",
"U+	c #242C0C",
"V+	c #91AB33",
"W+	c #D0E18D",
"X+	c #7F922B",
"Y+	c #010400",
"Z+	c #010106",
"`+	c #2A3408",
" @	c #182406",
".@	c #00000D",
"+@	c #0B0F06",
"@@	c #93A736",
"#@	c #96AE38",
"$@	c #000101",
"%@	c #030308",
"&@	c #232A0B",
"*@	c #98AC37",
"=@	c #94AB32",
"-@	c #0C1204",
";@	c #1B2307",
">@	c #9EB62F",
",@	c #010100",
"'@	c #9AB431",
")@	c #97AD37",
"!@	c #242B0E",
"~@	c #01000B",
"{@	c #01010C",
"]@	c #272C13",
"^@	c #9CAF3B",
"/@	c #131B07",
"(@	c #151B07",
"_@	c #9CB230",
":@	c #ADC555",
"<@	c #131C06",
"[@	c #94AB38",
"}@	c #252E0A",
"|@	c #262914",
"1@	c #9DAF46",
"2@	c #9DBA30",
"3@	c #141C07",
"4@	c #010302",
"5@	c #141906",
"6@	c #9CB332",
"7@	c #9EBB31",
"8@	c #1D2609",
"9@	c #010003",
"0@	c #0B1105",
"a@	c #91AA31",
"b@	c #222C0D",
"c@	c #05040A",
"d@	c #232B0A",
"e@	c #98AC3A",
"f@	c #91AB2D",
"g@	c #0B1204",
"h@	c #1D2307",
"i@	c #A0B834",
"j@	c #A5C233",
"k@	c #29340F",
"l@	c #020705",
"m@	c #7E942D",
"n@	c #95AE3B",
"o@	c #232E10",
"p@	c #03040C",
"q@	c #02020B",
"r@	c #020400",
"s@	c #222A0B",
"t@	c #92AE2E",
"u@	c #7D9327",
"v@	c #030704",
"w@	c #2A340D",
"x@	c #A7BF35",
"y@	c #BAD355",
"z@	c #3E4E18",
"A@	c #020110",
"B@	c #566821",
"C@	c #243503",
"D@	c #020213",
"E@	c #030501",
"F@	c #030306",
"G@	c #030207",
"H@	c #010203",
"I@	c #030206",
"J@	c #202E0B",
"K@	c #95AF37",
"L@	c #59691D",
"M@	c #43501A",
"N@	c #ADC732",
"O@	c #657A1D",
"P@	c #000102",
"Q@	c #01000E",
"R@	c #010108",
"S@	c #27330F",
"T@	c #A3BE3D",
"U@	c #586C1E",
"V@	c #000404",
"W@	c #00010A",
"X@	c #050704",
"Y@	c #262B13",
"Z@	c #262B15",
"`@	c #02010C",
" #	c #212E08",
".#	c #97AE3B",
"+#	c #ADC440",
"@#	c #232D06",
"##	c #030101",
"$#	c #6B7D21",
"%#	c #D5E48D",
"&#	c #8DA828",
"*#	c #02020A",
"=#	c #000106",
"-#	c #000401",
";#	c #748736",
">#	c #AECB3D",
",#	c #5E7227",
"'#	c #010204",
")#	c #252B0C",
"!#	c #9BAD3F",
"~#	c #A0B148",
"{#	c #232B0B",
"]#	c #020007",
"^#	c #020209",
"/#	c #2C3418",
"(#	c #646E34",
"_#	c #030600",
":#	c #030202",
"<#	c #050102",
"[#	c #0A1002",
"}#	c #90A727",
"|#	c #CFDE8D",
"1#	c #343D13",
"2#	c #02010D",
"3#	c #040509",
"4#	c #020308",
"5#	c #202B09",
"6#	c #9FBE2E",
"7#	c #ACCA34",
"8#	c #5E7026",
"9#	c #252C0D",
"0#	c #9AAE39",
"a#	c #9AAE3D",
"b#	c #22290D",
"c#	c #010306",
"d#	c #010209",
"e#	c #020004",
"f#	c #030201",
"g#	c #030100",
"h#	c #030009",
"i#	c #333C12",
"j#	c #AAC42F",
"k#	c #738429",
"l#	c #03020F",
"m#	c #4B591A",
"n#	c #A9C831",
"o#	c #A9C932",
"p#	c #576B1F",
"q#	c #263706",
"r#	c #91AB34",
"s#	c #92AC37",
"t#	c #202C0C",
"u#	c #6F8422",
"v#	c #A8BF33",
"w#	c #2B340C",
"x#	c #03010D",
"y#	c #516221",
"z#	c #AFC655",
"A#	c #95AE39",
"B#	c #202E09",
"C#	c #000204",
"D#	c #020102",
"E#	c #28360A",
"F#	c #7D8E2C",
"G#	c #040700",
"H#	c #03020C",
"I#	c #475520",
"J#	c #A3BD3C",
"K#	c #98AF3A",
"L#	c #2D3516",
"M#	c #010103",
"N#	c #0B0D05",
"O#	c #7E9627",
"P#	c #B0C833",
"Q#	c #4E5A17",
"R#	c #222B0B",
"S#	c #72852B",
"T#	c #A4BE35",
"U#	c #AFC544",
"V#	c #666F37",
"W#	c #171A0A",
"X#	c #93AB3A",
"Y#	c #A4BA2F",
"Z#	c #36400C",
"`#	c #040404",
" $	c #28310B",
".$	c #5C6C20",
"+$	c #7F922A",
"@$	c #93A833",
"#$	c #9CB92D",
"$$	c #90AA2C",
"%$	c #59691F",
"&$	c #222A07",
"*$	c #040501",
"=$	c #222D0D",
"-$	c #93AA3F",
";$	c #9DB03A",
">$	c #2C340D",
",$	c #000500",
"'$	c #0D1205",
")$	c #131A07",
"!$	c #0B1203",
"~$	c #010502",
"{$	c #020008",
"]$	c #010202",
"^$	c #212D0D",
"/$	c #94AE34",
"($	c #9FB435",
"_$	c #343F0C",
":$	c #040207",
"<$	c #020502",
"[$	c #2D4102",
"}$	c #A6BD33",
"|$	c #4F5B1D",
"1$	c #030701",
"2$	c #020201",
"3$	c #040502",
"4$	c #030505",
"5$	c #020100",
"6$	c #040102",
"7$	c #030007",
"8$	c #090C04",
"9$	c #16190A",
"0$	c #0A1301",
"a$	c #708727",
"b$	c #ACC836",
"c$	c #7B9223",
"d$	c #29300D",
"e$	c #7F9626",
"f$	c #94AB39",
"g$	c #212E0C",
"h$	c #020603",
"i$	c #101A01",
"j$	c #7A912E",
"k$	c #ADCB2F",
"l$	c #B0CD3A",
"m$	c #A6BD34",
"n$	c #728225",
"o$	c #0A0D03",
"p$	c #0C1101",
"q$	c #343D12",
"r$	c #6F8421",
"s$	c #94AB41",
"t$	c #202C0D",
"u$	c #0A1302",
"v$	c #7A902F",
"w$	c #B0CE37",
"x$	c #ABC33C",
"y$	c #8EA236",
"z$	c #677821",
"A$	c #425115",
"B$	c #2A3409",
"C$	c #151A07",
"D$	c #141A06",
"E$	c #1C2307",
"F$	c #2B330D",
"G$	c #43511B",
"H$	c #6C7E22",
"I$	c #ABC530",
"J$	c #92AC32",
"K$	c #314305",
"L$	c #6F8625",
"M$	c #ACCB30",
"N$	c #9CB330",
"O$	c #9CB432",
"P$	c #A1B935",
"Q$	c #A8BF36",
"R$	c #ADC632",
"                                                                                ",
"                                        . + @ #                                 ",
"                          $ % & * = - ; > , ' ) ! ~                             ",
"                      { ] ^ / ( _ _ : < : [ ( ( : } | 1 2                       ",
"                  3 4 5 6 [ 7 8 9 0 a b b c 8 d e 8 f g h i j                   ",
"                k l m n o p q q r s s t u v w x y z A B t C D E                 ",
"              F G [ H I J K L M N t 7 n O P P Q R S T U t < [ V W               ",
"            X Y _ Z `  ...+.@.7 [ #.$.; %.&.*.: : t =.-.;.S z : >.,.            ",
"          '.G < ).!.~.{.].P ^./.(._.:.<.[.}.|.1.2.3.4.5.6.7...8.[ 9.0.          ",
"        a.b.[ M c.d.e.J [ f.g.h.i.              j.k.l.( m.n.L x o.[ p.q.        ",
"        r.s.t.u.v.w.x.y.z.A.                        B.C.D.f E.F.c G.H.I.J.      ",
"      K.L.O M.b 7.N.y.O.P.                            Q.R.v S.T.U.R f V.W.      ",
"      X.a Y.c Z.`.[  +.+                    ++@+#+$+    %+&+[ *+=+-+;+>+,+      ",
"    '+)+_ !+0 ~+f {+]+                    ^+/+O (+_+:+    <+[+: }+n.|+: 1+2+    ",
"    3+4+5+b 6+7+8+9+                    0+a+: b+c+d+e+f+    g+h+7 n.r f i+j+    ",
"    k+( 8 r l+7 m+n+                  o+p+< 6.q+r+< s+t+    u+v+[ w+6+: [ x+    ",
"    * [ 9 R d _ y+z+                A+B+[ C+D+E+F+G+[ H+    I+J+[ < 6+c : K+    ",
"  L+M+[ q |+e N+O+                P+Q+[ L R+S+7.T+[ U+V+W+    X+Y+< [ Z+z `+    ",
"     @_ a T..@+@@@              #@Q+$@D.T.%@( L : &@*@        =@-@f c 5+n ;@>@  ",
"    ; : ,@t O ; '@            )@!@[ h+G+~@;+~@{@]@^@          <+/@f U.w _ (@_@  ",
"  :@> _ c u P <@[.          [@}@[ L  .;+Z+c |+S.|@1@          2@3@P 4@a [ 5@6@  ",
"  7@8@[ 9@w < 0@a@        ^+b@P y c@`.P 6+Y+b ;+_ d@e@        f@g@t *+T.( h@i@  ",
"  j@k@: 8 w f l@m@      n@o@( p@q@x 9 8.5+b r@T.].[ s@t@      u@v@8.( `.< w@x@  ",
"  y@z@: A@x Q _ B@      C@7 D@E+E@F@S+8 7 G@H@I@l+M [ J@K@    L@< 8.~.G@[ M@N@  ",
"    O@P@Q@y R@: S@T@    U@V@W@-.X@x ( Y@Z@: `@N |+r t._  #.#+#@#f N.##I@[ $#%#  ",
"    &#} 5+n *#=#-#;#    >#,#C '#Y._ )#!#~#{#_ ]#q a s ^#[ /#(#_#:#<#x.f [#}#    ",
"    |#1#: 2#3#4#n 5#6#    7#8#D.( 9#0#    a#b#[ t.|+L c#d#n [ e#f#g#h#< i#j#    ",
"      k#U.@.( r+l#: m#n#    o#p#q#r#        s#t#_ =+4@,@9@G._ r 6+c ( 6 u#      ",
"      v#w#S.z 7.x#l+6 y#%+      z#            A#B#[ C#6+c ,@c c s D#[ E#        ",
"        F#G#O H#Z+8 ..y.I#J#                    K#L#8 G.c c c c o.M#N#O#        ",
"        P#Q#[ Y.n.q  .=#[ R#S#T#                U#V#c : c c s c o.T.W#X#        ",
"          Y#Z#[ J `#>+U.*+f -# $.$+$@$  #$$$u@%$&$*$~@T.,@c c o.s s [ =$-$      ",
"            ;$>$_ G@L U.=+Q@O [ [ ,$'$)$3@!$~$( 7 {$9 6+s o.o.s s s ]$[ ^$/$    ",
"              ($_$[ M M n.T.|+G@R ;+P n z 7 7 R@:$<#g#c c H L 6+s s ]$<$[ [$    ",
"                }$|$1$P P q 2$L R c 3$4$'#H@0 a 5$6$7$( [ 8$9$[ ]$]$t.s+0$a$    ",
"                  b$c$d$D.[ f : H S+;+D.b r %@G@9 ( : w M+e$f$g$[ h$s+i$j$k$    ",
"                    l$m$n$1+o$[ : z n [ [ ( f _ s+p$q$r$      s$t$[ u$v$w$      ",
"                          x$y$z$A$B$;@C$D$E$F$G$H$}#I$          J$K$L$M$        ",
"                                    >@N$O$P$Q$R$                                ",
"                                                                                "};
#else
static const char * const logo_xpm[] = {
/* width height num_colors chars_per_pixel */
"21 16 213 2",
"  	c white",
". 	c #A3C511",
"+ 	c #A2C511",
"@ 	c #A2C611",
"# 	c #A2C510",
"$ 	c #A2C513",
"% 	c #A2C412",
"& 	c #A2C413",
"* 	c #A2C414",
"= 	c #A2C515",
"- 	c #A2C50F",
"; 	c #A3C510",
"> 	c #A2C410",
", 	c #A2C411",
"' 	c #A2C314",
") 	c #A2C316",
"! 	c #A2C416",
"~ 	c #A0C315",
"{ 	c #A1C313",
"] 	c #A1C412",
"^ 	c #A2C40F",
"/ 	c #A1C410",
"( 	c #A0C510",
"_ 	c #A0C511",
": 	c #A1C414",
"< 	c #9FC30E",
"[ 	c #98B51B",
"} 	c #5F7609",
"| 	c #5C6E0E",
"1 	c #5B6E10",
"2 	c #5C6C14",
"3 	c #5A6E0A",
"4 	c #839E16",
"5 	c #A0C515",
"6 	c #A0C513",
"7 	c #A2C512",
"8 	c #A1C512",
"9 	c #A1C511",
"0 	c #A1C50F",
"a 	c #91AE12",
"b 	c #505E11",
"c 	c #1F2213",
"d 	c #070606",
"e 	c #040204",
"f 	c #040306",
"g 	c #15160F",
"h 	c #2F3A0D",
"i 	c #859F1B",
"j 	c #A1C215",
"k 	c #A0C50F",
"l 	c #A1C510",
"m 	c #A0C110",
"n 	c #839C1B",
"o 	c #1E240A",
"p 	c #050205",
"q 	c #030304",
"r 	c #323917",
"s 	c #556313",
"t 	c #56680B",
"u 	c #536609",
"v 	c #4A561B",
"w 	c #0B0D04",
"x 	c #030208",
"y 	c #090A05",
"z 	c #5F6F18",
"A 	c #A0C117",
"B 	c #91AF10",
"C 	c #1E2209",
"D 	c #030205",
"E 	c #17190D",
"F 	c #7D981C",
"G 	c #9ABA12",
"H 	c #A3C411",
"I 	c #A3C713",
"J 	c #95B717",
"K 	c #7F9A18",
"L 	c #8FAE1B",
"M 	c #394413",
"N 	c #040305",
"O 	c #090807",
"P 	c #6C7E19",
"Q 	c #A6C614",
"R 	c #A1C411",
"S 	c #64761F",
"T 	c #030105",
"U 	c #070707",
"V 	c #728513",
"W 	c #A2C40C",
"X 	c #A2C70B",
"Y 	c #89A519",
"Z 	c #313B11",
"` 	c #101409",
" .	c #586A19",
"..	c #97B620",
"+.	c #1B2207",
"@.	c #282D11",
"#.	c #A6C41B",
"$.	c #A1C413",
"%.	c #A3C512",
"&.	c #2E370B",
"*.	c #030108",
"=.	c #21260F",
"-.	c #A5C21A",
";.	c #A0C60D",
">.	c #6D841A",
",.	c #0F1007",
"'.	c #040207",
").	c #0E1009",
"!.	c #515F14",
"~.	c #A2C41B",
"{.	c #5E701B",
"].	c #030203",
"^.	c #0B0B04",
"/.	c #87A111",
"(.	c #A0C411",
"_.	c #A0C316",
":.	c #212907",
"<.	c #222C0B",
"[.	c #A3C516",
"}.	c #9CBE1A",
"|.	c #5E6F1B",
"1.	c #0E0F0B",
"2.	c #040205",
"3.	c #181B0D",
"4.	c #93AE25",
"5.	c #A0C610",
"6.	c #617715",
"7.	c #030306",
"8.	c #070704",
"9.	c #809818",
"0.	c #A1C415",
"a.	c #475416",
"b.	c #030309",
"c.	c #12170B",
"d.	c #91B01E",
"e.	c #5C721F",
"f.	c #05050B",
"g.	c #33371D",
"h.	c #0E0F08",
"i.	c #040405",
"j.	c #758921",
"k.	c #46511B",
"l.	c #030207",
"m.	c #131409",
"n.	c #9FB921",
"o.	c #859D21",
"p.	c #080809",
"q.	c #030305",
"r.	c #46521C",
"s.	c #8EB017",
"t.	c #627713",
"u.	c #4D5F17",
"v.	c #97B71D",
"w.	c #77901D",
"x.	c #151708",
"y.	c #0D0D0B",
"z.	c #0C0B08",
"A.	c #455216",
"B.	c #A5C616",
"C.	c #A0C114",
"D.	c #556118",
"E.	c #050307",
"F.	c #050407",
"G.	c #363E17",
"H.	c #5D7309",
"I.	c #A2BF28",
"J.	c #A2C417",
"K.	c #A4C620",
"L.	c #60701D",
"M.	c #030103",
"N.	c #030303",
"O.	c #809A1B",
"P.	c #A0C310",
"Q.	c #A0C410",
"R.	c #A3C415",
"S.	c #9CB913",
"T.	c #6F801F",
"U.	c #1A210A",
"V.	c #1D1E0D",
"W.	c #1D220F",
"X.	c #1E210F",
"Y.	c #0F0F07",
"Z.	c #0E1007",
"`.	c #090906",
" +	c #2B360E",
".+	c #97B813",
"++	c #A2C50E",
"@+	c #A5C517",
"#+	c #90AD20",
"$+	c #5D6C1A",
"%+	c #394115",
"&+	c #050704",
"*+	c #040304",
"=+	c #202807",
"-+	c #5E6B21",
";+	c #728D0C",
">+	c #65791D",
",+	c #29330F",
"'+	c #7A911D",
")+	c #A2C614",
"!+	c #A1C513",
"~+	c #A3C50E",
"{+	c #A3C414",
"]+	c #9CBD11",
"^+	c #95B40C",
"/+	c #94B50F",
"(+	c #95B510",
"_+	c #99B913",
":+	c #A0C414",
"<+	c #9ABC11",
"[+	c #A0C314",
"}+	c #A1C40F",
"|+	c #A3C513",
". + + @ + # # $ % & * = & - + + + + + # # ",
"; > , > # > > $ ' ) ! ~ { ] ^ , - > , > # ",
"+ + / ( _ : < [ } | 1 2 3 4 5 6 : 7 8 # # ",
"+ 9 # ( 0 a b c d e e e f g h i j 9 k l + ",
"+ + > m n o p q r s t u v w x y z A & # # ",
"# % k B C D E F G H I J K L M N O P Q ] , ",
"$ R > S T U V W , X Y Z `  ...+.T @.#.$.] ",
"% %.* &.*.=.-.;.> >.,.'.).!.~.{.].^./.R 7 ",
"7 (._.:.D <.[.}.|.1.2.2.3.4.5.6.7.8.9._ 8 ",
". % 0.a.b.c.d.e.f.N g.h.2.i.j.k.l.m.n.$ # ",
"; + ; o.p.q.r.s.t.u.v.w.x.2.y.z.].A.B.l : ",
"# # R C.D.E.F.G.H.I.J.K.L.2.M.M.N.O.P.; l ",
"# / Q.R.S.T.U.].8.V.W.X.Y.e Z.`.]. +.+++7 ",
"+ + 9 / ; @+#+$+%+&+e *+=+-+;+>+,+'+)+, # ",
"# + > % & !+~+{+]+^+/+(+_+) Q.:+<+[+$ R # ",
"7 + > }+# % k |+8 + > + * $ _ / , 7 8 ] - "};
#endif

static ImageIconProvider *imageIconProvider = 0;
static PixmapChooser *pixmapChooser = 0;
#endif

PixmapView::PixmapView( QWidget *parent )
    : Q3ScrollView( parent )
{
    viewport()->setBackgroundMode( PaletteBase );
}

void PixmapView::setPixmap( const QPixmap &pix )
{
    pixmap = pix;
    resizeContents( pixmap.size().width(), pixmap.size().height() );
    viewport()->repaint( false );
}

void PixmapView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    p->fillRect( cx, cy, cw, ch, colorGroup().brush( QPalette::Base ) );
    p->drawPixmap( 0, 0, pixmap );
}

void PixmapView::previewUrl( const Q3Url &u )
{
    if ( u.isLocalFile() ) {
	QString path = u.path();
	QPixmap pix( path );
	if ( !pix.isNull() )
	    setPixmap( pix );
    } else {
	qWarning( "Previewing remot files not supported" );
    }
}

QStringList qChoosePixmaps( QWidget *parent )
{
    if ( !imageIconProvider && !Q3FileDialog::iconProvider() )
	Q3FileDialog::setIconProvider( ( imageIconProvider = new ImageIconProvider ) );

    QString filter;
    QString all = qApp->translate( "qChoosePixmap", "All Pixmaps (" );
    for ( uint i = 0; i < QImageIO::outputFormats().count(); i++ ) {
	filter += qApp->translate( "qChoosePixmap", "%1-Pixmaps (%2)\n" ).
		  arg( QImageIO::outputFormats().at( i ) ).
		  arg( "*." + QString( QImageIO::outputFormats().at( i ) ).toLower() );
	all += "*." + QString( QImageIO::outputFormats().at( i ) ).toLower() + ";";
    }
    filter.prepend( all + qApp->translate( "qChoosePixmap", ")\n" ) );
    filter += qApp->translate( "qChoosePixmap", "All Files (*)" );

    Q3FileDialog fd( QString::null, filter, parent, 0, true );
    fd.setMode( Q3FileDialog::ExistingFiles );
    fd.setContentsPreviewEnabled( true );
    PixmapView *pw = new PixmapView( &fd );
    fd.setContentsPreview( pw, pw );
    fd.setViewMode( Q3FileDialog::List );
    fd.setPreviewMode( Q3FileDialog::Contents );
    fd.setCaption( qApp->translate( "qChoosePixmap", "Choose Images" ) );
    if ( fd.exec() == QDialog::Accepted )
	return fd.selectedFiles();
    return QStringList();
}

QPixmap qChoosePixmap( QWidget *parent, FormWindow *fw, const QPixmap &old, QString *fn )
{
#if defined(DESIGNER)
    if ( !fw || fw->savePixmapInline() ) {
	if ( !imageIconProvider && !Q3FileDialog::iconProvider() )
	    Q3FileDialog::setIconProvider( ( imageIconProvider = new ImageIconProvider ) );

	QString filter;
	QString all = i18n( "All Pixmaps" );
	all += " (";
	for ( uint i = 0; i < QImageIO::outputFormats().count(); i++ ) {
	    filter += i18n( "%1-Pixmaps (%2)\n" , 
		      QImageIO::outputFormats().at( i ) , 
		      "*." + QString( QImageIO::outputFormats().at( i ) ).toLower() );
	    all += "*." + QString( QImageIO::outputFormats().at( i ) ).toLower() + ";";
	}
	filter.prepend( all + ")\n" );
	filter += i18n( "All Files (*)" );

	Q3FileDialog fd( QString::null, filter, parent, 0, true );
	fd.setContentsPreviewEnabled( true );
	PixmapView *pw = new PixmapView( &fd );
	fd.setContentsPreview( pw, pw );
	fd.setViewMode( Q3FileDialog::List );
	fd.setPreviewMode( Q3FileDialog::Contents );
	fd.setCaption( i18n( "Choose Pixmap" ) );
	if ( fd.exec() == QDialog::Accepted ) {
	    QPixmap pix( fd.selectedFile() );
	    if ( fn )
		*fn = fd.selectedFile();
	    MetaDataBase::setPixmapArgument( fw, pix.serialNumber(), fd.selectedFile() );
	    return pix;
	}
    }
#ifndef KOMMANDER
    else if ( fw && fw->savePixmapInProject() ) {
	PixmapCollectionEditor dia( parent, 0, true );
	dia.setProject( fw->project() );
	dia.setChooserMode( true );
	dia.setCurrentItem( MetaDataBase::pixmapKey( fw, old.serialNumber() ) );
	if ( dia.exec() == QDialog::Accepted ) {
	    QPixmap pix( fw->project()->pixmapCollection()->pixmap( dia.viewPixmaps->currentItem()->text() ) );
	    MetaDataBase::setPixmapKey( fw, pix.serialNumber(), dia.viewPixmaps->currentItem()->text() );
	    return pix;
	}
    }
#endif
    else {
	PixmapFunction dia( parent, 0, true );
	QObject::connect( dia.helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
	dia.labelFunction->setText( fw->pixmapLoaderFunction() + "(" );
	dia.editArguments->setText( MetaDataBase::pixmapArgument( fw, old.serialNumber() ) );
	dia.editArguments->setFocus();
	if ( dia.exec() == QDialog::Accepted ) {
	    QPixmap pix( PixmapChooser::loadPixmap( "image.xpm" ) );
	    MetaDataBase::setPixmapArgument( fw, pix.serialNumber(), dia.editArguments->text() );
	    return pix;
	}
    }
#else
    Q_UNUSED( parent );
    Q_UNUSED( fw );
    Q_UNUSED( old );
#endif
    return QPixmap();
}

ImageIconProvider::ImageIconProvider( QWidget *parent, const char *name )
    : Q3FileIconProvider( parent, name ), imagepm( PixmapChooser::loadPixmap( "image.xpm", PixmapChooser::Mini ) )
{
    fmts = QImage::inputFormats();
}

ImageIconProvider::~ImageIconProvider()
{
}

const QPixmap * ImageIconProvider::pixmap( const QFileInfo &fi )
{
    QString ext = fi.extension().toUpper();
    if ( fmts.contains( ext ) ) {
	return &imagepm;
    } else {
	return Q3FileIconProvider::pixmap( fi );
    }
}

PixmapChooser::PixmapChooser()
{
    // #### hardcoded at the moment
    miniPixDir = "pics/mini/";
    noSizePixDir = "pics/";
    smallPixDir = "pics/small/";
    largePixDir = "pics/large/";
}

QString PixmapChooser::pixmapPath( Size size ) const
{
    if ( size == Small )
	return smallPixDir;
    if ( size == Mini )
	return miniPixDir;
    if ( size == NoSize )
	return noSizePixDir;
    return largePixDir;
}

QPixmap PixmapChooser::loadPixmap( const QString &name, Size size )
{
#if defined(DESIGNER)
    if ( !pixmapChooser )
	pixmapChooser = new PixmapChooser;

    if ( name == "logo" )
	return QPixmap( logo_xpm );

    if ( name[ 0 ] == '/' || name[ 0 ] == '\\' || name[ 1 ] == ':' )
	return QPixmap( name );

    QString lookup;
    switch ( size ) {
    case Small:
	lookup = "small/";
	break;
    case Disabled:
	lookup = "small/disabled/";
	break;
    case Large:
	lookup = "large/";
	break;
    case Mini:
	lookup = "mini/";
	break;
    default:
	break;
    }

    QString lookup_png = lookup + QFileInfo( name ).baseName() + ".png";
    lookup += name;

    Embed *e = &embed_vec[ 0 ];
    while ( e->name ) {
	if ( QString( e->name ) == lookup ) {
	    QImage img;
	    img.loadFromData( (const uchar*)e->data, e->size );
	    QPixmap pix;
	    pix.convertFromImage( img );
	    return pix;
	} else if ( QString( e->name ) == lookup_png ) {
	    QImage img;
	    img.loadFromData( (const uchar*)e->data, e->size );
	    QPixmap pix;
	    pix.convertFromImage( img );
	    return pix;
	}
	e++;
    }

    // fallback
    return QPixmap( pixmapChooser->pixmapPath( size ) + name );
#else
    Q_UNUSED( name );
    Q_UNUSED( size );
    return QPixmap();
#endif
}
#include "pixmapchooser.moc"
