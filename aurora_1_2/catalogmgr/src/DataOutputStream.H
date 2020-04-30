/*
 * @(#)DataOutputStream.h	1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __DataOutputStream_H
#define __DataOutputStream_H

#include <string>
#include <iostream>

using namespace std;

/**
 * A data output stream lets an application write primitive c data types
 * to an output stream in a portable way. An application can then use
 * a data input stream to read the data back in.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class DataOutputStream: public ostream
{
  // Methods
  public:
    /**
     * Creates a new data output stream to write data to the specified
     * underlying output stream buffer.
     * @param psb the underlying output stream.
     */
    DataOutputStream(streambuf* psb);

    /**
     * Writes a boolean to the underlying output stream as a 1-byte value.
     * The value true is written out as the value (byte)1;
     * the value false is written out as the value (byte)0.
     * @param v a boolean value to be written.
     */
    void writeBoolean(bool v);

    /**
     * Writes a char to the underlying output stream as a 2-byte value,
     * high byte first.
     * @param v a char value to be written.
     */
    void writeChar(char v);

    /**
     * Writes a short to the underlying output stream as two bytes,
     * high byte first.
     * @param v a short value to be written.
     */
    void writeShort(short v);

    /**
     * Writes an int to the underlying output stream as four bytes,
     * high byte first.
     * @param v a int value to be written.
     */
    void writeInt(int v);

    /**
     * Writes a float to the underlying output stream as four bytes,
     * high byte first.
     * @param v a float value to be written.
     */
    void writeFloat(float v);

    /**
     * Writes a double to the underlying output stream as eight bytes,
     * high byte first.
     * @param v a double value to be written.
     */
    void writeDouble(double v);

    /**
     * Writes a string to the underlying output stream using UTF-8
     * encoding in a machine-independent manner.
     * First, two bytes are written to the output stream as if
     * by the writeShort method giving the number of bytes to follow.
     * This value is the number of bytes actually written out,
     * not the length of the string.
     * Following the length, each character of the string is output,
     * in sequence, using the UTF-8 encoding for the character.
     * This method is not yet 100% compatible with the Java counterpart especially for very long strings.
     * @param v a string value to be written.
     */
    void writeUTF(string v);
    /**
     * Returns the current value of the counter written,
     * the number of bytes written to this data output stream so far.
     * @return the number of bytes written to this data output stream so far.
     */
    long size();
};

#endif // __DataOutputStream_H
