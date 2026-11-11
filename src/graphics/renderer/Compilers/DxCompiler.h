#ifndef DXCOMPILER_H
#define DXCOMPILER_H

#if __has_include(<dxcapi.h>)
#define DXC_INCLUDED_H
#ifdef WIN32
#include <Windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#endif

#include <dxcapi.h>

#elif __has_include(<include/dxcapi.h>)
#define DXC_INCLUDED_H
#ifdef WIN32
#include <Windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#endif

#include <include/dxcapi.h>
#endif

#ifdef DXC_INCLUDED_H

class DxCompiler
{
public:
	DxCompiler() {}

	void Create();
	void Destroy();

	inline const ComPtr<IDxcUtils>& GetUtils()			const { return m_cUtils; }
	inline const ComPtr<IDxcCompiler3> GetCompiler()	const { return m_cCompiler; }

private:
	ComPtr<IDxcUtils> m_cUtils;
	ComPtr<IDxcCompiler3> m_cCompiler;
};

#endif
#endif