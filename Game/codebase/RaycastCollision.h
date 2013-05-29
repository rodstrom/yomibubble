#ifndef _RAYCAST_COLLISION_H_
#define _RAYCAST_COLLISION_H_

class IgnoreBodyCast : public btCollisionWorld::ClosestRayResultCallback{
public:
	IgnoreBodyCast(btRigidBody* body) :
		btCollisionWorld::ClosestRayResultCallback(btVector3(0,0,0), btVector3(0,0,0)),
		m_body(body){}

	btScalar AddSingleResult(btCollisionWorld::LocalRayResult& ray_result, bool normal_in_world_space){
		if (ray_result.m_collisionObject == m_body){
			return 1.0f;
		}
		return ClosestRayResultCallback::addSingleResult(ray_result, normal_in_world_space);
	}

private:
	btRigidBody* m_body;
};

class IgnoreBodyAndBubbleCast : public btCollisionWorld::ClosestRayResultCallback{
public:
	IgnoreBodyAndBubbleCast(btRigidBody* body, btRigidBody* bubble_body) :
		btCollisionWorld::ClosestRayResultCallback(btVector3(0,0,0), btVector3(0,0,0)),
		m_body(body), m_bubble_body(bubble_body){}

	btScalar AddSingleResult(btCollisionWorld::LocalRayResult& ray_result, bool normal_in_world_space){
		if (ray_result.m_collisionObject == m_body || ray_result.m_collisionObject == m_bubble_body){
			return 1.0f;
		}
		return ClosestRayResultCallback::addSingleResult(ray_result, normal_in_world_space);
	}

private:
	btRigidBody* m_body;
	btRigidBody* m_bubble_body;
};

#endif //_RAYCAST_COLLISION_H_