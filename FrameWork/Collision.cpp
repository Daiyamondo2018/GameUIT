﻿#include "Collision.h"
#include <iostream>
using namespace std;

Collision* Collision::instance = NULL;


Collision* Collision::getInstance()
{
	if (!instance)
		instance = new Collision();
	return instance;
}

collisionOut Collision::GetOppositeSide(collisionOut & input)
{
	collisionOut output = input;
	//Hướng va chạm ngược so với object kia.
	if (input.side == CollisionSide::left)
		output.side = CollisionSide::right;

	else if (input.side == CollisionSide::bottom)
		output.side = CollisionSide::top;

	else if (input.side == CollisionSide::top)
		output.side = CollisionSide::bottom;

	else if (input.side == CollisionSide::right)
		output.side = CollisionSide::left;

	else
		input.side = output.side;
	output.collisionTime = input.collisionTime;
	return output;
}

collisionOut Collision::SweptAABB(BoundingBox recta, BoundingBox rectb)
{
	collisionOut out;
	out.collisionTime = 2;
	out.side = CollisionSide::none;
	recta.vx = recta.vx - rectb.vx;
	recta.vy = recta.vy - rectb.vy;
	
	BoundingBox test;
	test.top = recta.vy > 0 ? recta.top + recta.vy : recta.top;
	test.bottom = recta.vy > 0 ? recta.bottom : recta.bottom + recta.vy;
	test.left = recta.vx > 0 ? recta.left : recta.left + recta.vx;
	test.right = recta.vx > 0 ? recta.right + recta.vx : recta.right;
	if (!IsCollide(test, rectb))
	{
		return out;
	}
	if (recta.vx > 0.0f)
	{
		dxEntry = rectb.left - recta.right;
		dxExit = rectb.right - recta.left;
	}
	else
	{
		dxEntry = rectb.right - recta.left;
		dxExit = rectb.left - recta.right;
		
	}
	if (recta.vy > 0.0f)
	{
		dyEntry = rectb.bottom - recta.top;
		dyExit = rectb.top - recta.bottom;
	}
	else
	{
		dyEntry = rectb.top - recta.bottom;
		dyExit = rectb.bottom - recta.top;
		
	}

	
	if (recta.vx == 0.0f)
	{
		txEntry = -std::numeric_limits<float>::infinity();
		txExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		txEntry = dxEntry / recta.vx;
		txExit = dxExit / recta.vx;
	}
	if (recta.vy == 0.0f)
	{
		tyEntry = -std::numeric_limits<float>::infinity();
		tyExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		tyEntry = dyEntry / recta.vy;
		tyExit = dyExit / recta.vy;
	}

	float entryTime = max(txEntry, tyEntry);
	float exitTime = min(tyExit, txExit);
	if (entryTime > exitTime || (txEntry < 0.0f&&tyEntry < 0.0f) || txEntry > 1.0f || tyEntry > 1.0f)
		return out;
	out.collisionTime = entryTime;
	if (txEntry > tyEntry)
	{
		if (dxEntry < 0)
			out.side = CollisionSide::left;
		else if(dxEntry==0)
		{
			if (recta.vx < 0)
				out.side == CollisionSide::left;
			else
			{
				out.side = CollisionSide::right;
			}
		}
		else
		{
			out.side = CollisionSide::right;
		}
	}
	else
	{
		if (dyEntry < 0)
			out.side = CollisionSide::bottom;
		else if(dyEntry==0)
		{
			if (recta.vy < 0)
				out.side  = CollisionSide::bottom;
			else
			{
				out.side = CollisionSide::top;
			}
		}
		else
		{
			out.side = CollisionSide::top;
		}
	}
	return out;
}

collisionOut Collision::SweptAABB(BoundingBox recta, BoundingBox rectb, float acceleratorA) {
	recta.vy += acceleratorA;
	return SweptAABB(recta, rectb);
}

bool Collision::IsCollide(BoundingBox box1, BoundingBox box2)
{
	return !(box1.left > box2.right || box1.right < box2.left || box1.top < box2.bottom || box1.bottom > box2.top);
}

bool Collision::IsCollide(BoundingBox box1, BoundingBox box2, CollisionSide* prioritySide) {
	if (!IsCollide(box1, box2)) {
		*prioritySide = CollisionSide::none;
		return false;
	}
	if (box1.left <= box2.left) {
		float dentaXOut = box1.left - box2.left;
		float dentaXIn = box1.right - box2.left;
		float dentaYIn = box1.bottom - box2.top;
		float dentaYOut = box1.top - box2.top;
		if (abs(dentaXIn / dentaXOut ) <= 0.75) {
			*prioritySide = CollisionSide::right;
		}
		else {
			if (box1.top > box2.top) {
				
				*prioritySide = CollisionSide::bottom;
			}
			else {
				*prioritySide = CollisionSide::top;
			}
		}
		return true;
	}
	if (box1.right >= box2.right) {
		float dentaXOut = box1.right - box2.right;
		float dentaXIn = box1.left - box2.right;
		float dentaY = box1.top - box2.top;
		if (abs(dentaXIn / dentaXOut) <= 0.75) {
			*prioritySide = CollisionSide::left;
		}
		else {
			if (box1.top > box2.top) {
				*prioritySide = CollisionSide::bottom;
			}
			else {
				*prioritySide = CollisionSide::top;
			}
		}
		return true;
	}
	if (box1.top > box2.top) {
		*prioritySide = CollisionSide::bottom;
		return true;
	}
	if (box1.bottom < box2.bottom) {
		*prioritySide = CollisionSide::top;
	}

	//*prioritySide = CollisionSide::none;
	return true;
}