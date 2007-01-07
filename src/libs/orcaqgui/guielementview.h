/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_GUI_ELEMENT_VIEW_H
#define ORCA2_GUI_ELEMENT_VIEW_H

#include <QTableView>
#include <vector>

namespace orcaqgui {

class GuiElementView : public QTableView
{
    Q_OBJECT

public:
    GuiElementView( QWidget *parent = 0 );
    
    //
    // Functions called from guielementmodel. They could all be implemented there
    // because the model has a pointer to the view. Don't know what's better.
    //
    
    // Returns a vector of indeces with the currently displayed adapters which are also *selected*
    // If none are selected, it returns *all* indeces of the currently displayed adapters
    void selectedAdaptersInView( int numElements, std::vector<int> &indeces);
    
    // Show all the elements in the model
    void showAllElements( int numElements );
    
    // Show all elements in the model except the ones with the passed indeces
    void hideElements( int numElements, const std::vector<int>& indeces );
    

protected:
    // Reimplement function from QTableView
    virtual void contextMenuEvent( QContextMenuEvent* e );

private slots:
    void action0() { action(0); };
    void action1() { action(1); };
    void action2() { action(2); };
    void action3() { action(3); };
    void action4() { action(4); };
    void action5() { action(5); };
    void action6() { action(6); };
    void action7() { action(7); };
    void action8() { action(8); };
    void action9() { action(9); };
    
    void setCooFrameToCurrentGuiElement();
    void removeCurrentGuiElement();
    
    //void showSelectedInterface(const QModelIndex&);

private:
    void action( int index );
};

}

#endif
