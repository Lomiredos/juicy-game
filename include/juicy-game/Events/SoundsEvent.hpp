#pragma once


namespace ee::event {


	struct LaserSoundEvent {};
	struct ExplosionSoundEvent {};
	struct AlertSoundEvent {};
	struct MoteurSoundEvent {
		float volume;
		float pitch;
	};

}