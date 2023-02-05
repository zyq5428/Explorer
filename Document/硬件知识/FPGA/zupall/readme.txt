*******************************************************************************
** © Copyright 2022 Xilinx, Inc. All rights reserved.
** This file contains confidential and proprietary information of Xilinx, Inc. and 
** is protected under U.S. and international copyright and other intellectual property laws.
*******************************************************************************
**   ____  ____ 
**  /   /\/   / 
** /___/  \  /   Vendor: Xilinx 
** \   \   \/    
**  \   \        readme.txt Version: 1.14
**  /   /        Date Last Modified: 01/05/2022
** /___/   /\    Date Created: 12/14/15
** \   \  /  \   Associated Filename: zupall.zip
**  \___\/\___\ 
** 
**  Device: Zynq UltraScale+
**  Revision History: 
**	v1.0 (12/14/2015):	Initial release
**	v1.1 (03/28/2016):	Added "(Preliminary)" to all packages that are still in the design stage
**				Updated pinout for packages:
**					xczu2egsbva484
**					xczu2egsfva625
**					xczu2egsfvc784
**					xczu3egsbva484
**					xczu3egsfva625
**					xczu3egsfvc784
**					xczu11egffvb1517
**					xczu11egffvc1760
**					xczu11egffvf1517
**					xczu17egffvb1517
**					xczu17egffvc1760
**					xczu17egffvd1760
**					xczu17egffve1924
**					xczu19egffvb1517
**					xczu19egffvc1760
**					xczu19egffvd1760
**					xczu19egffve1924
**	v1.2 (07/12/2016):	Removed pinout for packages:
**					xczu11egffvb1517
**					xczu11egffvc1760
**					xczu11egffvf1517
**					xczu17egffvb1517
**					xczu17egffvd1760
**					xczu17egffve1924
**					xczu19egffvb1517
**					xczu19egffvd1760
**					xczu19egffve1924
**				Updated pinout for packages:
**					xczu15egffvb1156
**					xczu15egffvc900
**					xczu17egffvc1760
**					xczu19egffvc1760
**					xczu19egffve1924
**				Added pinout for packages:
**					xczu2cgsbva484
**					xczu2cgsfva625
**					xczu2cgsfvc784
**					xczu3cgsbva484
**					xczu3cgsfva625
**					xczu3cgsfvc784
**					xczu6cgffvb1156
**					xczu6cgffvc900
**					xczu9cgffvb1156
**					xczu9cgffvc900
**	v1.3 (08/30/2016):	Added pinout for packages:
**					xczu17egffvb1517
**					xczu19egffvb1517
**	v1.4 (10/26/2016):	Replaced all files with upgraded header version
**				Added xczu7evfbvb900
**				Added xczu7cgfbvb900
**				Added xczu7egfbvb900
**				Added xczu7evffvc1156
**				Added xczu17egffve1924
**				Added xczu17egffvd1760
**				Added xczu19egffve1924
**				Added xczu19egffvd1760
**	v1.5 (12/01/2016):	Added xczu7egffvc1156
**				Added xczu7cgffvc1156
**	v1.6 (01/25/2017):	Added xczu7egffvf1517
**				Added xczu7cgffvf1517
**				Added xczu7evffvf1517
**				Added xczu11egffvc1760
**	v1.7 (05/16/2017):	Added xczu11egffvc1156
**				Added xczu4cgsfvc784
**				Added xczu4egsfvc784
**				Added xczu4evsfvc784
**				Added xczu5cgsfvc784
**				Added xczu5egsfvc784
**				Added xczu5evsfvc784
**				Added xczu11egffva1156
**				Added xczu11egffvf1517
**				Added xczu11egffvb1517
**				Added xczu4cgfbvb900
**				Added xczu4egfbvb900
**				Added xczu4evfbvb900
**				Added xczu5cgfbvb900
**				Added xczu5egfbvb900
**				Added xczu5evfbvb900
**				Updated xczu2cgsbva484
**				Updated xczu2cgsfva625
**				Updated xczu2cgsfvc784
**				Updated xczu2egsbva484
**				Updated xczu2egsfva625
**				Updated xczu2egsfvc784
**				Updated xczu3cgsbva484
**				Updated xczu3cgsfva625
**				Updated xczu3cgsfvc784
**				Updated xczu3egsbva484
**				Updated xczu3egsfva625
**				Updated xczu3egsfvc784
**				Updated xczu6egffvb1156
**				Updated xczu6egffvc900
**				Updated xczu6cgffvb1156
**				Updated xczu6cgffvc900
**				Updated xczu9egffvb1156
**				Updated xczu9egffvc900
**				Updated xczu9cgffvb1156
**				Updated xczu9cgffvc900
**				Updated xczu15egffvc900
**				Updated xczu15egffvb1156
**	v1.8 (09/27/2017):	Updated xczu11egffvc1760
**				Updated xczu11egffvf1517
**				Updated xczu11egffvc1156
**				Updated xczu11egffvb1517
**				Updated xczu17egffvb1517
**				Updated xczu17egffvc1760
**				Updated xczu17egffvd1760
**				Updated xczu17egffve1924
**				Updated xczu19egffvb1517
**				Updated xczu19egffvc1760
**				Updated xczu19egffvd1760
**				Updated xczu19egffve1924
**	v1.9 (12/05/2017):	Updated xczu4cgfbvb900
**				Updated xczu4cgsfvc784
**				Updated xczu4egfbvb900
**				Updated xczu4egsfvc784
**				Updated xczu4evfbvb900
**				Updated xczu4evsfvc784
**				Updated xczu5cgfbvb900
**				Updated xczu5cgsfvc784
**				Updated xczu5egfbvb900
**				Updated xczu5egsfvc784
**				Updated xczu5evfbvb900
**				Updated xczu5evsfvc784
**				Updated xczu7cgfbvb900
**				Updated xczu7cgffvc1156
**				Updated xczu7cgffvf1517
**				Updated xczu7egfbvb900
**				Updated xczu7egffvc1156
**				Updated xczu7egffvf1517
**				Updated xczu7evfbvb900
**				Updated xczu7evffvc1156
**				Updated xczu7evffvf1517
**	v1.10 (03/06/2018):	Added xczu21drffvd1156
**				Added xczu25drffve1156
**				Added xczu25drffvg1517
**				Added xczu25drfsve1156
**				Added xczu25drfsvg1517
**				Added xczu27drffve1156
**				Added xczu27drffvg1517
**				Added xczu27drfsve1156
**				Added xczu27drfsvg1517
**				Added xczu28drffve1156
**				Added xczu28drffvg1517
**				Added xczu28drfsve1156
**				Added xczu28drfsvg1517
**				Added xczu29drffvf1760
**				Added xczu29drfsvf1760
**				Added xqzu21drffrd1156
**				Added xqzu28drffre1156
**				Added xqzu28drffrg1517
**				Added xqzu29drffrf1760
**				Added xqzu11egffrc1156
**				Added xqzu11egffrc1760
**				Added xqzu15egffrb1156
**				Added xqzu15egffrc900
**				Added xqzu3egsfra484
**				Added xqzu3egsfrc784
**				Added xqzu5evsfrc784
**				Added xqzu7evffrb900
**				Added xqzu7evffrc1156
**				Added xqzu9egffrb1156
**				Added xqzu9egffrc900
**	v1.11 (08/10/2018):	Updated to Production:
**					xczu21drffvd1156
**					xczu25drffve1156
**					xczu25drffvg1517
**					xczu25drfsve1156
**					xczu25drfsvg1517
**					xczu27drffve1156
**					xczu27drffvg1517
**					xczu27drfsve1156
**					xczu27drfsvg1517
**					xczu28drffve1156
**					xczu28drffvg1517
**					xczu28drfsve1156
**					xczu28drfsvg1517
**					xczu29drffvf1760
**					xczu29drfsvf1760
**	v1.12 (11/06/2018):	Added:
**					xqzu5evffrb900
**					xqzu19egffrb1517
**					xqzu19egffrc1760
**				Updated:
**					xqzu11egffrc1156
**					xqzu11egffrc1760
**					xqzu15egffrb1156
**					xqzu15egffrc900
**					xqzu21drffrd1156
**					xqzu28drffre1156
**					xqzu28drffrg1517
**					xqzu29drffrf1760
**					xqzu3egsfra484
**					xqzu3egsfrc784
**					xqzu5evsfrc784
**					xqzu7evffrb900
**					xqzu7evffrc1156
**					xqzu9egffrb1156
**					xqzu9egffrc900
**	v1.13 (06/03/2020):	Added:
**					xczu39drffvf1760
**					xczu39drfsvf1760
**					xczu43drffve1156
**					xczu43drffvg1517
**					xczu43drfsve1156
**					xczu43drfsvg1517
**					xczu46drffvh1760
**					xczu46drfsvh1760
**					xczu47drffve1156
**					xczu47drffvg1517
**					xczu47drfsve1156
**					xczu47drfsvg1517
**					xczu48drffve1156
**					xczu48drffvg1517
**					xczu48drfsve1156
**					xczu48drfsvg1517
**					xczu49drffvf1760
**					xczu49drfsvf1760
**	v1.14 (01/05/2022):	Added:
**					xczu1cgsbva484
**					xczu1cgsfva625
**					xczu1cgsfvc784
**					xczu1cgubva494
**					xczu1egsbva484
**					xczu1egsfva625
**					xczu1egsfvc784
**					xczu1egubva494
**					xczu21drfsvd1156
**					xczu2cgubva530
**					xczu2egubva530
**					xczu3cgubva530
**					xczu3egubva530
**					xczu42drffve1156
**					xczu42drfsve1156
**					xczu65drffve1156
**					xczu65drfsve1156
**					xczu67drffve1156
**					xczu67drfsve1156
**					xqzu48drffre1156
**					xqzu48drfsre1156
**					xqzu48drfsrg1517
**					xqzu49drfsrf1760
**					xqzu65drffre1156
**					xqzu67drffre1156
**				Updated (from Engineering Sample to Production):
**					xczu43drfsve1156
**					xczu43drfsvg1517
**					xczu46drffvh1760
**					xczu46drfsvh1760
**					xczu47drffve1156
**					xczu47drffvg1517
**					xczu47drfsve1156
**					xczu47drfsvg1517
**					xczu48drffve1156
**					xczu48drffvg1517
**					xczu48drfsve1156
**					xczu48drfsvg1517
**					xczu49drffvf1760
**					xczu49drfsvf1760
*******************************************************************************
**
**  Disclaimer: 
**
**	This disclaimer is not a license and does not grant any rights to the materials 
**             	distributed herewith. Except as otherwise provided in a valid license issued to you 
**	by Xilinx, and to the maximum extent permitted by applicable law: 
**	(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, 
**	AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, 
**	INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR 
**	FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether in contract 
**	or tort, including negligence, or under any other theory of liability) for any loss or damage 
**	of any kind or nature related to, arising under or in connection with these materials, 
**	including for any direct, or any indirect, special, incidental, or consequential loss 
**	or damage (including loss of data, profits, goodwill, or any type of loss or damage suffered 
**	as a result of any action brought by a third party) even if such damage or loss was 
**	reasonably foreseeable or Xilinx had been advised of the possibility of the same.


**  Critical Applications:
**
**	Xilinx products are not designed or intended to be fail-safe, or for use in any application 
**	requiring fail-safe performance, such as life-support or safety devices or systems, 
**	Class III medical devices, nuclear facilities, applications related to the deployment of airbags,
**	or any other applications that could lead to death, personal injury, or severe property or 
**	environmental damage (individually and collectively, "Critical Applications"). Customer assumes 
**	the sole risk and liability of any use of Xilinx products in Critical Applications, subject only 
**	to applicable laws and regulations governing limitations on product liability.

**  THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT ALL TIMES.

*******************************************************************************
** IMPORTANT NOTES **

1) These package files contain advance information and are subject to change without notice and are provided solely for information purposes.

2) Please refer to the Zynq UltraScale+ Package Files section of UG1075, Zynq UltraScale+ Packaging and Pinouts, for detailed information on the contents of the package files.

 
