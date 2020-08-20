#pragma once

#include <deque>
#include <vector>
#include <functional>
namespace Ares
{

	class EditorUtility
	{
		static const uint32_t MAX_UNDOS = 25;

		typedef std::pair<std::function<void()>, std::function<void()>> UndoRedoPair;
	public:
		inline static bool CanUndo()
		{
			return (bool)s_UndoStack.size();
		}
		inline static bool CanRedo()
		{
			return (bool)s_RedoStack.size();
		}
		
		inline static void AddToUndoStack(UndoRedoPair undoRedoPair)
		{
			if (s_UndoStack.size() >= MAX_UNDOS)
			{
				s_UndoStack.pop_front();
			}
			s_UndoStack.push_back(undoRedoPair);
			s_RedoStack.clear();
		}
		inline static void TriggerUndo()
		{
			if (s_UndoStack.size())
			{
				UndoRedoPair undoRedoPair = s_UndoStack.back();
				undoRedoPair.first();
				s_UndoStack.pop_back();

				s_RedoStack.push_back(undoRedoPair);
			}
		}
		inline static void TriggerRedo()
		{
			if (s_RedoStack.size())
			{
				UndoRedoPair undoRedoPair = s_RedoStack.back();
				undoRedoPair.second();
				s_RedoStack.pop_back();
				s_UndoStack.push_back(undoRedoPair);
			}
		}
				
	private:
		inline static std::deque<UndoRedoPair> s_UndoStack;
		inline static std::vector<UndoRedoPair> s_RedoStack;
	};

}