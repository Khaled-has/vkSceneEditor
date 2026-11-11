#include "DxCompiler.h"
#include <log.h>

#ifdef DXC_INCLUDED_H

void DxCompiler::Create()
{
	HRESULT Hres;

	Hres = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_cUtils));
	if (FAILED(Hres))
	{
		LOG_ERROR("DxcCompiler error: Could'nt create the IDxcUtils");
		exit(1);
	}

	Hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_cCompiler));
	if (FAILED(Hres))
	{
		LOG_ERROR("DxcCompiler error: Could'nt create the IDxcCompiler3");
		exit(1);
	}
}

void DxCompiler::Destroy()
{
	
}

#endif