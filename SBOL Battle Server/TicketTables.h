#pragma once
#include <stdint.h>
#include "structures.h"

const int16_t CARTICKET_LOOKUP[165]{
	0, //AE86L3
	1, //AE86T3
	2, //AE86L2
	3, //AE86T2
	4, //SW20GT
	-1, //SW20G
	5, //A80RZ
	-1, //A80SZ
	6, //A80RZM
	7, //A80SZM
	8, //XE10RS
	-1, //XE10AS
	9, //100TV
	10, //100TVM
	-1, //100MV
	11, //100MVM
	12, //S13K
	13, //S13Q
	14, //PS13K
	-1, //PS13Q
	-1, //PS13KK
	-1, //S14K
	15, //S14Q
	-1, //S14KM
	16, //S14QM
	17, //S15R
	18, //S15S
	19, //RPS13X
	20, //RPS133
	21, //R32RV2
	22, //R32RN
	23, //R32GTM
	-1, //R33RV
	24, //R33RVM
	-1, //R34RV
	-1, //R34RVM
	25, //R34GT
	26, //R34GTM
	27, //Z32VS
	28, //Z32ZX
	29, //Y33CV
	30, //Y33GTU
	-1, //EK9
	-1, //EK9M
	-1, //EKC
	-1, //DC2
	-1, //DC2M
	-1, //DB8
	-1, //DB8M
	-1, //NA1
	-1, //NA2
	31, //CE9A
	32, //CN9A
	33, //CP9A5
	34, //CP9A6
	35, //CP9A6M
	36, //FC3E3
	-1, //FC3X
	37, //FD3RS
	38, //FD3RZ
	39, //GC8K
	40, //GC8S4
	41, //GC8C4
	42, //GC8S5
	43, //GC8C5
	44, //GC8S6
	45, //GC8C6
	46, //GF8W4
	47, //GF8W5
	48, //GF8W6
	-1, //AP1
	-1, //AP1HT
	-1, //964T
	-1, //964C2
	49, //S30Z
	50, //S30ZG
	-1, //AE111RL
	-1, //AE111RT
	51, //UZZ30
	51, //JZZ30
	52, //NB8RS
	53, //NB8RSM
	-1, //EG6
	54, //ST205
	55, //ST202
	56, //Z15AM
	56, //Z15A
	-1, //Z16A
	57, //DE3A
	58, //N15N1
	59, //BE5
	60, //BH5B
	61, //T231
	62, //W30
	63, //Y34CV
	64, //Y34GU
	-1, //E36
	-1, //N5S16M
	-1, //N5S16
	65, //C34M
	66, //C34
	67, //S161V
	68, //S161VM
	-1, //EF8
	-1, //EF8G
	-1, //CH9
	-1, //CL1
	-1, //PP1
	69, //PG6SA
	70, //EA11R
	70, //EA21R
	71, //JCESE
	72, //A31C
	-1, //RF2
	73, //A187
	74, //RX3
	75, //KPGC10
	76, //R30M
	77, //R30
	78, //SAGTX
	-1, //VGTS
	-1, //S30ZX
	-1, //CHA
	-1, //CFA
	79, //D32AGS
	-1, //ECGT
	-1, //BB6S
	80, //NCP35
	81, //UCF30B
	82, //A70GTA
	83, //A70TTR
	84, //CT9A
	85, //TAGDA
	86, //TASTI
	87, //CBAEP
	-1, //100TVK
	-1, //AP1HTK
	-1, //JZZ30K
	-1, //Z15AMK
	-1, //NA2K
	-1, //A70GTK
	-1, //R34RKK
	-1, //Y34GUK
	-1, //UZZ30K
	-1, //R34RK
	-1, //Z32VSK
	-1, //A80RZK
	-1, //FD3RK
	-1, //NCP35K
	-1, //S15RK
	-1, //R33RK
	-1, //FD3RKK
	-1, //S13KKK
	-1, //Z32ZXK
	-1, //S14KMK
	-1, //Y33CVK
	-1, //S161VK
	-1, //A80RK
	-1, //FC3E3K
	-1, //Z16AK
	-1, //NA1K
	-1, //PS13XK
	-1, //S15RKK
	-1, //JCESEK
	-1  //R32RNK
};
const PARTTICKETTABLE Class_C_TicketTable{
	{ 88, 89, 90, 91, 92, 93, 94, -1 }, // Engine
	{ 95, 96, 97, 98, 99, 100, 101, 102, -1 }, // Muffler
	{ 103, 104, 105, 106, -1 }, // Transmission
	{ 107, 108, 109, 110, -1 }, // Differential
	{ 111, 112, 113, 114, 115, -1, -1 }, // Suspension
	{ 124, 125, 126, 127, 128, -1 }, // Body
	{ 129, 130, 131, 132, 133, -1 }, // Front Bumper
	{ 134, 135, -1 }, // Bonnet
	{ 138, -1 }, // Overfenders
	{ 136, 137, -1 }, // Mirror
	{ 139, 140, 141, 142, 143, -1 }, // Side skirt
	{ 144, 145, 146, 147, 148, -1 }, // Rear Bumper
	{ 149, 150, 151, 152, 153, -1 }, // Rear Spoiler
	{ 154, 155, -1 }, // Grill
	{ 156, 157, -1 }, // Lights
	{ 116, 117, 118, 119, 120, 121, 122, 123, -1, -1 }, // Tire & Brakes
	{ 158 } // Body Colour
};
const PARTTICKETTABLE Class_B_TicketTable[5]{
	{	// 0 Generic
		{ 159, 160, 161, 162, -1, -1, -1, -1 }, // Engine
		{ 163, 164, 165, 166, -1, -1, -1, -1, -1 }, // Muffler
		{ 167, 168, -1, -1, -1 }, // Transmission
		{ 169, 170, -1, -1, -1 }, // Differential
		{ 171, 172, 173, -1, -1, -1, -1 }, // Suspension
		{ 178, 179, 180, -1, -1, -1 }, // Body
		{ 181, 182, 183, -1, -1, -1 }, // Front Bumper
		{ 184, -1, -1 }, // Bonnet
		{ 186, -1 }, // Overfenders
		{ 185, -1, -1 }, // Mirror
		{ 187, 188, 189, -1, -1, -1 }, // Side skirt
		{ 190, 191, 192, -1, -1, -1 }, // Rear Bumper
		{ 193, 194, 195, -1, -1, -1 }, // Rear Spoiler
		{ 196, -1, -1 }, // Grill
		{ 197, -1, -1 }, // Lights
		{ 174, 175, 176, 177, -1, -1, -1, -1, -1, -1 }, // Tire & Brakes
		{ 198 } // Body Colour
	},
	{	// 1 Toyota
		{ -1, -1, -1, -1, 199, 200, 201, -1 }, // Engine
		{ -1, -1, -1, -1, 202, 203, 204, 205, -1 }, // Muffler
		{ -1, -1, 206, 207, -1 }, // Transmission
		{ -1, -1, 208, 209, -1 }, // Differential
		{ -1, -1, -1, -1, 210, 211, -1 }, // Suspension
		{ -1, -1, -1, 216, 217, -1 }, // Body
		{ -1, -1, -1, 218, 219, -1 }, // Front Bumper
		{ -1, 220, -1 }, // Bonnet
		{ -1, -1 }, // Overfenders
		{ -1, 221, -1 }, // Mirror
		{ -1, -1, -1, 222, 223, -1 }, // Side skirt
		{ -1, -1, -1, 224, 225, -1 }, // Rear Bumper
		{ -1, -1, -1, 226, 227, -1 }, // Rear Spoiler
		{ -1, 228, -1 }, // Grill
		{ -1, 229, -1 }, // Lights
		{ -1, -1, -1, -1, 212, 213, 214, 215, -1, -1 }, // Tire & Brakes
		{ -1 } // Body Colour
	},
	{	// 2 Nissan
		{ -1, -1, -1, -1, 230, 231, 232, -1 }, // Engine
		{ -1, -1, -1, -1, 233, 234, 235, 236, -1 }, // Muffler
		{ -1, -1, 237, 238, -1 }, // Transmission
		{ -1, -1, 239, 240, -1 }, // Differential
		{ -1, -1, -1, 241, 241, 242, -1 }, // Suspension
		{ -1, -1, -1, 247, 248, -1 }, // Body
		{ -1, -1, -1, 249, 250, -1 }, // Front Bumper
		{ -1, 251, -1 }, // Bonnet
		{ -1, -1 }, // Overfenders
		{ -1, 252, -1 }, // Mirror
		{ -1, -1, -1, 253, 254, -1 }, // Side skirt
		{ -1, -1, -1, 255, 256, -1 }, // Rear Bumper
		{ -1, -1, -1, 257, 258, -1 }, // Rear Spoiler
		{ -1, 259, -1 }, // Grill
		{ -1, 260, -1 }, // Lights
		{ -1, -1, -1, -1, 243, 244, 245, 246, -1, -1 }, // Tire & Brakes
		{ -1 } // Body Colour
	},
	{	// 3 Mitsubishi
		{ -1, -1, -1, -1, 261, 262, 263, -1 }, // Engine
		{ -1, -1, -1, -1, 264, 265, 266, 267, -1 }, // Muffler
		{ -1, -1, 268, 269, -1 }, // Transmission
		{ -1, -1, 270, 271, -1 }, // Differential
		{ -1, -1, -1, -1, 272, 273, -1 }, // Suspension
		{ -1, -1, -1, 278, 279, -1 }, // Body
		{ -1, -1, -1, 280, 281, -1 }, // Front Bumper
		{ -1, 282, -1 }, // Bonnet
		{ -1, -1 }, // Overfenders
		{ -1, 283, -1 }, // Mirror
		{ -1, -1, -1, 284, 285, -1 }, // Side skirt
		{ -1, -1, -1, 286, 287, -1 }, // Rear Bumper
		{ -1, -1, -1, 288, 289, -1 }, // Rear Spoiler
		{ -1, 290, -1 }, // Grill
		{ -1, 291, -1 }, // Lights
		{ -1, -1, -1, -1, 274, 275, 276, 277, -1, -1 }, // Tire & Brakes
		{ -1 } // Body Colour
	},
	{	// 4 Mazda
		{ -1, -1, -1, -1, 292, 293, 294, -1 }, // Engine
		{ -1, -1, -1, -1, 295, 296, 297, 298, -1 }, // Muffler
		{ -1, -1, 299, 300, -1 }, // Transmission
		{ -1, -1, 301, 302, -1 }, // Differential
		{ -1, -1, -1, -1, 303, 304, -1 }, // Suspension
		{ -1, -1, -1, 309, 310, -1 }, // Body
		{ -1, -1, -1, 311, 312, -1 }, // Front Bumper
		{ -1, 313, -1 }, // Bonnet
		{ -1, -1 }, // Overfenders
		{ -1, 314, -1 }, // Mirror
		{ -1, -1, -1, 315, 316, -1 }, // Side skirt
		{ -1, -1, -1, 317, 318, -1 }, // Rear Bumper
		{ -1, -1, -1, 319, 320, -1 }, // Rear Spoiler
		{ -1, 321, -1 }, // Grill
		{ -1, 322, -1 }, // Lights
		{ -1, -1, -1, -1, 305, 306, 307, 308, -1, -1 }, // Tire & Brakes
		{ -1 } // Body Colour
	},
};