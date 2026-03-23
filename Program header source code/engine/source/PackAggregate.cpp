/**
 * \file      PackAggregate.cpp
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2006-7-19  21:11:00
 * \brief     Œƒº˛∞¸‘ÿ»Î£¨÷ß≥÷º”√‹—πÀı
 * 
 *	      Œƒº˛∞¸‘ÿ»Î£¨÷ß≥÷º”√‹—πÀı
 */

#include "../include/engine.h"
#include "../include/packaggregate.h"
#include "../../EncDec/include/EncDec.h"
#include "../../Zlib/Zlib.h"
#include "../include/crc.h"

const char c_szPackMagic[] = "XPAK";
const DWORD c_dwPackVersion = 9;
 
static t_DES_SPtrans Data_DES_SPtrans = {
{
/* nibble 0 */
0x02080800L, 0x00080000L, 0x02000002L, 0x02080802L,
0x02000000L, 0x00080802L, 0x00080002L, 0x02000002L,
0x00080802L, 0x02080800L, 0x02080000L, 0x00000802L,
0x02000802L, 0x02000000L, 0x00000000L, 0x00080002L,
0x00080000L, 0x00000002L, 0x02000800L, 0x00080800L,
0x02080802L, 0x02080000L, 0x00000802L, 0x02000800L,
0x00000002L, 0x00000800L, 0x00080800L, 0x02080002L,
0x00000800L, 0x02000802L, 0x02080002L, 0x00000000L,
0x00000000L, 0x02080802L, 0x02000800L, 0x00080002L,
0x02080800L, 0x00080000L, 0x00000802L, 0x02000800L,
0x02080002L, 0x00000800L, 0x00080800L, 0x02000002L,
0x00080802L, 0x00000002L, 0x02000002L, 0x02080000L,
0x02080802L, 0x00080800L, 0x02080000L, 0x02000802L,
0x02000000L, 0x00000802L, 0x00080002L, 0x00000000L,
0x00080000L, 0x02000000L, 0x02000802L, 0x02080800L,
0x00000002L, 0x02080002L, 0x00000800L, 0x00080802L,
},{
/* nibble 1 */
0x40108010L, 0x00000000L, 0x00108000L, 0x40100000L,
0x40000010L, 0x00008010L, 0x40008000L, 0x00108000L,
0x00008000L, 0x40100010L, 0x00000010L, 0x40008000L,
0x00100010L, 0x40108000L, 0x40100000L, 0x00000010L,
0x00100000L, 0x40008010L, 0x40100010L, 0x00008000L,
0x00108010L, 0x40000000L, 0x00000000L, 0x00100010L,
0x40008010L, 0x00108010L, 0x40108000L, 0x40000010L,
0x40000000L, 0x00100000L, 0x00008010L, 0x40108010L,
0x00100010L, 0x40108000L, 0x40008000L, 0x00108010L,
0x40108010L, 0x00100010L, 0x40000010L, 0x00000000L,
0x40000000L, 0x00008010L, 0x00100000L, 0x40100010L,
0x00008000L, 0x40000000L, 0x00108010L, 0x40008010L,
0x40108000L, 0x00008000L, 0x00000000L, 0x40000010L,
0x00000010L, 0x40108010L, 0x00108000L, 0x40100000L,
0x40100010L, 0x00100000L, 0x00008010L, 0x40008000L,
0x40008010L, 0x00000010L, 0x40100000L, 0x00108000L,
},{
/* nibble 2 */
0x04000001L, 0x04040100L, 0x00000100L, 0x04000101L,
0x00040001L, 0x04000000L, 0x04000101L, 0x00040100L,
0x04000100L, 0x00040000L, 0x04040000L, 0x00000001L,
0x04040101L, 0x00000101L, 0x00000001L, 0x04040001L,
0x00000000L, 0x00040001L, 0x04040100L, 0x00000100L,
0x00000101L, 0x04040101L, 0x00040000L, 0x04000001L,
0x04040001L, 0x04000100L, 0x00040101L, 0x04040000L,
0x00040100L, 0x00000000L, 0x04000000L, 0x00040101L,
0x04040100L, 0x00000100L, 0x00000001L, 0x00040000L,
0x00000101L, 0x00040001L, 0x04040000L, 0x04000101L,
0x00000000L, 0x04040100L, 0x00040100L, 0x04040001L,
0x00040001L, 0x04000000L, 0x04040101L, 0x00000001L,
0x00040101L, 0x04000001L, 0x04000000L, 0x04040101L,
0x00040000L, 0x04000100L, 0x04000101L, 0x00040100L,
0x04000100L, 0x00000000L, 0x04040001L, 0x00000101L,
0x04000001L, 0x00040101L, 0x00000100L, 0x04040000L,
},{
/* nibble 3 */
0x00401008L, 0x10001000L, 0x00000008L, 0x10401008L,
0x00000000L, 0x10400000L, 0x10001008L, 0x00400008L,
0x10000008L, 0x00401008L, 0x00400000L, 0x10000000L,
0x10400008L, 0x00401000L, 0x00001000L, 0x00000008L,
0x00401000L, 0x10001008L, 0x10400000L, 0x00001000L,
0x00001008L, 0x00000000L, 0x00400008L, 0x10401000L,
0x10001000L, 0x10400008L, 0x10401008L, 0x00400000L,
0x10400008L, 0x00001008L, 0x00400000L, 0x10000008L,
0x00000020L, 0x08000420L, 0x08010420L, 0x00010020L,
0x00401000L, 0x10001000L, 0x00000008L, 0x10400000L,
0x10001008L, 0x00000000L, 0x00001000L, 0x00400008L,
0x00000000L, 0x10400008L, 0x10401000L, 0x00001000L,
0x10000000L, 0x10401008L, 0x00401008L, 0x00400000L,
0x10401008L, 0x00000008L, 0x10001000L, 0x00401008L,
0x00400008L, 0x00401000L, 0x10400000L, 0x10001008L,
0x00001008L, 0x10000000L, 0x10000008L, 0x10401000L,
},{
/* nibble 4 */
0x08000000L, 0x00010000L, 0x00000400L, 0x08010420L,
0x10401000L, 0x10000008L, 0x10000000L, 0x00001008L,
0x08010020L, 0x08000400L, 0x00010420L, 0x08010000L,
0x00010000L, 0x00000020L, 0x08000020L, 0x00010400L,
0x08000420L, 0x08010020L, 0x08010400L, 0x00000000L,
0x00010400L, 0x08000000L, 0x00010020L, 0x00000420L,
0x08000400L, 0x00010420L, 0x00000000L, 0x08000020L,
0x08010000L, 0x00000400L, 0x00000420L, 0x08010400L,
0x08010400L, 0x08000420L, 0x00010020L, 0x08010000L,
0x00010000L, 0x00000020L, 0x08000020L, 0x08000400L,
0x08000000L, 0x00010400L, 0x08010420L, 0x00000000L,
0x00010420L, 0x08000000L, 0x00000400L, 0x00010020L,
0x08000420L, 0x00000400L, 0x00000000L, 0x08010420L,
0x08010020L, 0x08010400L, 0x00000420L, 0x00010000L,
0x00010400L, 0x08010020L, 0x08000400L, 0x00000420L,
0x00000020L, 0x00010420L, 0x08010000L, 0x08000020L,
},{
/* nibble 5 */
0x80000040L, 0x00200040L, 0x00000000L, 0x80202000L,
0x00200040L, 0x00002000L, 0x80002040L, 0x00200000L,
0x00002040L, 0x80202040L, 0x00202000L, 0x80000000L,
0x80002000L, 0x80000040L, 0x80200000L, 0x00202040L,
0x00200000L, 0x80002040L, 0x80200040L, 0x00000000L,
0x00002000L, 0x00000040L, 0x80202000L, 0x80200040L,
0x80202040L, 0x80200000L, 0x80000000L, 0x00002040L,
0x00000040L, 0x00202000L, 0x00202040L, 0x80002000L,
0x00002040L, 0x80000000L, 0x80002000L, 0x00202040L,
0x80202000L, 0x00200040L, 0x00000000L, 0x80002000L,
0x80000000L, 0x00002000L, 0x80200040L, 0x00200000L,
0x00200040L, 0x80202040L, 0x00202000L, 0x00000040L,
0x80202040L, 0x00202000L, 0x00200000L, 0x80002040L,
0x80000040L, 0x80200000L, 0x00202040L, 0x00000000L,
0x00002000L, 0x80000040L, 0x80002040L, 0x80202000L,
0x80200000L, 0x00002040L, 0x00000040L, 0x80200040L,
},{
/* nibble 6 */
0x00004000L, 0x00000200L, 0x01000200L, 0x01000004L,
0x01004204L, 0x00004004L, 0x00004200L, 0x00000000L,
0x01000000L, 0x01000204L, 0x00000204L, 0x01004000L,
0x00000004L, 0x01004200L, 0x01004000L, 0x00000204L,
0x01000204L, 0x00004000L, 0x00004004L, 0x01004204L,
0x00000000L, 0x01000200L, 0x01000004L, 0x00004200L,
0x01004004L, 0x00004204L, 0x01004200L, 0x00000004L,
0x00004204L, 0x01004004L, 0x00000200L, 0x01000000L,
0x00004204L, 0x01004000L, 0x01004004L, 0x00000204L,
0x00004000L, 0x00000200L, 0x01000000L, 0x01004004L,
0x01000204L, 0x00004204L, 0x00004200L, 0x00000000L,
0x00000200L, 0x01000004L, 0x00000004L, 0x01000200L,
0x00000000L, 0x01000204L, 0x01000200L, 0x00004200L,
0x00000204L, 0x00004000L, 0x01004204L, 0x01000000L,
0x01004200L, 0x00000004L, 0x00004004L, 0x01004204L,
0x01000004L, 0x01004200L, 0x01004000L, 0x00004004L,
},{
/* nibble 7 */
0x20800080L, 0x20820000L, 0x00020080L, 0x00000000L,
0x20020000L, 0x00800080L, 0x20800000L, 0x20820080L,
0x00000080L, 0x20000000L, 0x00820000L, 0x00020080L,
0x00820080L, 0x20020080L, 0x20000080L, 0x20800000L,
0x00020000L, 0x00820080L, 0x00800080L, 0x20020000L,
0x20820080L, 0x20000080L, 0x00000000L, 0x00820000L,
0x20000000L, 0x00800000L, 0x20020080L, 0x20800080L,
0x00800000L, 0x00020000L, 0x20820000L, 0x00000080L,
0x00800000L, 0x00020000L, 0x20000080L, 0x20820080L,
0x00020080L, 0x20000000L, 0x00000000L, 0x00820000L,
0x20800080L, 0x20020080L, 0x20020000L, 0x00800080L,
0x20820000L, 0x00000080L, 0x00800080L, 0x20020000L,
0x20820080L, 0x00800000L, 0x20800000L, 0x20000080L,
0x00820000L, 0x00020080L, 0x20020080L, 0x20800000L,
0x00000080L, 0x20820000L, 0x00820080L, 0x00000000L,
0x20000000L, 0x20800080L, 0x00020000L, 0x00820080L,
}};

