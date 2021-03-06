#include <node.h>
#include "StdAfx.h"
#include "VCAIOUSB64.h"
#include <math.h>
#include <ctype.h>

using namespace v8;

//? Takes two arguments: channel number, step direction (+1 for incriment, -1 for decriment)

void stepCode(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    int channel = args[0].As<Number>()->Value();
    int step = args[1].As<Number>()->Value(); // +1 for incriment, -1 for decriment

    //* C++ starts here

    unsigned long com_code = 0x00000000, addr = 0x00000000, offset = 0x00000000, com_word = 0x00000000, offset1 = 0x00000000, offset2 = 0x00000000;
    // Assume that we are only using one ACCES I/O device in the USB chain.
    unsigned long deviceIndex = diOnly;

    // Set resistor register addresses and IO bank offsets.
    int ch = channel - 1; // 0 based indexing for easier math
    unsigned long addrHexCodes[4] = {0x01, 0x07, 0x06, 0x00};

    addr = addrHexCodes[ch % 4];
    offset = floor(ch / 4) * 8; // channel 1-4: 0 = Bank A, 5-8: 8 = Bank B, 9-12: 16 = Bank C
    offset2 = floor(ch / 4);

    com_code = step > 0 ? 0x01 : 0x02; // 0x01 for incriment, 0x02 for decriment

    // Set up the 8-bit command word
    com_word = com_word | (com_code << 2);
    com_word = com_word | (addr << 4);

    // Write serial data to the DIO ports:
    // Port assignments:  x0=HV_EN, x1=/CS, x2=SCLK, and x3=SDI
    // Here 'x' is {A,B, or C}.
    DIO_Write1(deviceIndex, (3 + offset), false); //Set SDI to '0'
    DIO_Write1(deviceIndex, (2 + offset), true);  //Set SCLK to '1'
    DIO_Write1(deviceIndex, (1 + offset), true);  //Set /CS to '1'
    DIO_Write1(deviceIndex, (0 + offset), true);  //Set HV_EN to '1'

    // Set /CS to '0' to begin write command cycle:
    DIO_Write1(deviceIndex, (1 + offset), false);

    for (int i = 0; i < 8; i++)
    {
        // Set SCLK to '0'
        DIO_Write1(deviceIndex, (2 + offset), false);
        if (com_word & (1 << (7 - i)))
        {
            // Set SDI to '1'
            DIO_Write1(deviceIndex, (3 + offset), true);
        }
        else
        {
            // Set SDI to '0'
            DIO_Write1(deviceIndex, (3 + offset), false); //Set SDI to '0'
        }
        // Set SCLK to '1'
        DIO_Write1(deviceIndex, (2 + offset), true); // Set SCLK to '1', Read SDO data back
    }
    // Reset SCLK and /CS
    DIO_Write1(deviceIndex, (2 + offset), true); //Set SCLK to '1'
    DIO_Write1(deviceIndex, (1 + offset), true); //Set /CS to '1'

    //* C++ ends here
}

void init(Local<Object> exports, Local<Object> method)
{
    NODE_SET_METHOD(method, "exports", stepCode);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init);