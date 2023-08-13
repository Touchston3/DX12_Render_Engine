#include "Root_Signature.hpp"

Root_Signature::Root_Signature(const std::vector<Root_Signature::Parameter>& constants, const std::vector<Root_Signature::Parameter>& samplers) :
	m_description(),
	m_constants(constants),
	m_samplers(samplers)
{
	
}

