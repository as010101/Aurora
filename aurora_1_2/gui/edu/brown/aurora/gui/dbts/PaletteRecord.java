package edu.brown.aurora.gui.dbts;

/**
 * Specifies all of the classes that may be indirectly added to the PalettePanel, but
 * not directly added.  For example, arcs within a superbox that was saved to the
 * PalettePanel.
 *
 * @author Robin Yan (rly@cs.brown.edu)
 */
public interface PaletteRecord
{
    /**
     *  Writes the record into the out stream.
     */
    public void setRecord();

    /**
     * Gets the root box of this palette record object.
     */
    public int getRootBoxId();

    /**
     * Gets the id of this palette record object.
     */
    public int getId();

    /**
     * Gets the id of the parent of this palette record object.
     */
    public int getParentId();

}
