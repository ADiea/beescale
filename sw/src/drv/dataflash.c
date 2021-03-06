//*****************************************************************************
//
//      COPYRIGHT (c) ATMEL Norway, 1996-2001
//
//      The copyright to the document(s) herein is the property of
//      ATMEL Norway, Norway.
//
//      The document(s) may be used  and/or copied only with the written
//      permission from ATMEL Norway or in accordance with the terms and
//      conditions stipulated in the agreement/contract under which the
//      document(s) have been supplied.
//
//*****************************************************************************
//
//  File........: DATAFLASH.C
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: All AVRs with built-in HW SPI
//
//  Description.: Functions to access the Atmel AT45Dxxx dataflash series
//                Supports 512Kbit - 64Mbit
//
//  Revisions...:
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20011017 - 1.00 - Beta release                                  -  RM
//  20011017 - 0.10 - Generated file                                -  RM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20040121          added compare and erase function              - M.Thomas
//
//*****************************************************************************

/* 
   remark mthomas: If you plan to use the dataflash functions in own code
   for (battery powered) devices: disable the "chip select" after accessing
   the Dataflash. The current draw with cs enabled is "very" high. You can
   simply use the macro DF_CS_inactive already defined by Atmel after every
   DF access.

   The coin-cell battery on the Butterfly is not a reliable power-source if data
   in the flash-array should be changed (write/erase).
   See the Dataflash datasheet for the current needed during write-accesses.
*/

// Includes
//mtA
//#include <INA90.H>
//#include "iom169.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
//mtE

#include "dataflash.h"

// Constants
//Look-up table for these sizes ->  512k, 1M, 2M, 4M, 8M, 16M, 32M, 64M
// mt flash unsigned char DF_pagebits[]  ={  9,  9,  9,  9,  9,  10,  10,  11};	    //index of internal page address bits
const uint8_t DF_pagebits[] PROGMEM ={  9,  9,  9,  9,  9,  10,  10,  11};	    //index of internal page address bits
//Look-up table for these sizes ->  512k, 1M,  2M,  4M,  8M, 16M, 32M, 64M
// mt flash unsigned int  DF_pagesize[]  ={264,264, 264, 264, 264, 528, 528,1056};	//index of pagesizes
const uint16_t DF_pagesize[] PROGMEM ={264,264, 264, 264, 264, 528, 528,1056};	//index of pagesizes


// Globals
unsigned char PageBits = 0;
unsigned int  PageSize = 0;
// Functions

unsigned int dfGetPageCount(void)
{
	return 2048;
}

unsigned int dfGetBlockCount(void)
{
	return 2048/8;
}

unsigned int dfGetPageSize(void)
{
	return PageSize;
}

/*****************************************************************************
*
*	Function name : DF_SPI_init
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Sets up the HW SPI in Master mode, Mode 3
*					Note -> Uses the SS line to control the DF CS-line.
*
******************************************************************************/
void DF_SPI_init (void)
{
	// mtA
	// PORTB |= (1<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);
	// DDRB |= (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);		//Set MOSI, SCK AND SS as outputs
	PORTB |= (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);
	DDRB |= (1<<DDB2) | (1<<DDB1) | (1<<DDB0);		//Set MOSI, SCK AND SS as outputs
	// mtE
	SPSR = (1<<SPI2X);                                      //SPI double speed settings
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL);	//Enable SPI in Master mode, mode 3, Fosc/4
// mt: the following line was already commented out in the original code
//	SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL) | (1<<SPR1) | (1<<SPR0);	//Enable SPI in Master mode, mode 3, Fosc/2
}

/*****************************************************************************
*
*	Function name : DF_SPI_RW
*
*	Returns :		Byte read from SPI data register (any value)
*
*	Parameters :	Byte to be written to SPI data register (any value)
*
*	Purpose :		Read and writes one byte from/to SPI master
*
******************************************************************************/
unsigned char DF_SPI_RW (unsigned char output)
{
	unsigned char input;
	
	SPDR = output;							//put byte 'output' in SPI data register
	while(!(SPSR & 0x80));					//wait for transfer complete, poll SPIF-flag
	input = SPDR;							//read value in SPI data reg.
	
	return input;							//return the byte clocked in from SPI slave
}


