
#include"Camera.h"
#include"Shield.h"



Shield * Shield::instance = NULL;

BoundingBox Shield::getBoundingBox() {
	BoundingBox bounding;
	auto index = this->animation->curframeindex;

	if (index == 0) {
		bounding.top = this->pos.y + 7;
		bounding.left = this->pos.x - 4;
		bounding.right = this->pos.x + 4;
		bounding.bottom = this->pos.y - 7;
	}
	if (index == 1) {
		bounding.top = this->pos.y + 7;
		bounding.left = this->pos.x - 8;
		bounding.right = this->pos.x + 8;
		bounding.bottom = this->pos.y - 7;
	}
	if (index == 2) {
		bounding.top = this->pos.y + 3;
		bounding.left = this->pos.x - 8;
		bounding.right = this->pos.x + 8;
		bounding.bottom = this->pos.y - 4;
	}
	if (index == 3) {
		bounding.top = this->pos.y + 5;
		bounding.left = this->pos.x - 12;
		bounding.right = this->pos.x + 12;
		bounding.bottom = this->pos.y - 5;
	}
	bounding.vx = this->vx;
	bounding.vy = this->vy;
	return bounding;
}

Shield::Shield()
{
	Player* player = Player::getInstance();
	if (!player)
		return;
	this->tag = Tag::SHIELD;
	this->vx = this->vy = 0;
	this->accelerator = D3DXVECTOR2(0, 0);
	//sprites = SpriteManager::getInstance()->getSprites(this->tag, 0, 4);
	animation = new Animation(this->tag, 0, 4);
	Player::MoveDirection direction = Player::getInstance()->GetMoveDirection();
	this->SetShieldState(Shield::ShieldState::Defense);
	this->beginRound = TRUE;


	this->SetNumberOfRounds(1);
	this->SetFramePerRound(50);
}

Shield* Shield::getInstance()
{
	if (!instance)
		instance = new Shield();
	return instance;
}

Shield::~Shield()
{
}

//xư lí sự kiện nhấn phím cho shield
void Shield::InputHandler(float dt)
{
	KeyboardManager* keyboard = KeyboardManager::getInstance();
	Player* player = Player::getInstance();
	if (keyboard->getKeyPressedOnce(PLAYER_ATTACK) && player->hasShield && player->state != State::KICKING)
	{
		player->hasShield = false;
		switch (player->GetMoveDirection())
		{
		case Player::MoveDirection::LeftToRight:
		{
			this->vx = SHIELD_SPEED;
			this->direction = MoveDirection::LeftToRight;
			break;
		}
		case Player::MoveDirection::RightToLeft:
		{
			this->vx = -SHIELD_SPEED;
			this->direction = MoveDirection::RightToLeft;
			break;
		}
		default:
			break;
		}
	}
}

void Shield::setFrameIndex(int index)
{
	this->animation->curframeindex = index;
}

