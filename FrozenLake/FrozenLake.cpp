#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <gym.h>

using namespace std;

/*
https://gym.openai.com/envs/FrozenLake-v0
*/
class FrozenLake : gym::Environment
{
	enum class Actions
	{
		Up,
		Down,
		Left,
		Right
	};

public:
	FrozenLake()
		: m_board{
		"SFFF",
		"FHFH",
		"FFFH",
		"HFFG"
	}
		, m_agentX(0), m_agentY(0)
	{

	}

private:
	char m_board[4][4 + 1];
	int m_agentX, m_agentY;

public:
	gym::Result step(int action_)
	{
		Actions action = static_cast<Actions>(action_);
		if (action == Actions::Up)
		{
			return moveAgent(0, -1);
		}
		else if (action == Actions::Down)
		{
			return moveAgent(0, +1);
		}
		else if (action == Actions::Left)
		{
			return moveAgent(-1, 0);
		}
		else if (action == Actions::Right)
		{
			return moveAgent(+1, 0);
		}

		return { 0, true };
	}

	void* reset()
	{
		m_agentX = 0;
		m_agentY = 0;

		return m_board;
	}

	void render()
	{
		for (int y = 0; y<4; ++y)
		{
			for (int x = 0; x<4; ++x)
			{
				if (m_agentX == x && m_agentY == y)
				{
					cout.put('@');
				}
				else
				{
					cout.put(m_board[y][x]);
				}
			}

			cout << endl;
		}
	}

	int getAgentX() const
	{
		return m_agentX;
	}

	int getAgentY() const
	{
		return m_agentY;
	}

private:
	gym::Result moveAgent(int x, int y)
	{
		gym::Result result;
		result.reward = 0;
		result.done = false;


		m_agentX += x;
		m_agentY += y;

		if (m_agentX < 0)
		{
			m_agentX = 0;
			result.reward = -1;
		}
		else if (m_agentX >= 4)
		{
			m_agentX = 4 - 1;
			result.reward = -1;
		}

		if (m_agentY < 0)
		{
			m_agentY = 0;
			result.reward = -1;
		}
		else if (m_agentY >= 4)
		{
			m_agentY = 4 - 1;
			result.reward = -1;
		}


		if (m_board[m_agentY][m_agentX] == 'H')
		{
			result.reward = -1;
			result.done = true;
		}
		else if (m_board[m_agentY][m_agentX] == 'G')
		{
			result.reward = 10;
			result.done = true;
		}

		return result;
	}
};


class QLearner
{
public:
	QLearner()
		: m_scoreMap{ 0, }
	{

	}

private:
	// m_scoreMap[y][x][dir(up, down, left, right)] == (x, y)에서 dir로 이동했을 때의 점수
	int m_scoreMap[4][4][4];

public:
	void learn(FrozenLake& game, int totalEpochs)
	{
		for (int epoch = 1; epoch <= totalEpochs; ++epoch)
		{
			game.reset();

			for (int step = 1; step <= 100; ++step)
			{
				int actionIndex = rand() % 4;

				int oldX = game.getAgentX();
				int oldY = game.getAgentY();

				gym::Result result = game.step(actionIndex);

				int agentX = game.getAgentX();
				int agentY = game.getAgentY();

				int maxQ = *std::max_element(m_scoreMap[agentY][agentX], m_scoreMap[agentY][agentX] + 4);

				m_scoreMap[oldY][oldX][actionIndex] = result.reward + maxQ - 1;

				if (result.done)
				{
					break;
				}
			}
		}
	}

	void play(FrozenLake& game) const
	{
		const std::string actionName[] = {
			"Up", "Down", "Left", "Right"
		};

		game.reset();

		for (int step = 1; step <= 100; ++step)
		{
			int agentX = game.getAgentX();
			int agentY = game.getAgentY();

			int actionIndex = std::distance(m_scoreMap[agentY][agentX],
				std::max_element(m_scoreMap[agentY][agentX], m_scoreMap[agentY][agentX] + 4));

			cout << "Action " << actionName[actionIndex] << " Score " << m_scoreMap[agentY][agentX][actionIndex] << endl;

			gym::Result result = game.step(actionIndex);

			game.render();

			cout << "----" << endl;

			if (result.done)
			{
				break;
			}
		}

		cout << "End!" << endl;
	}

	void print() const
	{
		for (int y = 0; y<4; ++y)
		{
			for (int i = 0; i<3; ++i)
			{
				for (int x = 0; x<4; ++x)
				{
					if (i == 0)
					{
						cout << "    ";
						cout << setfill(' ') << setw(4) << m_scoreMap[y][x][0];
						cout << "    |";
					}
					else if (i == 1)
					{
						cout << setfill(' ') << setw(4) << m_scoreMap[y][x][2];
						cout << "    ";
						cout << setfill(' ') << setw(4) << m_scoreMap[y][x][3];
						cout << "|";
					}
					else
					{
						cout << "    ";
						cout << setfill(' ') << setw(4) << m_scoreMap[y][x][1];
						cout << "    |";
					}
				}

				cout << endl;
			}

			cout << std::string((4 * 3 + 1) * 4, '-') << endl;
		}
	}
};


int main()
{
	FrozenLake game;
	QLearner agent;

	agent.learn(game, 1000);

	agent.print();

	agent.play(game);


	return 0;
}
