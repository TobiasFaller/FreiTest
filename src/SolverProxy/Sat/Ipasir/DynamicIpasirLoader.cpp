#include "DynamicIpasirLoader.hpp"

#ifdef HAS_SAT_SOLVER_IPASIR_DYNAMIC

#include <dlfcn.h>
#include <type_traits>

#include "Basic/Logging.hpp"

namespace SolverProxy
{
namespace Sat
{

// Creates a new empty dynamic loader instance
DynamicIpasirLoader::DynamicIpasirLoader(void):
	_libraryHandle(nullptr),
	_solver(nullptr),
	_functionHandles()
{
}

//  Closes the currently open library, if it exits
DynamicIpasirLoader::~DynamicIpasirLoader(void)
{
	SolverRelease();
	CloseLibrary();
}

// Creates new instance, with loaded dynamic library from given path and initialized solver.
std::unique_ptr<DynamicIpasirLoader> DynamicIpasirLoader::CreateInstance(std::string path)
{
	auto instance = std::unique_ptr<DynamicIpasirLoader>(new DynamicIpasirLoader());
	if (!instance->LoadLibrary(path))
	{
		return {};
	}

	instance->SolverInit();
	return instance;
}

// Closes the currently loaded dynamic library via its handle (if existing)
void DynamicIpasirLoader::CloseLibrary(void)
{
	if (_libraryHandle)
	{
		if (dlclose(_libraryHandle) != 0)
		{
			LogError();
		}

		_libraryHandle = nullptr;
	}
}

const char* DynamicIpasirLoader::IpasirSignature(void) const
{
	return _functionHandles.ipasir_signature();
}

void DynamicIpasirLoader::SolverInit(void)
{
	if (!_solver)
	{
		_solver = _functionHandles.ipasir_init();
	}
}

void DynamicIpasirLoader::SolverRelease(void)
{
	if (_solver)
	{
		_functionHandles.ipasir_release(_solver);
		_solver = nullptr;
	}
}

void DynamicIpasirLoader::SolverReset(void)
{
	SolverRelease();
	SolverInit();
}

void DynamicIpasirLoader::IpasirReset(void)
{
	SolverReset();
}

void DynamicIpasirLoader::IpasirAdd(int32_t lit_or_zero) const
{
	_functionHandles.ipasir_add(_solver, lit_or_zero);
}

void DynamicIpasirLoader::IpasirAssume(int32_t lit) const
{
	_functionHandles.ipasir_assume(_solver, lit);
}

int DynamicIpasirLoader::IpasirSolve(void) const
{
	return _functionHandles.ipasir_solve(_solver);
}

int32_t DynamicIpasirLoader::IpasirVal(int32_t lit) const
{
	return _functionHandles.ipasir_val(_solver, lit);
}

int DynamicIpasirLoader::IpasirFailed(int32_t lit) const
{
	return _functionHandles.ipasir_failed(_solver, lit);
}

void DynamicIpasirLoader::IpasirSetTerminate(void* data, int (*terminate)(void* data)) const
{
	_functionHandles.ipasir_set_terminate(_solver, data, terminate);
}

void DynamicIpasirLoader::IpasirSetLearn(void* data, int max_length, void (*learn)(void* data, int32_t* clause)) const
{
	_functionHandles.ipasir_set_learn(_solver, data, max_length, learn);
}

// Log the newest occured error.
void DynamicIpasirLoader::LogError(void)
{
	LOG(ERROR) << dlerror();
}

// Tries to open given dynamic library.
// Logs error if loading failed.
bool DynamicIpasirLoader::LoadLibrary(std::string libraryPath)
{
	_libraryHandle = dlopen(libraryPath.c_str(), RTLD_NOW);
	if (_libraryHandle == nullptr)
	{
		LOG(ERROR) << "Could not load library " << libraryPath;
		LogError();
		return false;
	}

	return LoadSymbols();
}

// Loads all ipasir functions from shared library.
// Function handles are stored in struct.
bool DynamicIpasirLoader::LoadSymbols(void)
{
	DVLOG(3) << "Loading Symbols";
	const auto load = [&](auto& target, std::string name) -> auto {
		return target = LoadSymbol<std::remove_reference_t<decltype(target)>>(name);
	};

	if (!load(_functionHandles.ipasir_signature, "ipasir_signature")) return false;
	if (!load(_functionHandles.ipasir_init, "ipasir_init")) return false;
	if (!load(_functionHandles.ipasir_release, "ipasir_release")) return false;
	if (!load(_functionHandles.ipasir_add, "ipasir_add")) return false;
	if (!load(_functionHandles.ipasir_assume, "ipasir_assume")) return false;
	if (!load(_functionHandles.ipasir_solve, "ipasir_solve")) return false;
	if (!load(_functionHandles.ipasir_val, "ipasir_val")) return false;
	if (!load(_functionHandles.ipasir_failed, "ipasir_failed")) return false;
	if (!load(_functionHandles.ipasir_set_terminate, "ipasir_set_terminate")) return false;
	if (!load(_functionHandles.ipasir_set_learn, "ipasir_set_learn")) return false;

	return true;
}

// Generice template for symbol loading from shared library.
template<typename T>
T DynamicIpasirLoader::LoadSymbol(std::string name)
{
	auto functionHandle = dlsym(_libraryHandle, name.c_str());
	if (functionHandle == nullptr)
	{
		LOG(ERROR) << "Could not load function "  << name;
		LogError();
		return nullptr;
	}

	return reinterpret_cast<T>(functionHandle);
}

};
};

#endif
