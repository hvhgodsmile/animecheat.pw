#pragma once 

#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientUnknown.hpp"
#include "IClientThinkable.hpp"

struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	// Delete yourself.
	virtual void			Release( void ) = 0;
	inline const char* get_classname(void) {
		return ((const char*(__thiscall*)(IClientEntity*)) *(uintptr_t*)(*(uintptr_t*)this + 556))(this);
	}
	// Network origin + angles
	virtual const Vector&	GetAbsOrigin( void ) const = 0;
	virtual const Vector&	GetAbsAngles( void ) const = 0;
};