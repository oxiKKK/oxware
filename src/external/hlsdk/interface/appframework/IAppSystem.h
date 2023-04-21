//======== (C) Copyright 1999, 2000 Valve, L.L.C. All rights reserved. ========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: An application framework 
//
// $Revision: $
// $NoKeywords: $
//=============================================================================
#ifndef IAPPSYSTEM_H
#define IAPPSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Client systems are singleton objects in the client codebase responsible for
// various tasks
// The order in which the client systems appear in this list are the
// order in which they are initialized and updated. They are shut down in
// reverse order from which they are initialized.
//-----------------------------------------------------------------------------

class IAppSystem
{
public:
	// Non-existent in GoldSrc
};

#endif // IAPPSYSTEM_H