package edu.brown.aurora.gui;

/**
 * An exception that is thrown when the qos string is malformed.
 *
 * @author Robin Yan (rly@cs.brown.edu)
 */

public class QosSpecifierException extends Throwable
{
    public QosSpecifierException(String exc)
    {
        super(exc);
    }
}
