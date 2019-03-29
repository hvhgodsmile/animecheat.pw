#include "cheats/lagcompensation/lagcompensation.h"

struct c_resolver_info {
public:
	c_resolver_info() {
		state = nullptr;
		simtime = 0.f;
		last_balance_adjust_time = 0.f;
		balance_adjust = false;
		balance_adjust_playing = false;
		source.Init();
		m_angles.Init();
		velocity.Init();
		velocity_direction.Init();
	}
	c_resolver_info(const c_resolver_info& src) {
		last_balance_adjust_time = src.last_balance_adjust_time;
		balance_adjust = src.balance_adjust;
		balance_adjust_playing = src.balance_adjust_playing;
		state = src.state;
		simtime = src.simtime;
		source = src.source;
		m_angles = src.m_angles;
		velocity = src.velocity;
		velocity_direction = src.velocity_direction;

		poses = src.poses;

	}
public:
	std::vector< vec3_t > eye_positions;
	vec3_t source, m_angles;
	vec3_t velocity, velocity_direction;
	float simtime;
	std::array< float, 24 >* poses;
	c_baseplayeranimationstate* state;
	bool balance_adjust;
	bool balance_adjust_playing;
	float last_balance_adjust_time;
};

class c_resolver : public singleton< c_resolver > {
private:
	c_resolver_info current_record[65]; // current record, to compare with last record
	std::deque< c_resolver_info > last_record[65]; // the last record, for legit checks etc.
public:
	void execute();
	vec3_t local_pos;

	void remap_value(float val, float a, float b, float c, float d) {

		if (a == b)
			val = val >= b ? d : c;
		else
			val = c + (d - c) * (val - a) / (b - a);

	};

	void update_record(c_resolver_info info, player_t* entity); // updates your record information
	void create_move(); // for autowall checks
	void animation_fix(player_t* entity); // corrects the player animations so they are in sync with the server.

	void resolve(c_resolver_info record1, c_resolver_info record2, player_t* entity);

	bool can_autowall(player_t* entity, vec3_t src, int accuracy); // checks if anti freestanding is a viable option
	bool should_update_record(c_resolver_info current, c_resolver_info previous); // saves you from updating the record too often
	bool can_extrapolate(bool can_autowall, player_t* entity, int extension, int accuracy_boost); // extrapolates the local position for a more accurate trace to the enemy.
	bool is_slow_walking(c_resolver_info current, c_resolver_info previous, float accuracy);
	void compensate_for_slow_walk(player_t* entity, vec3_t velocity);
	void find_viable_record(c_resolver_info record1, c_resolver_info record2); // selects the best record
	vec3_t extrapolate_direction(vec3_t src, vec3_t dst, player_t* entity); // gets the direction to extrapolate the local player.
};