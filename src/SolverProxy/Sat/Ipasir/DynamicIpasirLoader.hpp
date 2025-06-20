#pragma once

#ifdef HAS_SAT_SOLVER_IPASIR_DYNAMIC

#include <string>
#include <memory>

namespace SolverProxy
{
namespace Sat
{

class DynamicIpasirLoader
{
public:
	virtual ~DynamicIpasirLoader(void);

	static std::unique_ptr<DynamicIpasirLoader> CreateInstance(std::string path);

	void IpasirReset(void);
	const char* IpasirSignature(void) const;
	void IpasirAdd(int32_t lit_or_zero) const;
	void IpasirAssume(int32_t lit) const;
	int IpasirSolve(void) const;
	int32_t IpasirVal(int32_t lit) const;
	int IpasirFailed(int32_t lit) const;
	void IpasirSetTerminate(void* data, int (*terminate)(void* data)) const;
	void IpasirSetLearn(void* data, int max_length, void (*learn)(void* data, int32_t* clause)) const;

private:
	DynamicIpasirLoader(void);

	void SolverInit(void);
	void SolverRelease(void);
	void SolverReset(void);

	void LogError(void);
	bool LoadLibrary(std::string path);
	bool LoadSymbols(void);
	void CloseLibrary(void);

	template<typename T>
	T LoadSymbol(std::string name);

	// See documentation at https://github.com/biotomas/ipasir/blob/master/ipasir.h
	struct FunctionHandles
	{
		FunctionHandles(void):
			ipasir_signature(nullptr),
			ipasir_init(nullptr),
			ipasir_release(nullptr),
			ipasir_add(nullptr),
			ipasir_assume(nullptr),
			ipasir_solve(nullptr),
			ipasir_val(nullptr),
			ipasir_failed(nullptr),
			ipasir_set_terminate(nullptr),
			ipasir_set_learn(nullptr)
		{
		}

		const char* (*ipasir_signature)(void);
		void* (*ipasir_init)(void);
		void (*ipasir_release)(void* solver);
		void (*ipasir_add)(void* solver, int32_t lit_or_zero);
		void (*ipasir_assume)(void* solver, int32_t lit);
		int (*ipasir_solve)(void* solver);
		int32_t (*ipasir_val)(void* solver, int32_t lit);
		int (*ipasir_failed)(void* solver, int32_t lit);
		void (*ipasir_set_terminate)(void* solver, void* data, int (*terminate)(void* data));
		void (*ipasir_set_learn)(void* solver, void* data, int max_length, void (*learn)(void* data, int32_t* clause));
	};

	void* _libraryHandle;
	void* _solver;
	FunctionHandles _functionHandles;

};

};
};

#endif