/*****************************************************************************
*
*	Function name : Read_DF_status
*
*	Returns :		One status byte. Consult Dataflash datasheet for further
*					decoding info
*
*	Parameters :	None
*
*	Purpose :		Status info concerning the Dataflash is busy or not.
*					Status info concerning compare between buffer and flash page
*					Status info concerning size of actual device
*
*   mt: the 'if' marked with 'mt 200401' is a possible optimisation
*   if only one type of Dataflash is used (like on the Butterfly).
*   If the uC controls different types of dataflash keep the PageBits
*   and PageSize decoding in this function to avoid problems.
******************************************************************************/
unsigned char Read_DF_status (void)
{
	unsigned char result,index_copy;
	
	DF_CS_inactive;							//make sure to toggle CS signal in order
	DF_CS_active;							//to reset dataflash command decoder
	result = DF_SPI_RW(StatusReg);			//send status register read op-code
	result = DF_SPI_RW(0x00);				//dummy write to get result
	
	index_copy = ((result & 0x38) >> 3);	//get the size info from status register
	// mtA
	/// if (!PageBits) { // mt 200401
		// PageBits   = DF_pagebits[index_copy];	//get number of internal page address bits from look-up table
		// PageSize   = DF_pagesize[index_copy];   //get the size of the page (in bytes)
		PageBits   = pgm_read_byte(&DF_pagebits[index_copy]);	//get number of internal page address bits from look-up table
		PageSize   = pgm_read_word(&DF_pagesize[index_copy]);   //get the size of the page (in bytes)
	/// }
	// mtE
	return result;							//return the read status register value
}


/*****************************************************************************
*
*	Function name : Page_To_Buffer
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of page to be transferred to buffer
*
*	Purpose :		Transfers a page from flash to dataflash SRAM buffer
*					
******************************************************************************/
void dfPageToBuffer (unsigned int PageAdr, unsigned char BufferNo)
{
	DF_CS_inactive;												//make sure to toggle CS signal in order
	DF_CS_active;												//to reset dataflash command decoder
	
	unsigned char opCode = FlashToBuf1Transfer;
	if(2 == BufferNo)
		opCode = FlashToBuf2Transfer;
	
	DF_SPI_RW(opCode);							//transfer to buffer 1 op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
	DF_SPI_RW(0x00);										//don't cares
	
	DF_CS_inactive;												//initiate the transfer
	DF_CS_active;
	
	while(!(Read_DF_status() & 0x80));							//monitor the status register, wait until busy-flag is high
}



/*****************************************************************************
*
*	Function name : Buffer_Read_Byte
*
*	Returns :		One read byte (any value)
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*
*	Purpose :		Reads one byte from one of the dataflash
*					internal SRAM buffers
*
******************************************************************************/
unsigned char Buffer_Read_Byte (unsigned char BufferNo, unsigned int IntPageAdr)
{
	unsigned char data;
	
	data='0'; // mt 
	
	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	
	unsigned char opCode = Buf1Read;
	if(2 == BufferNo)
		opCode = Buf2Read;
	
	DF_SPI_RW(opCode);					//buffer 1 read op-code
	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	DF_SPI_RW(0x00);						//don't cares
	data = DF_SPI_RW(0x00);					//read byte
	
	return data;								//return the read data byte
}



