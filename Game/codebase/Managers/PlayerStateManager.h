#ifndef _PLAYER_STATE_MANAGER_H_
#define _PLAYER_STATE_MANAGER_H_

class PlayerState;
class PlayerInsideBubble;
class PlayerStateManager
{
public:
	PlayerStateManager(void);
	~PlayerStateManager(void);

	void Update(float dt);
	void SetPlayerState(PlayerState* state);
	void BlowBubble(bool value);
	void HoldObject(bool value);
	void GoInsideBubble(bool value);
	void Init();
	PlayerState* GetPlayerState(int type);
	bool IsBlowingBubbles() const { return m_blow_bubbles; }
	bool IsHoldingObject() const { return m_holding_object; }
	bool IsInsideBubble() const { return m_is_inside_bubble; }

	int GetCurrentType();
	void AddPlayerState(PlayerState* state) { m_player_states.push_back(state); }
	void Shut();
	PlayerInsideBubble* GetInsideBubbleState() { return m_inside_bubble; }

private:
	PlayerState* m_active;
	PlayerState* m_blow_bubble_state;
	PlayerState* m_holding_object_state;
	PlayerInsideBubble* m_inside_bubble;
	std::vector<PlayerState*> m_player_states;
	bool m_blow_bubbles;
	bool m_holding_object;
	bool m_is_inside_bubble;
};

#endif // _PLAYER_STATE_MANAGER_H_