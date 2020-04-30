/*
 * @(#)DataInputStream.h	1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __DataInputStream_H
#define __DataInputStream_H

#include <string>
#include <iostream>

using namespace std;

/**
 * A data input stream lets an application read primitive C data types from an underlying
 * input stream in a machine-independent way.
 * An application uses a data output stream to write data that can later be
 * read by a data input stream.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class DataInputStream: public istream
{
  // Methods
  public:
    /**
     * Creates a new data input stream to read data from the specified
     * underlying input stream buffer.
     * @param psb the underlying input stream.
     */
    DataInputStream(streambuf* psb);

    /**
     * Reads one input byte and returns true if that byte is nonzero,
     * false if that byte is zero.
     * This method is suitable for reading the byte written by the writeBoolean method of class DataOutputStream.
     * @return the boolean read.
     */
    bool readBoolean();

    /**
     * Reads an input char and returns the char value.
     * As A Unicode char in Java is made up of two bytes, this method reads two bytes from the
     * underlying input stream.
     * This method is suitable for reading bytes written by the writeChar method of class DataOutputStream.
     * @return the char read.
     */
    char readChar();

    /**
     * Reads two input bytes and returns a short value.
     * This method is suitable for reading the bytes written by the writeShort method of class DataOutputStream.
     * @return the short read.
     */
    short readShort();

    /**
     * Reads four input bytes and returns an int value.
     * This method is suitable for reading bytes written by the writeInt method of class DataOutputStream.
     * @return the int read.
     */
    int readInt();

    /**
     * Reads four input bytes and returns a float value.
     * This method is suitable for reading bytes written by the writeFloat method of class DataOutputStream.
     * @return the float read.
     */
    float readFloat();

    /**
     * Reads eight input bytes and returns a double value.
     * This method is suitable for reading bytes written by the writeDouble method of class DataOutputStream.
     * @return the double read.
     */
    double readDouble();

    /**
     * Reads in a string that has been encoded using the writeUTF method of class DataOutputStream.
     * First, two bytes are read and used to construct an unsigned 16-bit integer in exactly
     * the manner of the readShort method.
     * This integer value is called the UTF length and specifies the number of additional bytes to be read.
     * This method is not yet 100% compatible with the Java counterpart especially for very long strings.
     * @return the string read.
     */
    string readUTF();
};

#endif // __DataInputStream_H
