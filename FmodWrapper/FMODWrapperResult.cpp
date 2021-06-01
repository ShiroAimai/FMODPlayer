#include "pch.h"
#include "FMODWrapperResult.h"

namespace NCWrapper {

	FMODWrapperResult FMODWrapperResult::From(FMOD_RESULT result)
	{
		FMODWrapperResult wrapperResult;
		wrapperResult.code = result == FMOD_OK
			? FMODWrapperResult::FMODWrapperResultCode::OK
			: FMODWrapperResult::FMODWrapperResultCode::ERROR;
		wrapperResult.msg = FMOD_ErrorString(result);

		return wrapperResult;
	}

	void FMODWrapperResult::Update(FMOD_RESULT result)
	{
		code = result == FMOD_OK
			? FMODWrapperResult::FMODWrapperResultCode::OK
			: FMODWrapperResult::FMODWrapperResultCode::ERROR;
		msg = FMOD_ErrorString(result);
	}

	bool FMODWrapperResult::IsValid() const
	{
		return code == FMODWrapperResultCode::OK;
	}
}