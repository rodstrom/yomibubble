#ifndef _PLAYER_STATE_MANAGER_H_
#define _PLAYER_STATE_MANAGER_H_

class PlayerState;
class PlayerStateManager
{
public:
	PlayerStateManager(void);
	~PlayerStateManager(void);

	void Update(float dt);
	void SetPlayerState(PlayerState* state);
	void BlowBubble(bool value);
	void Init();
	PlayerState* GetPlayerState(int type);
	bool IsBlowingBubbles() const { return m_blow_bubbles; }
	int GetCurrentType();
	void AddPlayerState(PlayerState* state) { m_player_states.push_back(state); }
	void Shut();

private:
	PlayerState* m_active;
	PlayerState* m_blow_bubble_state;
	std::vector<PlayerState*> m_player_states;
	bool m_blow_bubbles;
};

#endif // _PLAYER_STATE_MANAGER_H_