/*****************************************************************************
*
*	Function name : dfBufferReadStream
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be read
*					*BufferPtr	->	address of buffer to be used for read bytes
*
*	Purpose :		Reads one or more bytes from one of the dataflash
*					internal SRAM buffers, and puts read bytes into
*					buffer pointed to by *BufferPtr
*
******************************************************************************/
unsigned int dfBufferReadStream (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder

	unsigned char opCode = Buf1Read;
	if(2 == BufferNo)
		opCode = Buf2Read;
	
	DF_SPI_RW(opCode);					//buffer 1 read op-code
	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	DF_SPI_RW(0x00);						//don't cares
	for( i=0; i<No_of_bytes; i++)
	{
		*(BufferPtr) = DF_SPI_RW(0x00);		//read byte and put it in AVR buffer pointed to by *BufferPtr
		BufferPtr++;						//point to next element in AVR buffer
	}
	return i;
}
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..
//mtA 
// translation of the Norwegian comments (thanks to Eirik Tveiten):
// NB : Check that (IntAdr + No_of_bytes) < buffersize, if not there will be problems
//mtE


/*****************************************************************************
*
*	Function name : Buffer_Write_Enable
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to start writing from
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*					
*	Purpose :		Enables continous write functionality to one of the dataflash buffers
*					buffers. NOTE : User must ensure that CS goes high to terminate
*					this mode before accessing other dataflash functionalities 
*
******************************************************************************/
void dfBufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr)
{
	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	
	unsigned char opCode = Buf1Write;
	if(2 == BufferNo)
		opCode = Buf2Write;	
	
	DF_SPI_RW(opCode);					//buffer 1 write op-code
	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
}



/*****************************************************************************
*
*	Function name : dfBufferWriteByte
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to write byte to
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*					Data		->	Data byte to be written
*
*	Purpose :		Writes one byte to one of the dataflash
*					internal SRAM buffers
*
******************************************************************************/
void dfBufferWriteByte (unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
	
	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder

	unsigned char opCode = Buf1Write;
	if(2 == BufferNo)
		opCode = Buf2Write;		

	DF_SPI_RW(opCode);					//buffer 1 write op-code
	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	DF_SPI_RW(Data);						//write data byte
}


/*****************************************************************************
*
*	Function name : Buffer_Write_Str
*
*	Returns :		Number of bytes written
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be written
*					*BufferPtr	->	address of buffer to be used for copy of bytes
*									from AVR buffer to dataflash buffer 1 (or 2)
*
*	Purpose :		Copies one or more bytes to one of the dataflash
*					internal SRAM buffers from AVR SRAM buffer
*					pointed to by *BufferPtr
*
******************************************************************************/
unsigned int dfBufferWriteStream (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder

	unsigned char opCode = Buf1Write;
	if(2 == BufferNo)
		opCode = Buf2Write;
	
	DF_SPI_RW(opCode);					//buffer 1 write op-code
	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	for( i=0; i<No_of_bytes; i++)
	{
		DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to dataflash buffer 1 location
		BufferPtr++;						//point to next element in AVR buffer
	}
	
	return i;
}
//NB : Monitorer busy-flag i status-reg.
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..
//mtA 
// translation of the Norwegian comments (thanks to Eirik Tveiten):
// NB : Monitors busy-flag in status-reg
// NB : Check that (IntAdr + No_of_bytes) < buffersize, if not there will be problems
//mtE

/*****************************************************************************
*
*	Function name : dfBufferToPage
*
*	Returns :		None
*
*	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of flash page to be programmed
*
*	Purpose :		Transfers a page from dataflash SRAM buffer to flash
*					
******************************************************************************/
void dfBufferToPage (unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_inactive;												//make sure to toggle CS signal in order
	DF_CS_active;												//to reset dataflash command decoder
	
	unsigned char opCode = Buf1ToFlashWE;
	if(2 == BufferNo)
		opCode = Buf2ToFlashWE;
	
	DF_SPI_RW(opCode);								//buffer 1 to flash with erase op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
	DF_SPI_RW(0x00);										//don't cares

	DF_CS_inactive;												//initiate flash page programming
	DF_CS_active;												
	
	while(!(Read_DF_status() & 0x80));							//monitor the status register, wait until busy-flag is high
}