static t_DES_SPtrans HeaderData_DES_SPtrans = {
{
/* nibble 0 */
0x02080800L, 0x00080000L, 0x02000002L, 0x02080802L,
0x02000000L, 0x00080802L, 0x00080002L, 0x02000002L,
0x00080802L, 0x02080800L, 0x02080000L, 0x00000802L,
0x02080802L, 0x00080800L, 0x02080000L, 0x02000802L,
0x02000802L, 0x02000000L, 0x00000000L, 0x00080002L,
0x00080000L, 0x00000002L, 0x02000800L, 0x00080800L,
0x02080802L, 0x02080000L, 0x00000802L, 0x02000800L,
0x00000002L, 0x00000800L, 0x00080800L, 0x02080002L,
0x00000800L, 0x02000802L, 0x02080002L, 0x00000000L,
0x00000000L, 0x02080802L, 0x02000800L, 0x00080002L,
0x02080800L, 0x00080000L, 0x00000802L, 0x02000800L,
0x02080002L, 0x00000800L, 0x00080800L, 0x02000002L,
0x00080802L, 0x00000002L, 0x02000002L, 0x02080000L,
0x02000000L, 0x00000802L, 0x00080002L, 0x00000000L,
0x00080000L, 0x02000000L, 0x02000802L, 0x02080800L,
0x00000002L, 0x02080002L, 0x00000800L, 0x00080802L,
},{
/* nibble 1 */
0x40108010L, 0x00000000L, 0x00108000L, 0x40100000L,
0x40000010L, 0x00008010L, 0x40008000L, 0x00108000L,
0x00008000L, 0x40100010L, 0x00000010L, 0x40008000L,
0x00100010L, 0x40108000L, 0x40100000L, 0x00000010L,
0x00100000L, 0x40008010L, 0x40100010L, 0x00008000L,
0x00108010L, 0x40000000L, 0x00000000L, 0x00100010L,
0x40008010L, 0x00108010L, 0x40108000L, 0x40000010L,
0x40000000L, 0x00100000L, 0x00008010L, 0x40108010L,
0x00100010L, 0x40108000L, 0x40008000L, 0x00108010L,
0x40108010L, 0x00100010L, 0x40000010L, 0x00000000L,
0x40000000L, 0x00008010L, 0x00100000L, 0x40100010L,
0x00008000L, 0x40000000L, 0x00108010L, 0x40008010L,
0x40108000L, 0x00008000L, 0x00000000L, 0x40000010L,
0x00000010L, 0x40108010L, 0x00108000L, 0x40100000L,
0x40100010L, 0x00100000L, 0x00008010L, 0x40008000L,
0x40008010L, 0x00000010L, 0x40100000L, 0x00108000L,
},{
/* nibble 2 */
0x04000001L, 0x04040100L, 0x00000100L, 0x04000101L,
0x00040001L, 0x04000000L, 0x04000101L, 0x00040100L,
0x04000100L, 0x00040000L, 0x04040000L, 0x00000001L,
0x04040101L, 0x00000101L, 0x00000001L, 0x04040001L,
0x00000000L, 0x00040001L, 0x04040100L, 0x00000100L,
0x00000101L, 0x04040101L, 0x00040000L, 0x04000001L,
0x00040101L, 0x04000001L, 0x04000000L, 0x04040101L,
0x04040001L, 0x04000100L, 0x00040101L, 0x04040000L,
0x00040100L, 0x00000000L, 0x04000000L, 0x00040101L,
0x04040100L, 0x00000100L, 0x00000001L, 0x00040000L,
0x00000101L, 0x00040001L, 0x04040000L, 0x04000101L,
0x00000000L, 0x04040100L, 0x00040100L, 0x04040001L,
0x00040001L, 0x04000000L, 0x04040101L, 0x00000001L,
0x00040000L, 0x04000100L, 0x04000101L, 0x00040100L,
0x04000100L, 0x00000000L, 0x04040001L, 0x00000101L,
0x04000001L, 0x00040101L, 0x00000100L, 0x04040000L,
},{
/* nibble 3 */
0x00401008L, 0x10001000L, 0x00000008L, 0x10401008L,
0x00000000L, 0x10400000L, 0x10001008L, 0x00400008L,
0x10000008L, 0x00401008L, 0x00400000L, 0x10000000L,
0x10400008L, 0x00401000L, 0x00001000L, 0x00000008L,
0x00401000L, 0x10001008L, 0x10400000L, 0x00001000L,
0x00001008L, 0x00000000L, 0x00400008L, 0x10401000L,
0x10001000L, 0x10400008L, 0x10401008L, 0x00400000L,
0x10400008L, 0x00001008L, 0x00400000L, 0x10000008L,
0x00000020L, 0x08000420L, 0x08010420L, 0x00010020L,
0x00401000L, 0x10001000L, 0x00000008L, 0x10400000L,
0x10001008L, 0x00000000L, 0x00001000L, 0x00400008L,
0x00000000L, 0x10400008L, 0x10401000L, 0x00001000L,
0x10000000L, 0x10401008L, 0x00401008L, 0x00400000L,
0x10401008L, 0x00000008L, 0x10001000L, 0x00401008L,
0x00400008L, 0x00401000L, 0x10400000L, 0x10001008L,
0x00001008L, 0x10000000L, 0x10000008L, 0x10401000L,
},{
/* nibble 5 */
0x80000040L, 0x00200040L, 0x00000000L, 0x80202000L,
0x00200040L, 0x00002000L, 0x80002040L, 0x00200000L,
0x00002040L, 0x80202040L, 0x00202000L, 0x80000000L,
0x00000040L, 0x00202000L, 0x00202040L, 0x80002000L,
0x00002040L, 0x80000000L, 0x80002000L, 0x00202040L,
0x80002000L, 0x80000040L, 0x80200000L, 0x00202040L,
0x00200000L, 0x80002040L, 0x80200040L, 0x00000000L,
0x00002000L, 0x00000040L, 0x80202000L, 0x80200040L,
0x80202040L, 0x80200000L, 0x80000000L, 0x00002040L,
0x80202000L, 0x00200040L, 0x00000000L, 0x80002000L,
0x80000000L, 0x00002000L, 0x80200040L, 0x00200000L,
0x00200040L, 0x80202040L, 0x00202000L, 0x00000040L,
0x80202040L, 0x00202000L, 0x00200000L, 0x80002040L,
0x80000040L, 0x80200000L, 0x00202040L, 0x00000000L,
0x00002000L, 0x80000040L, 0x80002040L, 0x80202000L,
0x80200000L, 0x00002040L, 0x00000040L, 0x80200040L,
},{
/* nibble 6 */
0x00004000L, 0x00000200L, 0x01000200L, 0x01000004L,
0x01004204L, 0x00004004L, 0x00004200L, 0x00000000L,
0x01000000L, 0x01000204L, 0x00000204L, 0x01004000L,
0x00000004L, 0x01004200L, 0x01004000L, 0x00000204L,
0x01000204L, 0x00004000L, 0x00004004L, 0x01004204L,
0x00000000L, 0x01000200L, 0x01000004L, 0x00004200L,
0x01004004L, 0x00004204L, 0x01004200L, 0x00000004L,
0x00004204L, 0x01004004L, 0x00000200L, 0x01000000L,
0x00004204L, 0x01004000L, 0x01004004L, 0x00000204L,
0x00004000L, 0x00000200L, 0x01000000L, 0x01004004L,
0x01000204L, 0x00004204L, 0x00004200L, 0x00000000L,
0x00000200L, 0x01000004L, 0x00000004L, 0x01000200L,
0x00000000L, 0x01000204L, 0x01000200L, 0x00004200L,
0x00000204L, 0x00004000L, 0x01004204L, 0x01000000L,
0x01004200L, 0x00000004L, 0x00004004L, 0x01004204L,
0x01000004L, 0x01004200L, 0x01004000L, 0x00004004L,
},{
/* nibble 4 */
0x08000000L, 0x00010000L, 0x00000400L, 0x08010420L,
0x10401000L, 0x10000008L, 0x10000000L, 0x00001008L,
0x08010020L, 0x08000400L, 0x00010420L, 0x08010000L,
0x00010000L, 0x00000020L, 0x08000020L, 0x00010400L,
0x08000420L, 0x08010020L, 0x08010400L, 0x00000000L,
0x00010400L, 0x08000000L, 0x00010020L, 0x00000420L,
0x08000400L, 0x00010420L, 0x00000000L, 0x08000020L,
0x08010000L, 0x00000400L, 0x00000420L, 0x08010400L,
0x08010400L, 0x08000420L, 0x00010020L, 0x08010000L,
0x00010000L, 0x00000020L, 0x08000020L, 0x08000400L,
0x08000000L, 0x00010400L, 0x08010420L, 0x00000000L,
0x00010420L, 0x08000000L, 0x00000400L, 0x00010020L,
0x08000420L, 0x00000400L, 0x00000000L, 0x08010420L,
0x08010020L, 0x08010400L, 0x00000420L, 0x00010000L,
0x00010400L, 0x08010020L, 0x08000400L, 0x00000420L,
0x00000020L, 0x00010420L, 0x08010000L, 0x08000020L,
},{
/* nibble 7 */
0x20800080L, 0x20820000L, 0x00020080L, 0x00000000L,
0x20020000L, 0x00800080L, 0x20800000L, 0x20820080L,
0x00000080L, 0x20000000L, 0x00820000L, 0x00020080L,
0x00820080L, 0x20020080L, 0x20000080L, 0x20800000L,
0x00020000L, 0x00820080L, 0x00800080L, 0x20020000L,
0x20820080L, 0x20000080L, 0x00000000L, 0x00820000L,
0x20000000L, 0x00800000L, 0x20020080L, 0x20800080L,
0x00800000L, 0x00020000L, 0x20820000L, 0x00000080L,
0x00800000L, 0x00020000L, 0x20000080L, 0x20820080L,
0x00020080L, 0x20000000L, 0x00000000L, 0x00820000L,
0x20800080L, 0x20020080L, 0x20020000L, 0x00800080L,
0x20820000L, 0x00000080L, 0x00800080L, 0x20020000L,
0x20820080L, 0x00800000L, 0x20800000L, 0x20000080L,
0x00820000L, 0x00020080L, 0x20020080L, 0x20800000L,
0x00000080L, 0x20820000L, 0x00820080L, 0x00000000L,
0x20000000L, 0x20800080L, 0x00020000L, 0x00820080L,
}};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pszPassword : √Ë ˆ
 * \param key1 : √Ë ˆ
 * \param key2 : √Ë ˆ
 * \param key3 : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void PasswordToDesKey(const char* pszPassword,BYTE* key1,BYTE * key2,BYTE* key3)
{
	BYTE md5[16];
	Assert(pszPassword);
	MD5String(pszPassword,md5);
	memcpy(key1,md5,8);
	memcpy(key2,md5+8,8);
	memcpy(key3,md5+4,8);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param inData : √Ë ˆ
 * \param len : √Ë ˆ
 * \param key1 : √Ë ˆ
 * \param key2 : √Ë ˆ
 * \param key3 : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void EncryptData(BYTE* inData,size_t len,const BYTE* key1,const BYTE * key2,const BYTE* key3)
{
    Assert((len&0x7) == 0);
	//*
#ifdef _DEBUG
	BYTE* oldData = new BYTE[len];
	memcpy(oldData,inData,len);
#endif
	//*/
	DES_key_schedule sKey[3];

	DES_set_key((const_DES_cblock*)key1,&sKey[0]);
	DES_set_key((const_DES_cblock*)key2,&sKey[1]);
	DES_set_key((const_DES_cblock*)key3,&sKey[2]);

    for(size_t i=0;i<len;i+=8)
    {
        DES_encrypt3((DWORD*)(inData + i),&sKey[0],&sKey[1],&sKey[2],&Data_DES_SPtrans);
    }

	//*
#ifdef _DEBUG
	BYTE* newData = new BYTE[len];
	memcpy(newData,inData,len);
	DecryptData(newData,len,key1,key2,key3);
	int cmp = memcmp(newData,oldData,len);
	Assert(cmp == 0);
	delete [] newData;
	delete [] oldData;
#endif
	//*/
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param inData : √Ë ˆ
 * \param len : √Ë ˆ
 * \param key1 : √Ë ˆ
 * \param key2 : √Ë ˆ
 * \param key3 : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void DecryptData(BYTE* inData,size_t len,const BYTE* key1,const BYTE * key2,const BYTE* key3)
{
    Assert((len&0x7) == 0);

	DES_key_schedule sKey[3];

	DES_set_key((const_DES_cblock*)key1,&sKey[0]);
	DES_set_key((const_DES_cblock*)key2,&sKey[1]);
	DES_set_key((const_DES_cblock*)key3,&sKey[2]);

    for(size_t i=0;i<len;i+=8)
    {
        DES_decrypt3((DWORD*)(inData + i),&sKey[0],&sKey[1],&sKey[2],&Data_DES_SPtrans);
    }
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param inData : √Ë ˆ
 * \param len : √Ë ˆ
 * \param key1 : √Ë ˆ
 * \param key2 : √Ë ˆ
 * \param key3 : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void DecryptHeaderData(BYTE* inData,size_t len,const BYTE* key1,const BYTE * key2,const BYTE* key3)
{
	Assert((len&0x7) == 0);

	DES_key_schedule sKey[3];

	DES_set_key((const_DES_cblock*)key1,&sKey[0]);
	DES_set_key((const_DES_cblock*)key2,&sKey[1]);
	DES_set_key((const_DES_cblock*)key3,&sKey[2]);

	for(size_t i=0;i<len;i+=8)
	{
		DES_decrypt3((DWORD*)(inData + i),&sKey[0],&sKey[1],&sKey[2],&HeaderData_DES_SPtrans);
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param inData : √Ë ˆ
 * \param len : √Ë ˆ
 * \param key1 : √Ë ˆ
 * \param key2 : √Ë ˆ
 * \param key3 : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void EncryptHeaderData(BYTE* inData,size_t len,const BYTE* key1,const BYTE * key2,const BYTE* key3)
{
	Assert((len&0x7) == 0);
	//*
	#ifdef _DEBUG
	BYTE* oldData = new BYTE[len];
	memcpy(oldData,inData,len);
	#endif
	//*/
	DES_key_schedule sKey[3];

	DES_set_key((const_DES_cblock*)key1,&sKey[0]);
	DES_set_key((const_DES_cblock*)key2,&sKey[1]);
	DES_set_key((const_DES_cblock*)key3,&sKey[2]);

	for(size_t i=0;i<len;i+=8)
	{
		DES_encrypt3((DWORD*)(inData + i),&sKey[0],&sKey[1],&sKey[2],&HeaderData_DES_SPtrans);
	}

	//*
	#ifdef _DEBUG
	BYTE* newData = new BYTE[len];
	memcpy(newData,inData,len);
	DecryptHeaderData(newData,len,key1,key2,key3);
	int cmp = memcmp(newData,oldData,len);
	Assert(cmp == 0);
	delete [] newData;
	delete [] oldData;
	#endif
	//*/
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
PackAggregate::PackAggregate(void)
{
	m_szFileName[0] = 0;
	m_dwFlags = 0;
	m_pDesKey = NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
PackAggregate::~PackAggregate(void)
{
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void PackAggregate::closeAggregate()
{
	m_aFileInfo.resize(0);
	m_szFileName[0] = 0;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool PackAggregate::refreshAggregate()
{
   AssertFatal(m_szFileName[0] != 0, "No filename?  Must not be open.  Disallowed");

   char tmpBuff[MAX_PATH];
   strcpy(tmpBuff, m_szFileName);
   return openAggregate(tmpBuff);
}

bool MyDeleteFile(const char* fileName)
{
	DWORD dwAttrib = GetFileAttributes(fileName);
	if(dwAttrib == -1) return false;
	dwAttrib &= (~FILE_ATTRIBUTE_READONLY);
	SetFileAttributes(fileName,dwAttrib); 
	if(!::DeleteFile(fileName))
	{
		DWORD error = GetLastError();
#ifdef _DEBUG
		assert(0 && fileName);
#endif
		return false;
	}
	return true;
}

bool UpdateFile(const char* srcFileName,const char* dstFileName)
{
	if(IsFileExist(dstFileName))
	{
		if(!::MyDeleteFile(dstFileName))
		{
			return false;
		}
	}
	if(!::MoveFile(srcFileName,dstFileName)) return false;
	return true;
}

bool PackAggregate::resortFilePack(const char* pack,std::vector<PackAggregate::stFileInfo> & files)
{
	char szDir[MAX_PATH];
	char tmpFile[MAX_PATH];
	char* p;

	strcpy(szDir,pack);

	p = strrchr(szDir,'\\');
	if(p) 
		*p = 0;

	if(!GetTempFileName(szDir,"pak",0,tmpFile))
	{
		return false;
	}

	if(!CopyFile(pack,tmpFile,FALSE))
		return false;

	FileStream out;
	if(!out.open(tmpFile,FileStream::ReadWrite)){
		DeleteFile(tmpFile);
		return false;
	}

	out.setPosition(0);
	stPackHeader hdr;
	out.read(&hdr,sizeof(hdr));

	size_t filesize = out.getSize();
	out.setPosition(filesize - hdr.headerSize);

	std::vector<BYTE> tmpBuf;
	tmpBuf.resize(files.size() * sizeof(PackAggregate::stFileInfo));
	p = (char*)&tmpBuf[0];
	for(size_t i=0; i<files.size(); ++i)
	{
		stFileInfo1 * dst1 = (stFileInfo1*)p;
		stFileInfo& st = files[i];
		p += sizeof(stFileInfo1);

		*dst1 = st.st1;

		char* pname = (char*)p;
		strcpy(pname,st.szName);
		p += strlen(pname)+1;
	}

	size_t size = ((p - (char*)&tmpBuf[0]) + 7) & (~7);

	BYTE des_key[3][8];
	PasswordToDesKey( PackAggregate::GetDefaultPassword(),des_key[0],des_key[1],des_key[2] );
	EncryptHeaderData(&tmpBuf[0],size,des_key[0],des_key[1],des_key[2]);
	
	if(!out.write(&tmpBuf[0],size)){
		out.close();
		DeleteFile(tmpFile);
		return false;
	}
	out.close();
	return UpdateFile(tmpFile,pack);
}

bool PackAggregate::getFileInfo(const char* szFileName,std::vector<PackAggregate::stFileInfo> & files)
{
	FileStream stream;
	if(!stream.open(szFileName,FileStream::Read))
		return false;

	size_t filesize = stream.getSize();
	stPackHeader hdr;
	stream.read(&hdr,sizeof(hdr));

	if(strncmp((char*)&hdr.magic , c_szPackMagic,4))
		return false;

	if(hdr.version != c_dwPackVersion && hdr.version != 8)
		return false;

	stream.setPosition(filesize - hdr.headerSize);

	std::vector<BYTE> aBuf;
	aBuf.reserve(hdr.headerSize);
	
	stream.read(&aBuf[0],hdr.headerSize);

	BYTE des_key[3][8];
	PasswordToDesKey( PackAggregate::GetDefaultPassword(),des_key[0],des_key[1],des_key[2] );

	if( (hdr.flag & flagEncrypt) || (hdr.flag & flagEncryptHeaderOnly) )
	{
		DecryptHeaderData
			( 
			(BYTE*)&aBuf[0],
			hdr.headerSize,
			des_key[0],des_key[1] ,des_key[2] 
			);
	}

	BYTE* p = &aBuf[0];
	files.resize(hdr.filenum);
	for(int i=0;i<hdr.filenum;++i)
	{
		stFileInfo & st = files[i];

		memcpy(&st.st1,p,sizeof(stFileInfo1));
		p += sizeof(stFileInfo1);
		strcpy(st.szName,(char*)p);
		p += strlen(st.szName) + 1;
	}

	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param szFileName : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool PackAggregate::openAggregate(const char* szFileName)
{
	closeAggregate();
	FileStream stream;
	if(!stream.open(szFileName,FileStream::Read))
		return false;

	size_t filesize = stream.getSize();
	stPackHeader hdr;
	stream.read(&hdr,sizeof(hdr));
	
	// 2009
//	hdr.filenum += 10;
//	hdr.headerSize += 10;

	if(strncmp((char*)&hdr.magic , c_szPackMagic,4))
		return false;

	if(hdr.version != c_dwPackVersion && hdr.version != 8)
		return false;

	m_dwFlags = hdr.flag;
	m_dwVersion = hdr.version;

	strcpy(m_szFileName,szFileName);

	stream.setPosition(filesize - hdr.headerSize);

	std::vector<BYTE> aBuf;
	aBuf.reserve(hdr.headerSize);

	stream.read(&aBuf[0],hdr.headerSize);
	
	if( (hdr.flag & flagEncrypt) || (hdr.flag & flagEncryptHeaderOnly) )
	{
		Assert( m_pDesKey );
		DecryptHeaderData
			( 
			(BYTE*)&aBuf[0],
			hdr.headerSize,
			m_pDesKey,m_pDesKey + 8 ,m_pDesKey+16 
			);
	}

	m_aFileInfo.resize(hdr.filenum);
	BYTE* p = &aBuf[0];
	for(int i=0;i<hdr.filenum;++i)
	{
		stFileInfo st;
		stFileBaseInfo &dst = m_aFileInfo[i];

		memcpy(&st.st1,p,sizeof(stFileInfo1));
		p += sizeof(stFileInfo1);
		strcpy(st.szName,(char*)p);
		p += strlen(st.szName) + 1;

		dst.size = st.size;
		dst.fileOffset = st.fileOffset + sizeof(hdr);
		dst.dwCRC = st.dwCRC;
		strcpy(dst.szName,st.szName);

		/*
		stream.setPosition(st.fileOffset + sizeof(hdr));

		stFileSizeInfo fInfo;
		stream.read(&fInfo,sizeof(fInfo));

		if( (hdr.flag & flagEncrypt) ||  (hdr.flag & flagEncryptHeaderOnly))
			DecryptData((BYTE*)&fInfo,sizeof(fInfo),m_pDesKey,m_pDesKey + 8 ,m_pDesKey+16);

		dst.fileOffset = stream.getPosition();
		dst.uncompressedSize = fInfo.uncompressedSize;
		dst.dwCRC = fInfo.dwCRC;

		if(hdr.version == c_dwPackVersion && (dst.flags & (~3)) == 0)
		{
			dst.flags = fInfo.flags;
		}
		else
		{
			dst.flags = hdr.flag;
			if(dst.uncompressedSize != dst.size - sizeof(fInfo))
				dst.flags |= flagCompress;
		}
		//*/
	}

	return true;
}

bool PackAggregate::getFileInfo(DWORD dwFileIndex,Stream* pStream,DWORD dwVersion,DWORD flags,
								DWORD offset,stFileDetailInfo * result)
{
	pStream->setPosition(offset);

	stFileSizeInfo fInfo;
	pStream->read(&fInfo,sizeof(fInfo));

	if( (flags & flagEncrypt) ||  (flags & flagEncryptHeaderOnly))
		DecryptData((BYTE*)&fInfo,sizeof(fInfo),m_pDesKey,m_pDesKey + 8 ,m_pDesKey+16);

	result->fileOffset = pStream->getPosition();
	result->uncompressedSize = fInfo.uncompressedSize;
	result->dwUnprocessCRC = fInfo.dwCRC;
	result->flags = fInfo.flags;

	return true;
}

struct FileInfo{
	char szName[MAX_PATH];
	DWORD size;
};

bool gMakeFilePackFindSubDir = true;
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param szDir : √Ë ˆ
 * \param aPack : √Ë ˆ
 * \param nPack : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void GetDirInfo(LPCSTR szDir,FileInfo* &aPack,int& nPack,std::vector<String> & fileFilter)
{
	// aPack±ÿ–Î «“˝”√£¨“ÚŒ™“™µ›πÈ
	char s[MAX_PATH];
	strcpy(s,szDir);
	strcat(s,"\\*.*");
	WIN32_FIND_DATA fd;
	memset(&fd,0,sizeof(fd));
	HANDLE hFd;
	hFd = FindFirstFile(s,&fd);
	
	BOOL b = (hFd != INVALID_HANDLE_VALUE);

	while(b)
	{
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) || (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
		{}
		else
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(strcmp(fd.cFileName,".") == 0 || strcmp(fd.cFileName,"..")==0 || stricmp(fd.cFileName,".svn") == 0 || stricmp(fd.cFileName,"_svn") == 0)
				{
					b = FindNextFile(hFd,&fd);
					continue;
				}
				if(gMakeFilePackFindSubDir)
				{
					char s1[MAX_PATH];
					sprintf(s1,"%s\\%s",szDir,fd.cFileName);
					GetDirInfo(s1,aPack,nPack,fileFilter);
				}
			}
			else if(fd.cFileName[0])
			{
				bool bAdd = true;
				if(!fileFilter.empty())
				{
					bAdd = false;
					char* pExt = strchr(fd.cFileName,'.');
					if(pExt)
					{
						pExt++;
						for(size_t i=0; i < fileFilter.size(); ++i)
						{
							if(stricmp(pExt,fileFilter[i].c_str()) == 0)
							{
								bAdd = true;
								break;
							}
						}
					}
				}
				if(bAdd)
				{
					if(aPack)
					{
						aPack[nPack].size =fd.nFileSizeLow;
						sprintf(aPack[nPack].szName,"%s\\%s",szDir,fd.cFileName);
						Assert(aPack[nPack].szName[0] != 0);
						strlwr(aPack[nPack].szName);
					}
					nPack++;
				}
			}
		}
		b = FindNextFile(hFd,&fd);
	}
	FindClose(hFd);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param szName : √Ë ˆ
 * \param n : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline void RemoveDirName(LPSTR szName, size_t n)
{
	if(n == 0) return;
	Assert(n <= strlen(szName));
	if(szName[n-1] != '\\')
	{
		n++;
	}
	strcpy(szName,szName + n );
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pOut : √Ë ˆ
 * \param pOutLen : √Ë ˆ
 * \param pIn : √Ë ˆ
 * \param nInLen : √Ë ˆ
 * \param level : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline int compress_data(unsigned char* pOut,unsigned long * pOutLen,unsigned char* pIn,unsigned long nInLen,int level)
{
	static z_stream_s zipStream;
	zipStream.zalloc = Z_NULL;
	zipStream.zfree  = Z_NULL;
	zipStream.opaque = Z_NULL;

	zipStream.next_in   = pIn;
	zipStream.avail_in  = nInLen;
	zipStream.total_in  = 0;
	zipStream.next_out  = pOut;
	zipStream.avail_out = *pOutLen;
	zipStream.total_out = 0;

	int err;
	err = deflateInit2(&zipStream, level, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if (err != Z_OK) return err;
	err = deflate(&zipStream, Z_FINISH);
	if (err != Z_STREAM_END) {
		deflateEnd(&zipStream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*pOutLen = zipStream.total_out;
	err = deflateEnd(&zipStream);
	return err;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param szDir : √Ë ˆ
 * \param szPackName : √Ë ˆ
 * \param bZip : √Ë ˆ
 * \param nEncrypt : √Ë ˆ
 * \param zipLevel : √Ë ˆ
 * \param szPassword : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool PackAggregate::MakePackDir(LPCSTR szDir,LPCSTR szPackName,bool bZip,int nEncrypt,int zipLevel,const char* szPassword,std::vector<String> & fileFilter,tOutputFunc  pOutoutFunc,BOOL & bStop,
								bool (* fnOnMakeSubFile)(const char* fileName,DWORD dwCRC) )
{
	std::vector<FileInfo> aPack;
	std::vector<PackAggregate::stFileInfo> aFileInfoH;

    std::vector<BYTE> aBuf1;
    std::vector<BYTE> aBuf2;

	BYTE key1[8],key2[8],key3[8];
	if(nEncrypt)
	{
		if(szPassword == NULL) szPassword = GetDefaultPassword();
		PasswordToDesKey(szPassword,key1,key2,key3);
	}

	int nPack = 0;
	FileInfo* pPack = NULL;
	GetDirInfo(szDir,pPack,nPack,fileFilter);

	int oldNum = nPack;
	aPack.resize(nPack);
	aFileInfoH.resize(nPack);

	nPack = 0;
	pPack = &aPack[0];
	GetDirInfo(szDir,pPack,nPack,fileFilter);

	Assert(oldNum == nPack);

	if(oldNum != nPack) return FALSE;

	int filepos = 0;
	int dirNameLen;
	char* p1 = strrchr( szDir,'\\' );
	if(p1) dirNameLen = p1-szDir+1;
	else dirNameLen = strlen( szDir );

	FILE * f;
    char sTmp[MAX_PATH];
	char szTempPath[MAX_PATH];
	GetTempPath(MAX_PATH,szTempPath);
	char* pStr = szTempPath + strlen(szTempPath) - 1;
	if(*pStr == '\\') *pStr = 0;

	GetTempFileName(szTempPath,"Pack",0,sTmp);

    f = fopen(sTmp,"wb");
    if(!f) return FALSE;

	if(pOutoutFunc)
	{
		pOutoutFunc("ø™ º¥Ú∞¸ %s .......\r\n",szPackName);
	}

    DWORD totallen = 0;
	for ( int i=0; i<nPack; i++ )
	{
		if(bStop)
		{
			pOutoutFunc("”√ªßΩ· ¯¥Ú∞¸\r\n");
			fclose(f);
			DeleteFile(sTmp);
			return false;
		}

		FileInfo * st1 = &aPack[i];

		if(pOutoutFunc)
		{
			pOutoutFunc("’˝‘⁄¥Ú∞¸Œƒº˛ %s \r\n",st1->szName);
		}

		stFileSizeInfo fInfo;
		fInfo.version = c_dwPackVersion;
		fInfo.uncompressedSize = st1->size;
		fInfo.flags = 0;
        BYTE* pBuf;
        int filelen;
        if( bZip || (nEncrypt == 1) )
        {
			// º”√‹ªÚ—πÀıµƒ
			
			if(bZip)
				fInfo.flags |= PackAggregate::flagCompress;
			if(nEncrypt == 1) 
				fInfo.flags |= PackAggregate::flagEncrypt;

            DWORD len = (st1->size + 7)/8 * 8;
            aBuf1.resize(len);

            FILE* file = fopen(st1->szName,"rb");
            if(!file) return FALSE;

            fread(&aBuf1[0],st1->size,1,file);
            fclose(file);

			fInfo.dwCRC = calculateCRC(&aBuf1[0],st1->size);

            if(!bZip)
            {
                EncryptData(&aBuf1[0],len,key1,key2,key3);
                pBuf = &aBuf1[0];
                filelen = len;
				aFileInfoH[i].size = filelen;
            }
            else
            {
                int outlen = len * 5/4;
                aBuf2.resize(outlen);
                compress_data(&aBuf2[0],(DWORD*)&outlen,&aBuf1[0],st1->size,zipLevel);
                if(nEncrypt == 1)
                {
                    len = (outlen + 7)/8 * 8;
                    EncryptData(&aBuf2[0],len,key1,key2,key3);
                }
                else
                {
                    len = outlen;
				}
				filelen = len;
                pBuf = &aBuf2[0];
            }
        }
        else
        {
			// ≤ª—πÀı£¨≤ªº”√‹

            filelen = st1->size;
            aBuf1.resize(st1->size);

			FILE* file = fopen(st1->szName,"rb");
            if(!file) return FALSE;
            fread(&aBuf1[0],st1->size,1,file);
            fclose(file);

			pBuf = &aBuf1[0];
			fInfo.dwCRC = calculateCRC(pBuf,st1->size);
		}
		if(nEncrypt == 1)
			EncryptData((BYTE*)&fInfo,sizeof(fInfo),key1,key2,key3);

		PackAggregate::stFileInfo & infoH = aFileInfoH[i];
		infoH.fileOffset = totallen;
		infoH.size = filelen + sizeof(fInfo);
		infoH.interspaceSize = (filelen + sizeof(fInfo) + 3) & (~3);
		DWORD crc = calculateCRC( &fInfo, sizeof(fInfo) );
		infoH.dwCRC = calculateCRC( pBuf,filelen ,crc );

		if(fnOnMakeSubFile)
		{
			if(!fnOnMakeSubFile(st1->szName,aFileInfoH[i].dwCRC)){
				fclose(f);
				return FALSE;
			}
		}

#ifdef _DEBUG
		BYTE* szBuf = new BYTE[sizeof(fInfo) + filelen];
		memcpy(szBuf,&fInfo,sizeof(fInfo));
		memcpy(szBuf + sizeof(fInfo),pBuf,filelen);
		Assert(aFileInfoH[i].dwCRC == calculateCRC(szBuf,sizeof(fInfo) + filelen));
		delete [] szBuf;
#endif

		TRACE("%s crc:%x\n",st1->szName,aFileInfoH[i].dwCRC);

		fwrite(&fInfo,sizeof(fInfo),1,f);
		fwrite(pBuf,filelen,1,f);

		fseek( f , aFileInfoH[i].interspaceSize - aFileInfoH[i].size , SEEK_CUR );
		totallen +=  aFileInfoH[i].interspaceSize;

		RemoveDirName( st1->szName , dirNameLen );
		replaceBacklashPath(st1->szName);
		strcpy(aFileInfoH[i].szName,st1->szName);
		aFileInfoH[i].dwFileID = calculateCRC(st1->szName,strlen(st1->szName));
	}

    fclose(f);

	aBuf2.clear();
	aBuf1.clear();

	char s[MAX_PATH];
	strcpy(s,szPackName);
	if (s[0] == 0)
		sprintf(s,"%s.pak",szDir);

    f = fopen(s,"wb");
    if(!f) return FALSE;
    
	PackAggregate::stPackHeader hdr;
	strncpy((char*)&hdr.magic,c_szPackMagic,4);
	hdr.version = c_dwPackVersion;
    hdr.filenum = nPack;
	hdr.flag = 0;

	if(bZip) hdr.flag |= PackAggregate::flagCompress;
	if(nEncrypt == 1) hdr.flag |= PackAggregate::flagEncrypt;
	if(nEncrypt == 2) hdr.flag |= PackAggregate::flagEncryptHeaderOnly;

	size_t hdrlen ;

	aBuf1.resize(aFileInfoH.size() * sizeof(PackAggregate::stFileInfo));
	BYTE* p = &aBuf1[0];
	for(size_t i=0; i< aFileInfoH.size();++i)
	{
		memcpy(p,&aFileInfoH[i].st1,sizeof(stFileInfo1));
		p += sizeof(stFileInfo1);

		strcpy( (char*)p,aFileInfoH[i].szName );
		p += strlen( (char*)p ) + 1;
	}

	hdrlen = ((p - &aBuf1[0]) + 7) & (~7);

    if( nEncrypt )
        EncryptHeaderData( (BYTE*)&aBuf1[0],hdrlen,key1,key2,key3 );

    FILE* file = fopen( sTmp,"rb" );
    if(!file) return FALSE;

	hdr.headerSize = hdrlen;

	// 2009
	//hdr.filenum -= 10;
	//hdr.headerSize -= 10;

	fwrite(&hdr,sizeof(hdr),1,f);

    aBuf2.resize(8192);
    while( totallen )
    {
        DWORD readlen;
        if(8192 > totallen)
            readlen = totallen;
        else
            readlen = 8192;

        fread(&aBuf2[0],readlen,1,file);
        fwrite(&aBuf2[0],readlen,1,f);

        totallen -= readlen;
    }
    fclose(file);
    ::DeleteFile(sTmp);

    fwrite(&aBuf1[0],hdrlen,1,f);
	fclose(f);
    
	if(pOutoutFunc)
	{
		pOutoutFunc("ÕÍ≥…¥Ú∞¸ %s\r\n",szPackName);
	}

	return TRUE;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
const char* PackAggregate::GetDefaultPassword()
{
	static char pszPassword[] = "20210112taozhi";
	return pszPassword;
}