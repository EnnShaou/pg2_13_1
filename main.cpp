#include <Novice.h>
#include"Vector2.h"
#include<vector>
#include<memory>
const char kWindowTitle[] = "GC1D_03_エン_シャオウ_";
const int kWindowWidth = 1280;//画面の横幅
const int kWindowHeight = 720;//画面の縦幅

class Character
{
public:
	static bool isAlive;
	/// <summary>
	/// 状態代入
	/// </summary>
	/// <param name="state"></param>
	static void SetIsAlive(bool state) {
		isAlive = state;
	}
	Character(Vector2 pos) :
		pos_(pos),
		speed_(6.0f, 0.0f),
		radius(16) {}
	~Character() {}
	/// <summary>
	/// 移動
	/// </summary>
	/// <param name="k">KEY</param>
	/// <param name="pK">PREKEY</param>
	virtual void move(char* k, char* pK) {
		if (!k || !pK)
		{
			return;
		}
	}
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="color">色</param>
	virtual void Draw(unsigned int color) {
		Novice::DrawEllipse(static_cast<int>(pos_.x), static_cast<int>(pos_.y), radius, radius, 0.0f, color, kFillModeSolid);
	}
	/// <summary>
	/// 座標取る
	/// </summary>
	/// <returns>座標値</returns>
	Vector2 GetPos() { return pos_; }
	/// <summary>
	/// スピード取る
	/// </summary>
	/// <returns>スピード値</returns>
	Vector2 GetSpeed() { return speed_; }
	/// <summary>
	/// 半径を取る
	/// </summary>
	/// <returns>半径値</returns>
	int GetRadius() { return radius; }
protected:
	Vector2 pos_;
	Vector2 speed_;
	int radius;
};
bool Character::isAlive = true;

class Bullet :public Character
{
public:
	Bullet(Vector2 pos) :Character(pos) {
		speed_ = Vector2(0.0f, 6.0f);
		radius = 4;
	}
	~Bullet() {}
	/// <summary>
	/// 弾移動
	/// </summary>
	/// <param name="k"></param>
	/// <param name="pK"></param>
	void move(char* k, char* pK)override {
		if (!k || !pK)
		{
			return;
		}
		pos_.y -= speed_.y;
	}
	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="c">目標</param>
	/// <returns>目標との判定</returns>
	bool IsCircleColliding(Character c) {
		Vector2 d = pos_ - c.GetPos();
		float distanceSquared = d.length();
		int radiusSum = c.GetRadius() + radius;
		return distanceSquared < radiusSum;
	}
private:

};

class Enemy :public Character
{
public:

	Enemy(Vector2 pos) :Character(pos) { isAlive = true; }
	~Enemy() { isAlive = false; }
	/// <summary>
	/// 敵移動
	/// </summary>
	void move(char* k, char* pK)override {

		if (!k || !pK)
		{
			return;
		}
		if (isAlive)
		{
			pos_ += speed_;
			if (pos_.x<0.0f || pos_.x>kWindowWidth)
			{
				speed_ *= -1;
			}
		}
	}
	/// <summary>
	/// 敵描画
	/// </summary>
	/// <param name="color"></param>
	void Draw(unsigned int color)override {
		if (isAlive)
		{
			Novice::DrawEllipse(static_cast<int>(pos_.x), static_cast<int>(pos_.y), radius, radius, 0.0f, color, kFillModeSolid);
		}
	}

private:

};
class Player :public Character
{
public:
	Player(Vector2 pos) :
		Character(pos) {}
	~Player() {
	}
	/// <summary>
	/// プレーヤ移動
	/// </summary>
	/// <param name="k"></param>
	/// <param name="pK"></param>
	void move(char* k, char* pK)override {
		if (!k || !pK)
		{
			return;
		}
		if (k[DIK_D] && pos_.x < kWindowWidth)pos_.x += speed_.x;
		if (k[DIK_A] && pos_.x > 0.0f)pos_.x -= speed_.x;
		Shot(k, pK);

		for (auto& b : bullet)
		{
			b->move(k, pK);
		}
	}
	/// <summary>
	/// 弾発射
	/// </summary>
	/// <param name="k"></param>
	/// <param name="pK"></param>
	void Shot(char* k, char* pK) {
		if (k[DIK_SPACE] && !pK[DIK_SPACE])
		{
			bullet.push_back(std::make_unique<Bullet>(pos_));
		}
	}
	/// <summary>
	/// 弾消す
	/// </summary>
	/// <param name="enemy">敵データ</param>
	void BulletDead(Character* enemy) {

		for (auto it = bullet.begin(); it != bullet.end();)
		{
			if ((*it)->IsCircleColliding(*enemy) && Enemy::isAlive)
			{
				Enemy::SetIsAlive(false);
				it = bullet.erase(it);
			}
			else
			{
				if ((*it)->GetPos().y < 0.0f)
				{
					it = bullet.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}
	/// <summary>
	/// プレーヤ描画
	/// </summary>
	/// <param name="color"></param>
	void Draw(unsigned int color)override {
		Novice::DrawEllipse(static_cast<int>(pos_.x), static_cast<int>(pos_.y), radius, radius, 0.0f, color, kFillModeSolid);
		for (const auto& b : bullet)
		{
			b->Draw(WHITE);
		}
		Novice::ScreenPrintf(0, 0, "bullet number : %d", bullet.size());
	}
private:
	std::vector<std::unique_ptr<Bullet>>bullet;
};
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
	const int enemyMax = 2;
	Player* player = new Player(Vector2(400.0f, 400.0f));
	Character* enemy[enemyMax];
	for (int i = 0; i < enemyMax; i++)
	{
		enemy[i] = new Enemy(Vector2(200.0f + i * 100.0f, 200.0f + i * 100.0f));
	}
	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		player->move(keys, preKeys);
		for (int i = 0; i < enemyMax; i++)
		{
			enemy[i]->move(keys, preKeys);
			player->BulletDead(enemy[i]);

		}
		if (keys[DIK_R] && !preKeys[DIK_R])
		{
			Enemy::SetIsAlive(true);
		}
		///
		/// ↑更新処理ここまで
		///
		player->Draw(BLUE);
		for (int i = 0; i < enemyMax; i++)
		{
			enemy[i]->Draw(RED);
		}
		Novice::ScreenPrintf(0, 20, "enemyA : %d", enemy[0]->isAlive);
		Novice::ScreenPrintf(0, 40, "enemyB : %d", enemy[1]->isAlive);
		Novice::ScreenPrintf(0, 60, "Press R to reset");
		///
		/// ↓描画処理ここから
		///

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}
	delete player;
	for (int i = 0; i < enemyMax; i++)
	{
		delete enemy[i];
	}
	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