/*****************************************************************************
*
*	Function name : dfContFlashReadEnable
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
*					IntPageAdr	->	Internal page address where cont.read starts from
*
*	Purpose :		Initiates a continuous read from a location in the DataFlash
*
******************************************************************************/
void dfContFlashReadEnable (unsigned int PageAdr, unsigned int IntPageAdr)
{
	DF_CS_inactive;																//make sure to toggle CS signal in order
	DF_CS_active;																//to reset dataflash command decoder
	
	DF_SPI_RW(ContArrayRead);													//Continuous Array Read op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));						//upper part of page address
	DF_SPI_RW((unsigned char)((PageAdr << (PageBits - 8))+ (IntPageAdr>>8)));	//lower part of page address and MSB of int.page adr.
	DF_SPI_RW((unsigned char)(IntPageAdr));										//LSB byte of internal page address
	DF_SPI_RW(0x00);															//perform 4 dummy writes
	DF_SPI_RW(0x00);															//in order to intiate DataFlash
	DF_SPI_RW(0x00);															//address pointers
	DF_SPI_RW(0x00);
}

/*****************************************************************************
*
*	Function name : dfPageBufferCompare
*
*	Returns :		0 match, 1 if mismatch
*
*	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of flash page to be compared with buffer
*
*	Purpose :		comparte Buffer with Flash-Page
*
*   added by Martin Thomas, Kaiserslautern, Germany. This routine was not 
*   included by ATMEL
*					
******************************************************************************/
unsigned char dfPageBufferCompare(unsigned char BufferNo, unsigned int PageAdr)
{
	unsigned char stat;
	
	DF_CS_inactive;					//make sure to toggle CS signal in order
	DF_CS_active;					//to reset dataflash command decoder
	
	unsigned char opCode = FlashToBuf1Compare;
	if(2 == BufferNo)
		opCode = FlashToBuf2Compare;

	DF_SPI_RW(opCode);	
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address and MSB of int.page adr.
	DF_SPI_RW(0x00);	// "dont cares"
	
	DF_CS_inactive;												
	DF_CS_active;		
	
	do {
		stat=Read_DF_status();
	} while(!(stat & 0x80));							//monitor the status register, wait until busy-flag is high
	
	return (stat & 0x40);
}

/*****************************************************************************
*
*	Function name : Page_Erase
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page to be erased
*
*	Purpose :		Sets all bits in the given page (all bytes are 0xff)
*
*	function added by mthomas. 
*
******************************************************************************/
void Page_Erase (unsigned int PageAdr)
{
	DF_CS_inactive;																//make sure to toggle CS signal in order
	DF_CS_active;																//to reset dataflash command decoder

	DF_SPI_RW(PageErase);										//Page erase op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address and MSB of int.page adr.
	DF_SPI_RW(0x00);	// "dont cares"

	DF_CS_inactive;												//initiate flash page erase
	DF_CS_active;

	while(!(Read_DF_status() & 0x80));							//monitor the status register, wait until busy-flag is high
}

/*****************************************************************************
*
*	Function name : dfBlockErase
*
*	Returns :		None
*
*	Parameters :	BlockAdr		->	Address of flash block to be erased
*
*	Purpose :		Sets all bits in the given block (all bytes are 0xff)
*
******************************************************************************/
void dfBlockErase (unsigned int BlockAdr)
{
	DF_CS_inactive;																//make sure to toggle CS signal in order
	DF_CS_active;																//to reset dataflash command decoder

	DF_SPI_RW(FlashBlockErase);										//Page erase op-code
	DF_SPI_RW((unsigned char)(BlockAdr >> 4));	//upper part of block address
	DF_SPI_RW((unsigned char)(BlockAdr << 4));	//lower part of block address 
	DF_SPI_RW(0x00);	// "dont cares"

	DF_CS_inactive;												//initiate flash block erase
	DF_CS_active;

	while(!(Read_DF_status() & 0x80));							//monitor the status register, wait until busy-flag is high
}


// *****************************[ End Of DATAFLASH.C ]*************************
