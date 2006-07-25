/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "section.h"


Section::Section()
{
	//next = NULL;
	isNextCon = false;

	isALine = false;
	A = 0.0;
	B = 0.0;
	C = 0.0;

	elements.clear();
}


Section::~Section()
{
	// Nothing to delete
}


/*
LaserReturn *
Section::getFirstElement()
{
	return elements[0];
}



LaserReturn *
Section::getLastElement()
{
	if (elements.size() == 0)
		return NULL;
	return elements[elements.size()-1];
}
*/