void Shield::Update(float dt)
{
	Player* player = Player::getInstance();
	if (!player)
		return;
	Player::MoveDirection direction = Player::getInstance()->GetMoveDirection();// lấy hướng di chuyển của player player
	//kiểm tra nếu player đang giữ shield
	if (player->hasShield) {
		//nếu là shield up thì đổi sprite index
		if (player->state == State::SHIELD_UP)
		{
			this->setFrameIndex(2);
			this->pos.y = Player::getInstance()->getBoundingBox().top - 2;
			this->pos.x = Player::getInstance()->pos.x;
			return;
		}
		else 
			if(player->state==State::SHIELD_DOWN)
		{
			this->animation->curframeindex = 3;
			this->pos.x = player->pos.x;
			this->pos.y = player->getBoundingBox().bottom;
			return;
		}
		else
		{
			this->animation->curframeindex = 0;
		}
		//player đang ở trạng thái jump
		if (player->state == State::JUMPING)
		{
			this->animation->curframeindex = 1;
			this->pos.y = player->pos.y + 9;
			switch (direction)
			{
			case Player::MoveDirection::LeftToRight:
			{
				this->pos.x = player->pos.x - 3;//shield ở vị trí bên phải của player
				break;
			}
			case Player::MoveDirection::RightToLeft:
			{
				this->pos.x = player->pos.x + 3;//shield ở vị trí bên trái của player
				break;
			}
			default:
				break;
			}
			return;
		}
		//player đang ở trạng thái kick
		if (player->state == State::KICKING)
		{
			this->animation->curframeindex = 0;
			if (direction == Player::MoveDirection::LeftToRight)
			{
				this->pos.y = player->pos.y;
				this->pos.x = player->pos.x - 10;
			}
			else if (direction == Player::MoveDirection::RightToLeft)
			{
				this->pos.y = player->pos.y;
				this->pos.x = player->pos.x + 10;
			}
			return;
		}
		//player đang ở trạng thái stand
		if (player->state == State::STANDING)
		{
			this->pos.y = player->pos.y + 7;
			switch (direction)
			{
			case Player::LeftToRight:
			{
				this->pos.x = player->playerstate->getBoundingBox().right - 2;//shield ở vị trí bên phải của player
				break;
			}
			case Player::RightToLeft:
			{
				this->pos.x = player->playerstate->getBoundingBox().left + 2;//shield ở vị trí bên trái của player
				break;
			}
			default:
				break;
			}
			return;
		}
		//player đang ở trạng thái run
		if (player->state == State::RUNNING)
		{
			switch (direction)
			{
			case Player::MoveDirection::LeftToRight:
			{
				this->pos.x = player->playerstate->getBoundingBox().right + 1;//shield ở vị trí bên phải của player
				this->pos.y = player->pos.y + 10;
				break;
			}
			case Player::MoveDirection::RightToLeft:
			{
				this->pos.x = player->playerstate->getBoundingBox().left;//shield ở vị trí bên trái của player
				this->pos.y = player->pos.y + 10;
				break;
			}
			default:
				break;
			}
			return;
		}
		//player đang ở trạng thái sit
		if (player->state == State::DUCKING)
		{
			switch (direction)
			{
			case Player::MoveDirection::LeftToRight:
			{
				this->pos.x = player->playerstate->getBoundingBox().right - 2;//shield ở vị trí bên phải của player
				this->pos.y = player->pos.y - 10;
				break;
			}
			case Player::MoveDirection::RightToLeft:
			{
				this->pos.x = player->playerstate->getBoundingBox().left + 1;//shield ở vị trí bên trái của player
				this->pos.y = player->pos.y - 10;
				break;
			}
			default:
				break;
			}
			return;
		}
		if (player->state == State::SHIELD_ATTACK) {
			this->animation->curframeindex = 2;
			switch (direction)
			{
				case Player::MoveDirection::LeftToRight:
				{
					this->pos.x = player->pos.x - 20;//shield ở vị trí bên phải của player
					this->pos.y = player->pos.y + 16;
					break;
				}
				case Player::MoveDirection::RightToLeft:
				{
					this->pos.x = player->pos.x + 20;//shield ở vị trí bên trái của player
					this->pos.y = player->pos.y + 16;
					break;
				}
				default:
					break;
				}
			return;
		}
		if (this->state == ShieldState::NotRender) {
			this->pos = player->pos;
		}
	}
	//shield đã được player dùng để tấn công
	else
	{
		//xử lí update khi khiêng đang tấn công
		//update vị trí dự theo vị trí player
		if (this->state == ShieldState::Attack) {
			this->animation->curframeindex = 2;
			this->Move();
		}
	}
}

void Shield::Render()
{
	Player* player = Player::getInstance();

	// nếu player NULL hoặc shield đang không được active
	if (!player || !this->GetActive())
		return;

	D3DXVECTOR3 pos = Camera::getCameraInstance()->convertWorldToViewPort(D3DXVECTOR3(this->pos.x, this->pos.y, 0));

	switch (this->direction) {
		case MoveDirection::LeftToRight: {
			this->animation->Render(D3DXVECTOR2(pos), TransformationMode::FlipHorizontal);
			break;
		}
		case MoveDirection::RightToLeft: {
			this->animation->Render(pos);
			break;
		}
	}
}

