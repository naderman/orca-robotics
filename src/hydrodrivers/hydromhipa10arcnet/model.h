/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_MHI_PA10_MODEL_H
#define HYDRO_MHI_PA10_MODEL_H

// Comment the model number which matches your PA10
#define PA10_6CE
//#define PA10_7CE

#if defined( PA10_6CE )

#define FOREACH_OPERATIONAL_AXIS(index)					\
	for (unsigned int index = 0; index < 7; ++index)		\
	{											\
		if (index != 2)							\
		{
#define ENDEACH_OPERATIONAL_AXIS						\
		}										\
	}

#endif


#if defined( PA10_7CE )

#define FOREACH_OPERATIONAL_AXIS(index)					\
	for (unsigned int index = 0; index < 7; ++index)		\
	{
#define FOREACH_OPERATIONAL_AXIS						\
	}
						
#endif

#endif
