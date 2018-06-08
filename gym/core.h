#pragma once

namespace gym
{
	class Result
	{
	public:
		void* observation;
		int reward;
		bool done;
		void* info;
	};

	class Environment
	{
	public:
		virtual Result step(int action) = 0;
		virtual void* reset() = 0;
		virtual void render() = 0;
		virtual void close();
		virtual void seed();
	};
}