void Shield::OnCollision(Object* object, collisionOut* out)
{
	//xu li va cham
	if (object->tag == Tag::PLAYER) {
		auto player = Player::getInstance();
		switch (player->state) {
			case State::KICKING:
			case State::JUMPING: 
				this->SetShieldState(Shield::ShieldState::Transparent);
				break;
			case State::DUCKING:
			case State::DUCKING_PUNCHING:
			case State::STANDING:
			case State::STAND_PUNCH:
			case State::RUNNING:
				this->SetShieldState(ShieldState::Defense);
				break;
			case State::DIVING:
			case State::FLOATING:
			case State::ROLLING:
			case State::DASHING:
				this->SetShieldState(ShieldState::NotRender);
				break;
		}
	}
}

void Shield::SetShieldState(Shield::ShieldState state) {
	auto player = Player::getInstance();
	this->state = state;
	switch (this->state) {
	case Shield::ShieldState::Attack: {
		this->SetActive(TRUE);
		player->hasShield = FALSE;
		break;
	}
	case Shield::ShieldState::Defense: {
		this->SetActive(TRUE);
		player->hasShield = TRUE;
		break;
	}
	case Shield::ShieldState::Transparent: {
		this->SetActive(TRUE);
		player->hasShield = TRUE;
		break;
	}
	case Shield::ShieldState::NotRender: {
		this->SetActive(FALSE);
		player->hasShield = TRUE;
		break;
	}

	}
}

void Shield::SetNumberOfRounds(int numberOfRounds) {
	this->numberOfRounds = numberOfRounds;
}

void Shield::SetFramePerRound(int fpr) {
	this->framePerRound = fpr;
}

void Shield::Move() {
	auto player = Player::getInstance();
	if (this->beginRound) {
		// setup các thông số ban đầu
		if (this->direction == MoveDirection::LeftToRight) {
			this->vx = SHIELD_INITIAL_SPEED;
		}
		else {
			this->vx = -SHIELD_INITIAL_SPEED;
		}
		this->pos = player->pos;
		this->vy = 0;
		this->accelerator.x = -this->vx / (this->framePerRound / 2);
		this->accelerator.y = 0;
		this->restFrames = this->framePerRound / 2;
		this->moveBehave = MoveBehavior::FarFromPlayer;

		this->beginRound = FALSE;
	}
	else {
		if (this->moveBehave == MoveBehavior::FarFromPlayer) {
			if (!MoveOutFromPlayer(this->restFrames)) {
				this->moveBehave = MoveBehavior::BackToPlayer;
				this->restFrames = this->framePerRound / 2;
				// reset giá trị di chuyển
				this->vx = this->vy = 0;
				this->accelerator = D3DXVECTOR2(0, 0);
				return;
			}
		}
		else {
			if (this->moveBehave == MoveBehavior::BackToPlayer) {
				if (!MoveBackToPlayer(this->restFrames)) {
   					OnCollision(player, NULL);
					ResetMoveStatus();
				}
			}
		}
		this->restFrames--;
	}
}

void Shield::ResetMoveStatus() {
	this->beginRound = TRUE;
	this->vx = this->vy = 0;
	this->accelerator = D3DXVECTOR2(0, 0);
	this->moveBehave = MoveBehavior::NotMove;
}

BOOL Shield::MoveOutFromPlayer(int totalFrames) {
	// vẫn còn đang tiếp tục di chuyển
	if (totalFrames > 0) {
		UpdatePositionVector();
		return TRUE;
	}

	return FALSE;
}

void Shield::SetMoveDirection(MoveDirection moveDir) {
	this->direction = moveDir;
}

BOOL Shield::MoveBackToPlayer(int totalFrames) {
	auto player = Player::getInstance();
	if (totalFrames <= 0) {
		return FALSE;
	}
	auto returnPos = player->GetShieldReturnPos();
	float dentaX = returnPos.x - this->pos.x;
	float dentaY = returnPos.y - this->pos.y;
	
	this->vx = dentaX / totalFrames;
	this->vy = dentaY / totalFrames;

	UpdatePositionVector();

	return TRUE;
}

void Shield::UpdatePositionVector() {
	this->vx += this->accelerator.x;
	this->vy += this->accelerator.y;

	this->pos.x += this->vx;
	this->pos.y += this->vy;
}


