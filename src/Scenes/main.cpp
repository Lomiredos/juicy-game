
#include "juicy-game/Engine.hpp"
#include "juicy-game/Managers/SoundsManager.hpp"
int main()
{
	Engine engine;

	engine.Start("Juicy-game", 800, 600, 60);

	engine.Run();

	engine.Quit();


	return 0;
}
