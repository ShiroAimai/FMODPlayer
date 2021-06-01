#pragma once

namespace NCWrapper {
	struct FMODWrapperResult
	{
		enum class FMODWrapperResultCode {
			OK,
			ERROR
		};
		static FMODWrapperResult From(FMOD_RESULT result);

		FMODWrapperResultCode code = FMODWrapperResultCode::OK;
		std::string msg;

		void Update(FMOD_RESULT result);
		bool IsValid() const;
	};

}